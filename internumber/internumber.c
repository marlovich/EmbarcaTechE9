/*
  Por: Marlovich Vaz Dantas

Programa para atividade da unidade 4 de Sistemas Embarcados
                
                Interrupções e Debouncing.
  Uso de um botão e de um LED RGB da placa BitDogLab para
  demonstração do funcionamento das interrupções. Tentativa de correção 
  do Efeito Bouncing em botões.

  O código a seguir faz com que o Led vermelho pisca a cada 1000 ms.
  Uma variável de verificação é incrementada a cada interrupção ao pressionar 
  dos botões, que modifica a matriz de leds.

*/

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

// Configurações dos pinos
const uint ledA_pin = 13; // Red => GPIO13
const uint button_0 = 5; // Botão A = 5
const uint button_1 = 6; // Botão B = 6 

#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7
#define tempo 1000

// Variáveis globais
static volatile uint8_t current_number = 0;
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)

// Definição das cores para exibição
#define COLOR_OFF 0x000000
#define COLOR_ON  0x00FF00  // Verde

static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

uint32_t led_buffer[NUM_PIXELS] = {0}; // Buffer para armazenar as cores de todos os LEDs

// Função para desenhar números na matriz de LEDs
void draw_number(uint8_t number) {
    uint8_t number_patterns[10][5] = {
        {0x1F, 0x11, 0x11, 0x11, 0x1F}, // 0
        {0x00, 0x00, 0x1F, 0x00, 0x00}, // 1
        {0x1D, 0x15, 0x15, 0x15, 0x17}, // 2
        {0x11, 0x15, 0x15, 0x15, 0x1F}, // 3
        {0x07, 0x04, 0x04, 0x04, 0x1F}, // 4
        {0x17, 0x15, 0x15, 0x15, 0x1D}, // 5
        {0x1F, 0x15, 0x15, 0x15, 0x1D}, // 6
        {0x01, 0x01, 0x01, 0x01, 0x1F}, // 7
        {0x1F, 0x15, 0x15, 0x15, 0x1F}, // 8
        {0x17, 0x15, 0x15, 0x15, 0x1F}  // 9
    };

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (number_patterns[number][i] & (1 << j)) {
                led_buffer[i * 5 + j] = COLOR_ON;
            } else {
                led_buffer[i * 5 + j] = COLOR_OFF;
            }
        }
    }
}

void set_leds_from_buffer() {
    for (int i = 0; i < NUM_PIXELS; i++) {
        put_pixel(led_buffer[i]);
    }
}

// Prototipação da função de interrupção
static void gpio_irq_handler(uint gpio, uint32_t events);

int main()
{
    // Inicializações
    stdio_init_all();
    printf("WS2812 5x5 Matriz\n");

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    // Configuração dos pinos dos botões
    gpio_init(button_0);
    gpio_set_dir(button_0, GPIO_IN);
    gpio_pull_up(button_0);
    gpio_set_irq_enabled_with_callback(button_0, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    gpio_init(button_1);
    gpio_set_dir(button_1, GPIO_IN);
    gpio_pull_up(button_1);
    gpio_set_irq_enabled_with_callback(button_1, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    draw_number(current_number);
    set_leds_from_buffer();

    // Loop do Led vermelho piscando
    while (true)
    {
        gpio_put(ledA_pin, true);
        sleep_ms(tempo);
        gpio_put(ledA_pin, false);
        sleep_ms(tempo);
    }
}

// Função de interrupção com debouncing
void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time > 200000) // 200 ms de debouncing
    {
        last_time = current_time; // Atualiza o tempo do último evento

        if (gpio == button_0)
        {
            current_number = (current_number + 1) % 10; // Incrementa o número
        }
        else if (gpio == button_1)
        {
            current_number = (current_number == 0) ? 9 : current_number - 1; // Decrementa o número
        }
        draw_number(current_number);
        set_leds_from_buffer();
    }
}
