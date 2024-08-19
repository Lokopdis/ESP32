#include <BluetoothSerial.h>

// Cria uma instância do BluetoothSerial
BluetoothSerial SerialBT;

void setup() {
  // Inicializa a comunicação serial para monitoramento via USB
  Serial.begin(115200);

  // Inicializa a comunicação Bluetooth
  SerialBT.begin("ESP32_Bluetooth"); // Nome do dispositivo Bluetooth

  // Imprime uma mensagem indicando que o Bluetooth foi iniciado
  Serial.println("Bluetooth iniciado. Dispositivo pronto para parear.");
}

void loop() {
  // Gera um número aleatório entre 0 e 100
  int numeroAleatorio = random(0, 101);

  // Imprime o número no monitor serial (USB)
  Serial.print("Número Aleatório: ");
  Serial.println(numeroAleatorio);

  // Envia o número via Bluetooth para o celular
  SerialBT.print("Número Aleatório: ");
  SerialBT.println(numeroAleatorio);

  // Aguarda por 1 segundo antes de gerar outro número

}
