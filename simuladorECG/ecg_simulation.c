#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "ssd1306.h"

// Configurações do PWM
#define PWM_SLICE_NUM 0
#define PWM_CHANNEL 0
#define PWM_GPIO_PIN 15

// Configurações do Display OLED
#define I2C_PORT i2c0
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

// Inicializa o display OLED
ssd1306_t oled;

void init_display() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    ssd1306_init(&oled, 128, 64, I2C_PORT, 0x3C);
    ssd1306_clear(&oled);
    ssd1306_show(&oled);
}

// Função para inicializar o PWM
void init_pwm() {
    gpio_set_function(PWM_GPIO_PIN, GPIO_FUNC_PWM);
    pwm_config config = pwm_get_default_config();
    pwm_init(PWM_SLICE_NUM, &config, true);
}

// Função para gerar um sinal de ECG simulado
uint16_t generate_ecg_signal(uint32_t time) {
    // Simula um sinal de ECG usando uma função senoidal
    float frequency = 1.0; // Frequência do sinal em Hz
    float amplitude = 2048.0; // Amplitude do sinal
    float offset = 2048.0; // Offset do sinal
    return (uint16_t)(amplitude * sin(2.0 * M_PI * frequency * time / 1000.0) + offset);
}

// Função para atualizar o display OLED com o sinal de ECG
void update_display(uint16_t ecg_signal) {
    static uint8_t x_pos = 0;

    // Desloca a tela para a esquerda
    ssd1306_scroll_left(&oled, 0x00, 0x0F);

    // Desenha o novo ponto do sinal de ECG
    uint8_t y_pos = (uint8_t)((ecg_signal * 64) / 4096);
    ssd1306_draw_pixel(&oled, x_pos, y_pos, 1);
    ssd1306_show(&oled);

    // Atualiza a posição x para o próximo ponto
    x_pos = (x_pos + 1) % 128;
}

int main() {
    stdio_init_all();
    init_pwm();
    init_display();

    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    
    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time()) - start_time;
        uint16_t ecg_signal = generate_ecg_signal(current_time);

        // Atualiza o PWM com o valor do sinal de ECG gerado
        pwm_set_gpio_level(PWM_GPIO_PIN, ecg_signal);
        
        // Exibe o valor do sinal de ECG no terminal
        printf("ECG Signal: %d\n", ecg_signal);
        
        // Atualiza o display OLED com o sinal de ECG
        update_display(ecg_signal);
        
        // Aguarda um pequeno intervalo de tempo
        sleep_ms(1);
    }

    return 0;
}