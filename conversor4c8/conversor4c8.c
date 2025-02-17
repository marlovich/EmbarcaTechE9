/* 
Código para tarefa da Unidade 4 capítulo 8

Descrição do Projeto:
    O joystick fornecerá valores analógicos correspondentes aos eixos X e Y, que serão utilizados para:
Controlar a intensidade luminosa dos LEDs RGB, onde:
• O LED Azul terá seu brilho ajustado conforme o valor do eixo Y. Quando o joystick estiver solto
(posição central - valor 2048), o LED permanecerá apagado. À medida que o joystick for movido para
cima (valores menores) ou para baixo (valores maiores), o LED aumentará seu brilho gradualmente,
atingindo a intensidade máxima nos extremos (0 e 4095).
• O LED Vermelho seguirá o mesmo princípio, mas de acordo com o eixo X. Quando o joystick estiver
solto (posição central - valor 2048), o LED estará apagado. Movendo o joystick para a esquerda
(valores menores) ou para a direita (valores maiores), o LED aumentará de brilho, sendo mais intenso
nos extremos (0 e 4095).
• Os LEDs serão controlados via PWM para permitir variação suave da intensidade luminosa.
Exibir no display SSD1306 um quadrado de 8x8 pixels, inicialmente centralizado, que se moverá
proporcionalmente aos valores capturados pelo joystick.
Adicionalmente, o botão do joystick terá as seguintes funcionalidades:
• Alternar o estado do LED Verde a cada acionamento.
• Modificar a borda do display para indicar quando foi pressionado, alternando entre diferentes estilos
de borda a cada novo acionamento.
• O botão A terá a seguinte funcionalidade: Ativar ou desativar os LED PWM a cada acionamento.
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

// Definição dos pinos
#define PIN_LED_R     11      // LED Vermelho
#define PIN_LED_G     12      // LED Verde
#define PIN_LED_B     13      // LED Azul
#define PIN_BTN_A     5       // Botão A
#define PIN_JOY_BTN   22      // Botão do Joystick
#define PIN_JOY_X     26      // Eixo X do Joystick (ADC0)
#define PIN_JOY_Y     27      // Eixo Y do Joystick (ADC1)

// Definição do I2C
#define I2C_PORT      i2c0
#define I2C_SDA       14
#define I2C_SCL       15

// Configurações do display
#define SSD1306_H
#define DISPLAY_WIDTH     128
#define DISPLAY_HEIGHT    64

// Variáveis globais
volatile bool led_pwm_enabled = true;
volatile bool led_green_state = false;
volatile uint8_t border_style = 0;

// Instância do display
ssd1306_t display;

// Protótipos de funções
void gpio_callback(uint gpio, uint32_t events);
void init_pwm(uint pin);
void set_pwm_duty(uint pin, uint16_t duty);
void debounce(gpio_irq_callback_t callback);

// Função principal
int main() {
    stdio_init_all();

    // Inicialização dos GPIOs dos LEDs
    gpio_init(PIN_LED_R);
    gpio_set_function(PIN_LED_R, GPIO_FUNC_PWM);
    init_pwm(PIN_LED_R);

    gpio_init(PIN_LED_B);
    gpio_set_function(PIN_LED_B, GPIO_FUNC_PWM);
    init_pwm(PIN_LED_B);

    gpio_init(PIN_LED_G);
    gpio_set_dir(PIN_LED_G, GPIO_OUT);
    gpio_put(PIN_LED_G, led_green_state);

    // Inicialização dos botões com pull-up e interrupções
    gpio_init(PIN_BTN_A);
    gpio_set_dir(PIN_BTN_A, GPIO_IN);
    gpio_pull_up(PIN_BTN_A);
    gpio_set_irq_enabled_with_callback(PIN_BTN_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_init(PIN_JOY_BTN);
    gpio_set_dir(PIN_JOY_BTN, GPIO_IN);
    gpio_pull_up(PIN_JOY_BTN);
    gpio_set_irq_enabled(PIN_JOY_BTN, GPIO_IRQ_EDGE_FALL, true);

    // Inicialização do ADC
    adc_init();
    adc_gpio_init(PIN_JOY_X);
    adc_gpio_init(PIN_JOY_Y);

    // Inicialização do I2C e do display
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&display, DISPLAY_WIDTH, DISPLAY_HEIGHT, I2C_PORT, 0x3C);

    // Limpa o display
    ssd1306_clear(&display);

    // Loop principal
    while (true) {
        // Leitura do joystick
        adc_select_input(0); // Eixo X
        uint16_t joy_x = adc_read();
        adc_select_input(1); // Eixo Y
        uint16_t joy_y = adc_read();

        // Mapeamento dos valores do joystick para PWM (0 a 4095)
        uint16_t duty_r = 0;
        uint16_t duty_b = 0;

        if (led_pwm_enabled) {
            // Para o LED Vermelho (Eixo X)
            if (joy_x < 2048) {
                duty_r = (2048 - joy_x) * 2;
            } else if (joy_x > 2048) {
                duty_r = (joy_x - 2048) * 2;
            }

            // Para o LED Azul (Eixo Y)
            if (joy_y < 2048) {
                duty_b = (2048 - joy_y) * 2;
            } else if (joy_y > 2048) {
                duty_b = (joy_y - 2048) * 2;
            }
        }

        // Ajuste dos PWM dos LEDs
        set_pwm_duty(PIN_LED_R, duty_r);
        set_pwm_duty(PIN_LED_B, duty_b);

        // Atualização do quadrado no display
        int8_t square_x = (joy_x * (DISPLAY_WIDTH - 8)) / 4095;
        int8_t square_y = (joy_y * (DISPLAY_HEIGHT - 8)) / 4095;

        // Limpa o display
        ssd1306_clear(&display);

        // Desenha a borda conforme o estilo atual
        switch (border_style % 3) {
            case 0:
                // Borda simples
                ssd1306_draw_rectangle(&display, 0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, WHITE);
                break;
            case 1:
                // Borda dupla
                ssd1306_draw_rectangle(&display, 0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, WHITE);
                ssd1306_draw_rectangle(&display, 2, 2, DISPLAY_WIDTH - 3, DISPLAY_HEIGHT - 3, WHITE);
                break;
            case 2:
                // Sem borda
                break;
        }

        // Desenha o quadrado móvel
        ssd1306_draw_filled_rectangle(&display, square_x, square_y, square_x + 7, square_y + 7, WHITE);

        // Atualiza o display
        ssd1306_show(&display);

        // Pequeno atraso
        sleep_ms(50);
    }

    return 0;
}

// Função de callback para interrupções dos botões
void gpio_callback(uint gpio, uint32_t events) {
    static absolute_time_t last_time_a = {0};
    static absolute_time_t last_time_joy = {0};
    absolute_time_t now = get_absolute_time();

    if (gpio == PIN_BTN_A) {
        // Debouncing do Botão A
        if (absolute_time_diff_us(last_time_a, now) > 200000) { // 200 ms
            led_pwm_enabled = !led_pwm_enabled;
            printf("LEDs PWM %s\n", led_pwm_enabled ? "Ativados" : "Desativados");
            last_time_a = now;
        }
    } else if (gpio == PIN_JOY_BTN) {
        // Debouncing do Botão do Joystick
        if (absolute_time_diff_us(last_time_joy, now) > 200000) {
            led_green_state = !led_green_state;
            gpio_put(PIN_LED_G, led_green_state);
            printf("LED Verde %s\n", led_green_state ? "Ligado" : "Desligado");

            border_style++;
            last_time_joy = now;
        }
    }
}

// Função para inicializar o PWM em um pino específico
void init_pwm(uint pin) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();

    // Ajuste do clock para permitir valores de 0 a 4095
    pwm_config_set_clkdiv(&config, 4.0f);
    pwm_config_set_wrap(&config, 4095);

    pwm_init(slice_num, &config, true);
}

// Função para ajustar o duty cycle do PWM
void set_pwm_duty(uint pin, uint16_t duty) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_gpio_level(pin, duty);
}
