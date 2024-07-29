#include <Arduino.h>
#include "esp_timer.h"

// Declarar o timer handle
esp_timer_handle_t my_timer;

// Função de callback que será chamada a cada 100 ms
void IRAM_ATTR onTimer(void* arg) {
    // Código a ser executado a cada 100 ms
    int64_t time_us = esp_timer_get_time(); // Tempo em microsegundos
    Serial.print("Timer disparado! Tempo decorrido: ");
    Serial.print(time_us / 1000); // Converte para milissegundos
    Serial.println(" ms");
}

void setup() {
  Serial.begin(115200);

  // Configurações do timer
  const esp_timer_create_args_t timer_args = {
    .callback = &onTimer,  // Função de callback
    .name = "MyTimer"      // Nome do timer (para fins de depuração)
  };

  // Criar o timer
  esp_timer_create(&timer_args, &my_timer);

  // Iniciar o timer com intervalo de 100 ms (100000 microssegundos)
  esp_timer_start_periodic(my_timer, 100000);
}

void loop() {
  // O loop pode ser usado para outras tarefas
  delay(1000);  // Apenas para simulação
}
