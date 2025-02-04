#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Configurações dos pinos para os LEDs RGB e o botão
const uint led_red_pin = 11;    // Pino do LED vermelho
const uint led_yellow_pin = 12; // Pino do LED amarelo
const uint led_green_pin = 13;  // Pino do LED verde
const uint button_pin = 5;      // Pino do botão

// Variáveis globais
volatile bool button_pressed = false;  // Flag para indicar se o botão foi pressionado
volatile bool timer_active = false;    // Flag para indicar se o temporizador está ativo
volatile uint8_t led_state = 0;        // Estado atual dos LEDs

// Função de callback para o temporizador
bool turn_off_callback(struct repeating_timer *t) {
    if (led_state == 3) {
        gpio_put(led_yellow_pin, 0); // Desliga o LED amarelo
        led_state = 2;
    } else if (led_state == 2) {
        gpio_put(led_red_pin, 0); // Desliga o LED vermelho
        led_state = 1;
    } else if (led_state == 1) {
        gpio_put(led_green_pin, 0); // Desliga o LED verde
        led_state = 0;
        timer_active = false; // Desativa o temporizador
    }
    return timer_active; // Retorna true se o temporizador ainda estiver ativo
}

// Função de callback para o botão
void button_callback(uint gpio, uint32_t events) {
    static absolute_time_t last_press_time = 0;
    absolute_time_t current_time = get_absolute_time();
    if (absolute_time_diff_us(last_press_time, current_time) > 200000) { // Debounce de 200 ms
        last_press_time = current_time;
        if (!timer_active) {
            // Liga todos os LEDs
            gpio_put(led_red_pin, 1);
            gpio_put(led_yellow_pin, 1);
            gpio_put(led_green_pin, 1);
            led_state = 3;
            timer_active = true;
            add_alarm_in_ms(3000, turn_off_callback, NULL, true);
        }
    }
}

int main() {
    // Inicializa o padrão de entrada/saída
    stdio_init_all();

    // Inicializa os pinos dos LEDs
    gpio_init(led_red_pin);
    gpio_set_dir(led_red_pin, GPIO_OUT);
    gpio_put(led_red_pin, 0);

    gpio_init(led_yellow_pin);
    gpio_set_dir(led_yellow_pin, GPIO_OUT);
    gpio_put(led_yellow_pin, 0);

    gpio_init(led_green_pin);
    gpio_set_dir(led_green_pin, GPIO_OUT);
    gpio_put(led_green_pin, 0);

    // Inicializa o pino do botão
    gpio_init(button_pin);
    gpio_set_dir(button_pin, GPIO_IN);
    gpio_pull_up(button_pin);
    gpio_set_irq_enabled_with_callback(button_pin, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    // Loop principal
    while (1) {
        if (timer_active) {
            printf("Temporizador ativo: LED %s ainda ligado.\n", led_state == 3 ? "amarelo" : led_state == 2 ? "vermelho" : "verde");
        }
        sleep_ms(1000); // Imprime uma mensagem a cada segundo
    }

    return 0;
}
