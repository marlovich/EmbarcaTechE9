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
  
  Esta variável é exibida no serial monitor para verificar o Bounce.

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
#define NUM_PIXELS 0
#define WS2812_PIN 7
#define tempo 1000

// Variáveis globais
static volatile uint a = 1;
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)

// Variável global para armazenar a cor (Entre 0 e 255 para intensidade)
uint8_t led_r = 0; // Intensidade do vermelho
uint8_t led_g = 0; // Intensidade do verde
uint8_t led_b = 200; // Intensidade do azul

// Buffer para armazenar quais LEDs estão ligados na matriz 5x5 formando os números de 0 a 9
bool led_buffer[NUM_PIXELS == 0] = {
    0, 0, 1, 0, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 0, 1, 0, 0
};
bool led_buffer[NUM_PIXELS == 1] = {
    0, 0, 1, 0, 0, 
    0, 1, 1, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0
};
bool led_buffer[NUM_PIXELS == 2] = {
    1, 1, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 0, 0, 
    1, 1, 0, 0, 0, 
    0, 1, 1, 1, 1
};
bool led_buffer[NUM_PIXELS == 3] = {
    1, 1, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    1, 1, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    1, 1, 1, 1, 0
};
bool led_buffer[NUM_PIXELS == 4] = {
    1, 0, 0, 1, 0, 
    1, 0, 0, 1, 0, 
    1, 1, 1, 1, 0, 
    0, 0, 1, 1, 0, 
    0, 0, 1, 1, 0
};
bool led_buffer[NUM_PIXELS == 5] = {
    0, 1, 1, 1, 1, 
    0, 1, 0, 0, 0, 
    0, 1, 1, 1, 1, 
    0, 0, 0, 0, 1, 
    0, 1, 1, 1, 1
};
bool led_buffer[NUM_PIXELS == 6] = {
    0, 1, 1, 1, 0, 
    0, 1, 0, 0, 0, 
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0
};
bool led_buffer[NUM_PIXELS == 7] = {
    1, 1, 1, 1, 0, 
    1, 0, 0, 1, 0, 
    0, 0, 1, 1, 1, 
    0, 0, 0, 1, 0, 
    0, 0, 0, 1, 0
};
bool led_buffer[NUM_PIXELS == 8] = {
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0
};
bool led_buffer[NUM_PIXELS == 9] = {
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 0, 0, 1, 0
};

static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void set_one_led(uint8_t r, uint8_t g, uint8_t b)
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    // Define todos os LEDs com a cor especificada
        if (led_buffer[NUM_PIXELS])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
}

// Prototipação da função de interrupção
static void gpio_irq_handler(uint gpio, uint32_t events);

int main()
{
    // Inicializações
    stdio_init_all();
    // Inicializa o LED
    gpio_init(ledA_pin);              // Inicializa o pino do LED
    gpio_set_dir(ledA_pin, GPIO_OUT); // Configura o pino como saída
 
    // Inicializa o botão A
    gpio_init(button_0);
    gpio_set_dir(button_0, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(button_0);          // Habilita o pull-up interno
    // Configuração da interrupção com callback
    gpio_set_irq_enabled_with_callback(button_0, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Inicializa o botão B
    gpio_init(button_1);
    gpio_set_dir(button_1, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(button_1);          // Habilita o pull-up interno
    // Configuração da interrupção com callback
    gpio_set_irq_enabled_with_callback(button_1, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

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
        NUM_PIXELS++;            // incrementa a variavel do numero de pixels
    }
}
