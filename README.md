Na tarefa 2 do semáforo o que o código faz:

Inicializa os LEDs: Configura os pinos dos LEDs como saída e inicia com o LED vermelho aceso.

Callback do temporizador: A função repeating_timer_callback é chamada a cada 3 segundos. Nela, o estado do semáforo é alterado ciclicamente entre vermelho, amarelo e verde. Os LEDs são atualizados de acordo com o estado atual e uma flag é definida para imprimir uma mensagem.

Função principal: No loop principal, a cada segundo, uma mensagem informando a mudança de estado do semáforo é impressa na porta serial.

Na tarefa 2 One Shot o que o código faz:

Inicializa os LEDs e o botão: Configura os pinos dos LEDs como saída e o pino do botão como entrada com pull-up.

Callback do botão: A função button_callback é chamada quando o botão é pressionado. Ela verifica o debounce e, se o temporizador não estiver ativo, liga todos os LEDs e inicia o temporizador.

Callback do temporizador: A função turn_off_callback é chamada a cada 3 segundos pelo temporizador. Ela muda o estado dos LEDs, desligando-os progressivamente. Quando o último LED é desligado, o temporizador é desativado.

Função principal: No loop principal, uma mensagem é impressa a cada segundo informando o estado atual do temporizador e dos LEDs.
