#include <Arduino.h>
#include "esp_timer.h"

// Variável de controle de estado
bool isRunning = false;

// Definição de variáveis para o timer
esp_timer_handle_t periodic_timer;

// Função que será chamada periodicamente
void IRAM_ATTR onTimer(void* arg) {
  if (isRunning) {
    Serial.println("Esta rodando!");
  }else{
    Serial.println("Esta parado!");
  }
}

// Função para alternar o estado do sistema
void toggleRunState() {
  isRunning = !isRunning;
  Serial.print("System is now ");
  Serial.println(isRunning ? "running" : "paused");
}

void setup() {
  Serial.begin(115200);

  // Configurações iniciais (pinos, interrupções, etc.)

  // Configuração do timer periódico
  const esp_timer_create_args_t timer_args = {
    .callback = &onTimer,
    .name = "periodic_timer"
  };

  esp_timer_create(&timer_args, &periodic_timer);
  esp_timer_start_periodic(periodic_timer, 1000000); // 1000000 us = 1 s
}

void loop() {
  // Aguarda entrada Serial
  if (Serial.available() > 0) {
    String input = Serial.readString(); // Lê a entrada como uma string
    Serial.println("Received: " + input); // Responde com o que foi recebido

    // Processa o comando recebido
    if (input.startsWith("TOGGLE")) {
      toggleRunState();
    }
    // Adicione mais comandos conforme necessário
  }
}
