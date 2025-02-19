# Link Do Vídeo: https://youtube.com/shorts/sO8Nr08WDIc

# Projeto: Conversores A/D, PWM e Interface OLED no RP2040

## 🎯 Objetivos

- **ADC:** Ler os eixos X e Y do joystick, convertendo sinais analógicos em digitais.
- **PWM:** Controlar a intensidade dos LEDs RGB:
  - **LED Azul:** Brilho controlado pelo eixo Y.
  - **LED Vermelho:** Brilho controlado pelo eixo X.
- **Display OLED:** Exibir um quadrado de 8x8 pixels que se movimenta conforme o joystick.
- **Interrupções e Debouncing:** Implementar rotinas para os botões com tratamento de bouncing.
- **I2C:** Comunicar-se com o display SSD1306 via I2C.

---

## 🛠 Componentes Utilizados

- **Placa:** BitDogLab com microcontrolador RP2040.
- **Display OLED:** SSD1306 (128x64 pixels) – Comunicação I2C.
- **Joystick:**
  - Eixo X: ADC (GPIO 26)
  - Eixo Y: ADC (GPIO 27)
  - Botão: GPIO 22
- **Botão A:** GPIO 5
- **LED RGB:**
  - LED Vermelho: GPIO 12 (PWM)
  - LED Verde: GPIO 11 (PWM)
  - LED Azul: GPIO 13 (PWM)

---

## 🔧 Funcionamento do Projeto

1. **Leitura dos dados do Joystick**
   - Valores analógicos dos eixos X e Y são lidos e processados.
   - A posição do quadrado no display e a intensidade dos LEDs são ajustadas.

2. **Controle dos LEDs RGB via PWM**
   - LEDs vermelho e azul variam de intensidade conforme os valores lidos.
   - LED verde alterna entre ligado/desligado ao pressionar o botão do joystick.

3. **Movimentação do Quadrado no Display**
   - O quadrado é atualizado dinamicamente de acordo com os valores do joystick.
   - O botão do joystick altera o estilo da borda do display.

4. **Tratamento de Botões**
   - Implementação de interrupções e debouncing para garantir leituras precisas.

---

## ▶️ Como Utilizar

1. **Compilação e Upload:**
   - Utilize o ambiente de desenvolvimento compatível com o RP2040 (ex.: Pico SDK).
   - Compile e faça o upload do firmware para a placa BitDogLab.

2. **Operação:**
   - **Joystick:** Movimente para controlar a posição do quadrado e a intensidade dos LEDs.
   - **Botão do Joystick:** Alterna o LED verde e modifica a borda do display.
   - **Botão A:** Liga/desliga o controle PWM dos LEDs RGB.


## 📢 Conclusão

Este projeto explora os conceitos de ADC, PWM e comunicação I2C no RP2040, integrando joystick, LEDs e um display OLED para criar uma interface interativa e dinâmica. A implementação eficiente de interrupções e debouncing garante um desempenho otimizado e responsivo.

# PROJETO DESENVOLVIDO POR CÉSAR REBOUÇAS COSTA


