/*
 * Esse código foi para a atividade da Unidade 4 Capítulo 7

    1) Considerando a GPIO 22, defina a sua frequência de PWM para,
aproximadamente, 50Hz – período de 20ms.
    2) Defina o ciclo ativo do módulo PWM para 2.400µs
(microssegundos) – Ciclo de Trabalho (Duty Cycle) de 12%. isto
ajustará a flange (braço) do servomotor para a posição de, aproximadamente, 
180 graus. Aguarde 05 segundos nesta posição.
    3) Defina o ciclo ativo do módulo PWM para 1.470µs (microssegundos) 
    Ciclo de Trabalho (Duty Cycle) de 0,0735%. Isto ajustará a flange do servomotor para a posição de,
aproximadamente, 90 graus. Aguarde 05 segundos nesta posição.
    4) Defina o ciclo ativo do módulo PWM para 500µs (microssegundos)  
    Ciclo de Trabalho (Duty Cycle) de 0,025%. Isto ajustará a flange do servomotor para a posição de,
aproximadamente, 0 graus. Aguarde 05 segundos nesta posição.
    5) Após a realização das proposições anteriores, crie uma rotina
para movimentação periódica do braço do servomotor entre os ângulos de 0 e 180 graus. 
Obs.: a movimentação da flange deve ser suave, recomenda-se o incremento de ciclo ativo de ±5µs,
com um atraso de ajuste de 10ms.
    6) Com o emprego da Ferramenta Educacional BitDogLab, faça
um experimento com o código deste exercício utilizando o LED RGB – GPIO 12.
 *
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

// Definição dos pinos
#define GPIO_PWM 22
#define GPIO_LED_G 12

// Função para inicializar o PWM na GPIO especificada
void pwm_init(uint gpio, uint freq) {
    // Calcular a divisão do clock
    uint32_t clock_div = clock_get_hz(clk_sys) / (freq * 1000);
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&cfg, clock_div);
    pwm_config_set_wrap(&cfg, 20000); // Período de 20ms (50Hz)
    
    pwm_init(pwm_gpio_to_slice_num(gpio), &cfg, true);
    gpio_set_function(gpio, GPIO_FUNC_PWM);
}

void set_servo_angle(uint gpio, uint duty_us) {
    // Define o nível do PWM para o ciclo ativo especificado
    pwm_set_gpio_level(gpio, duty_us);
}

void pwm_test() {
    // Definir ciclo ativo para 2400µs (180 graus) e aguardar 5 segundos
    set_servo_angle(GPIO_PWM, 2400);
    sleep_ms(5000);

    // Definir ciclo ativo para 1470µs (90 graus) e aguardar 5 segundos
    set_servo_angle(GPIO_PWM, 1470);
    sleep_ms(5000);

    // Definir ciclo ativo para 500µs (0 graus) e aguardar 5 segundos
    set_servo_angle(GPIO_PWM, 500);
    sleep_ms(5000);

    // Movimentação periódica do braço do servomotor entre 0 e 180 graus
    for (int i = 500; i <= 2400; i += 5) {
        set_servo_angle(GPIO_PWM, i);
        sleep_ms(10);
    }

    for (int i = 2400; i >= 500; i -= 5) {
        set_servo_angle(GPIO_PWM, i);
        sleep_ms(10);
    }
}

int main() {
    stdio_init_all();

    // Inicializar PWM na GPIO 22 para aproximadamente 50Hz (período de 20ms)
    pwm_init(GPIO_PWM, 50);

    // Inicializar GPIO 12 para LED RGB
    gpio_init(GPIO_LED_G);
    gpio_set_dir(GPIO_LED_G, GPIO_OUT);

    while (true) {
        // Executar rotina de teste de PWM
        pwm_test();
    }

    return 0;
}