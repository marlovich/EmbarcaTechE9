/*
 * Esse código foi editado para a atividade da Unidade 4 Capítulo 6

 1. Modificação da Biblioteca font.h
• Adicionar caracteres minúsculos à biblioteca font.h

 2. Entrada de caracteres via PC
• Utilize o Serial Monitor do VS Code para digitar os caracteres.
• Cada caractere digitado no Serial Monitor deve ser exibido no display SSD1306

 3. Interação com o Botão A
• Pressionar o botão A deve alternar o estado do LED RGB Verde (ligado/desligado).
• A operação deve ser registrada com uma mensagem informativa sobre o estado do LED
sendo exibida no display SSD1306

 4. Interação com o Botão B
• Pressionar o botão A deve alternar o estado do LED RGB Azul (ligado/desligado).
• A operação deve ser registrada com uma mensagem informativa sobre o estado do LED
sendo exibida no display SSD1306
 *
*/

#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#define I2C_PORT i2c1
#define PIN_RGB_R 11
#define PIN_RGB_G 12
#define PIN_RGB_B 13
#define I2C_SDA 14
#define I2C_SCL 15
#define PIN_BTN_A 5
#define PIN_BTN_B 6
#define endereco 0x3C

// Configurações do display SSD1306
ssd1306_t ssd1306;

int main()
{
  // Inicialização das bibliotecas
  stdio_init_all();
  gpio_init(PIN_RGB_R);
  gpio_set_dir(PIN_RGB_R, GPIO_OUT);
  gpio_init(PIN_RGB_G);
  gpio_set_dir(PIN_RGB_G, GPIO_OUT);
  gpio_init(PIN_RGB_B);
  gpio_set_dir(PIN_RGB_B, GPIO_OUT);
  gpio_init(PIN_BTN_A);
  gpio_set_dir(PIN_BTN_A, GPIO_IN);
  gpio_init(PIN_BTN_B);
  gpio_set_dir(PIN_BTN_B, GPIO_IN);
  gpio_put(PIN_RGB_R, 0);
  gpio_put(PIN_RGB_G, 0);
  gpio_put(PIN_RGB_B, 0);
  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA);                                        // Pull up the data line
  gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
  ssd1306_t ssd;                                                // Inicializa a estrutura do display
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd);                                         // Configura o display
  ssd1306_send_data(&ssd);                                      // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  bool cor = true;
  while (true)
  {
    cor = !cor;
    // Atualiza o conteúdo do display com animações
    ssd1306_fill(&ssd, !cor);                            // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);        // Desenha um retângulo
    ssd1306_draw_string(&ssd, "A B C D a b c d", 8, 10); // Desenha uma string

    ssd1306_send_data(&ssd); // Atualiza o display

    if (uart_is_readable(uart0))
    {
      char input = uart_getc(uart0);
      ssd1306_draw_char(&ssd1306, input, 0, 0);
      ssd1306_send_data(&ssd1306);
    }

    // Verifica estado do Botão A
    if (gpio_get(PIN_BTN_A))
    {
      static bool stateG = false;
      stateG = !stateG;
      gpio_put(PIN_RGB_G, stateG ? 1 : 0);
      ssd1306_draw_string(&ssd1306, stateG ? "LED Verde Ligado" : "LED Verde Desligado", 0, 0);
      ssd1306_send_data(&ssd1306);
    
      sleep_ms(200); // Debounce
    }

    // Verifica estado do Botão B
    if (gpio_get(PIN_BTN_B))
    {
      static bool stateB = false;
      stateB = !stateB;
      gpio_put(PIN_RGB_B, stateB ? 1 : 0);
      ssd1306_draw_string(&ssd1306, stateB ? "LED Azul Ligado" : "LED Azul Desligado", 0, 0);
      ssd1306_send_data(&ssd1306);

      sleep_ms(200); // Debounce
    }
  }
}
