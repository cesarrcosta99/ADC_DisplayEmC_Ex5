#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"

// Configurações de Hardware
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define OLED_ADDR 0x3C

#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27
#define JOYSTICK_PB 22
#define BOTAO_A 5

#define LED_R 12
#define LED_G 11
#define LED_B 13

// Parâmetros do Display
#define SQUARE_SIZE 8
#define CENTER_X 60
#define CENTER_Y 28
#define SCALE_FACTOR 41  // Valor otimizado

// Valores de centro calibrados (ajuste conforme seu joystick)
#define CENTRO_X_ADC 2065  // Valor central X
#define CENTRO_Y_ADC 2050  // Valor central Y
#define DEADZONE 100       // Zona morta para centro

// Variáveis Globais
volatile bool pwm_enabled = true;
volatile bool led_g_state = false;
volatile uint8_t border_style = 0;
volatile uint32_t last_press_time = 0;
int square_x = CENTER_X;
int square_y = CENTER_Y;

// Estruturas para PWM
uint slice_r, slice_g, slice_b;

// Protótipos de Função
void setup_pwm();
void setup_gpio();
void update_leds(uint16_t x, uint16_t y);
void draw_interface(ssd1306_t *ssd);
void handle_joystick_pb();
void handle_botao_a();
bool debounce();

// Interrupções
void gpio_callback(uint gpio, uint32_t events) {
    if(debounce()) {
        if(gpio == JOYSTICK_PB) handle_joystick_pb();
        if(gpio == BOTAO_A) handle_botao_a();
    }
}

int main() {
    stdio_init_all();
    
    // Configuração PWM LEDs
    setup_pwm();
    
    // Configuração GPIO e Interrupções
    setup_gpio();
    
    // Configuração I2C e Display
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    ssd1306_t ssd;
    ssd1306_init(&ssd, 128, 64, false, OLED_ADDR, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Configuração ADC
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    while(true) {
        // Leitura do Joystick
        adc_select_input(0);
        uint16_t x = adc_read();
        adc_select_input(1);
        uint16_t y = adc_read();

        // Debug: descomente para ver valores reais
        // printf("X: %4u (C: %4d) | Y: %4u (C: %4d)\n", 
        //        x, x-CENTRO_X_ADC, y, y-CENTRO_Y_ADC);

        // Atualização dos LEDs
        if(pwm_enabled) update_leds(x, y);
        
        // Cálculo da posição com escala correta
        int delta_x = (int)x - CENTRO_X_ADC;
        int delta_y = (int)y - CENTRO_Y_ADC;
        
        square_x = CENTER_X - (delta_x / SCALE_FACTOR);
        square_y = CENTER_Y + (delta_y / SCALE_FACTOR);

        // Limites precisos do display
        square_x = (square_x < 0) ? 0 : (square_x > (128 - SQUARE_SIZE)) ? (128 - SQUARE_SIZE) : square_x;
        square_y = (square_y < 0) ? 0 : (square_y > (64 - SQUARE_SIZE)) ? (64 - SQUARE_SIZE) : square_y;
        
        // Atualização do display
        draw_interface(&ssd);
        sleep_ms(16); // ~60 FPS
    }
}

void setup_pwm() {
    gpio_set_function(LED_R, GPIO_FUNC_PWM);
    gpio_set_function(LED_G, GPIO_FUNC_PWM);
    gpio_set_function(LED_B, GPIO_FUNC_PWM);

    slice_r = pwm_gpio_to_slice_num(LED_R);
    slice_g = pwm_gpio_to_slice_num(LED_G);
    slice_b = pwm_gpio_to_slice_num(LED_B);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, 4095);
    
    pwm_init(slice_r, &config, true);
    pwm_init(slice_g, &config, true);
    pwm_init(slice_b, &config, true);
}

void setup_gpio() {
    gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_pull_up(JOYSTICK_PB);
    gpio_pull_up(BOTAO_A);
}

void update_leds(uint16_t x, uint16_t y) {
    // Cálculo com zona morta central
    int16_t red = abs((int)x - CENTRO_X_ADC);
    int16_t blue = abs((int)y - CENTRO_Y_ADC);
    
    // Aplicar deadzone para evitar luz residual
    red = (red < DEADZONE) ? 0 : red - DEADZONE;
    blue = (blue < DEADZONE) ? 0 : blue - DEADZONE;
    
    // Mapear para 0-4095
    red = (red * 16) > 4095 ? 4095 : red * 16;
    blue = (blue * 16) > 4095 ? 4095 : blue * 16;
    
    pwm_set_gpio_level(LED_R, red);
    pwm_set_gpio_level(LED_B, blue);
    pwm_set_gpio_level(LED_G, led_g_state ? 4095 : 0);
}

void draw_interface(ssd1306_t *ssd) {
    ssd1306_fill(ssd, false);
    
    // Bordas
    switch(border_style % 3) {
        case 0: ssd1306_rect(ssd, 0, 0, 128, 64, true, false); break;
        case 1: ssd1306_rect(ssd, 2, 2, 124, 60, true, false); break;
        case 2: 
            ssd1306_rect(ssd, 0, 0, 128, 64, true, false);
            ssd1306_rect(ssd, 4, 4, 120, 56, true, false);
            break;
    }
    
    // Quadrado móvel
    ssd1306_rect(ssd, square_x, square_y, SQUARE_SIZE, SQUARE_SIZE, true, true);
    
    ssd1306_send_data(ssd);
}

void handle_joystick_pb() {
    led_g_state = !led_g_state;
    border_style++;
}

void handle_botao_a() {
    pwm_enabled = !pwm_enabled;
    pwm_set_enabled(slice_r, pwm_enabled);
    pwm_set_enabled(slice_g, pwm_enabled);
    pwm_set_enabled(slice_b, pwm_enabled);
}

bool debounce() {
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if(now - last_press_time > 200) {
        last_press_time = now;
        return true;
    }
    return false;
}