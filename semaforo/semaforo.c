#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

// Configurações dos pinos para os LEDs RGB
const uint led_red_pin = 11;    // Pino do LED vermelho
const uint led_yellow_pin = 12; // Pino do LED amarelo
const uint led_green_pin = 13;  // Pino do LED verde

// Variáveis globais
volatile uint8_t led_state = 0; // Estado atual do semáforo
volatile bool print_message = false; // Flag para imprimir mensagem

// Função de callback do temporizador
bool repeating_timer_callback(struct repeating_timer *t) {
    // Muda o estado do semáforo
    led_state = (led_state + 1) % 3;

    // Atualiza os LEDs de acordo com o estado atual do semáforo
    gpio_put(led_red_pin, led_state == 0);   // Vermelho
    gpio_put(led_yellow_pin, led_state == 1); // Amarelo
    gpio_put(led_green_pin, led_state == 2);  // Verde

    // Define a flag para imprimir a mensagem
    print_message = true;

    return true; // Mantém o temporizador ativo
}

int main() {
    // Inicializa o padrão de entrada/saída
    stdio_init_all();

    // Inicializa os pinos dos LEDs
    gpio_init(led_red_pin);
    gpio_set_dir(led_red_pin, GPIO_OUT);
    gpio_put(led_red_pin, 1); // Inicia com o LED vermelho aceso

    gpio_init(led_yellow_pin);
    gpio_set_dir(led_yellow_pin, GPIO_OUT);
    gpio_put(led_yellow_pin, 0);

    gpio_init(led_green_pin);
    gpio_set_dir(led_green_pin, GPIO_OUT);
    gpio_put(led_green_pin, 0);

    // Configura o temporizador para alterar o semáforo a cada 3 segundos
    struct repeating_timer timer;
    add_repeating_timer_ms(3000, repeating_timer_callback, NULL, &timer);

    // Loop principal
    while (1) {
        if (print_message) {
            printf("Mudança de estado do semáforo para %s\n", led_state == 0 ? "Vermelho" : led_state == 1 ? "Amarelo" : "Verde");
            print_message = false;
        }
        sleep_ms(1000); // Espera 1 segundo
    }

    return 0;
}
