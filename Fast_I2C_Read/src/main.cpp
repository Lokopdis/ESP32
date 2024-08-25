/*   ETAPAS DE PROGRAMAÇÃO
  --> Configurar o TIMER que sera utilizado (Feito)
  --> Configurar a comunicação I2C para modo modo Fast Plus
  --> Cofiguração geral do sensor que sera lido na I2C
  --> O programa atualmente esta configurado para leitura de 1 sensor
      ADXL345 via I2C com interrupção em 6ms para garantir máxima resolução
      do sensor.
*/

///////////////////////// BIBLIOTECAS /////////////////////////
#include <Arduino.h>
#include <Wire.h>

///////////////////////// DIRETRIZES //////////////////////////
// TIMER
#define TIMER true

// GERAIS
#define SENSOR true
#define VETOR true

// SENSORES
#define SENSOR_1 true

////////////////////////// VARIAVEIS //////////////////////////
#if TIMER == true
#define TAXA_AMOSTRAGEM 1 // 1us

hw_timer_t *timer_0 = NULL; // Define ponteiro para o timer

#endif

#if SENSOR == true // Configuração geral do sensor

#define POWER_CTL 0x2D // Power Control
#define MEASURE_MODE 0x08 // Modo de medição
#define DATAX0 0x32 // Registrador para dado X0
#define DATA_FORMAT 0x31 // Formato do dado
#define RANGE 0x02 // 2G -> 0x00 | 4G -> 0x01 | 8G -> 0x02 | 16G -> 0x03

volatile int SensorIndex = 0;
float sensitivy = 0;

#endif

#if SENSOR_1 == true // Configurações do sensor 1

#define SENSOR_ADDRESS_1 0x53 // PINO SDO NO GND
#define SDA_1 21
#define SCL_1 22

TwoWire I2C_1 = TwoWire(0);
#define TAMANHO_VETOR_1 50
volatile int count_1 = 0;
volatile bool Leitura_Sensor_1_Finalizada = false;

#endif

#if VETOR == true
volatile int16_t buffer_1[TAMANHO_VETOR_1][3];
#endif

//////////////////// DECLARAÇÃO DE FUNÇÕES ///////////////////
#if TIMER == true
void IRAM_ATTR Read_Sensor();
#endif

#if SENSOR == true
void InicializeADXL(TwoWire &wire, uint8_t address, uint8_t range);
void IRAM_ATTR ReadADXL(TwoWire &wire, uint8_t address, volatile int16_t *raw_data);
void ConvertAndPrintData(volatile int16_t buffer[50][3], int buffer_size, float sensitivity);
#endif

//////////////////////////// SETUP ///////////////////////////
void setup() {

#if TIMER == true
timer_0 = timerBegin(0,80,true);
timerAttachInterrupt(timer_0, &Read_Sensor, true);
timerAlarmWrite(timer_0,TAXA_AMOSTRAGEM,true);
timerAlarmEnable(timer_0);
#endif

#if SENSOR_1 == true & SENSOR == true // INICIALIZA SENSOR 1
I2C_1.begin(SDA_1, SCL_1);
I2C_1.setClock(1000000); // Configura pra modod Fast Plus (1MHz)
InicializeADXL(I2C_1, SENSOR_ADDRESS_1, RANGE);
#endif
}

/////////////////////////// LOOP ////////////////////////////
void loop() {
  if(Leitura_Sensor_1_Finalizada){
    Serial.println("Sensor 1: ");
    ConvertAndPrintData(buffer_1, TAMANHO_VETOR_1, sensitivy);
  }
}

///////////////////////// FUNÇÕES ///////////////////////////
#if TIMER == true
void IRAM_ATTR Read_Sensor(){
  // Cada ciclo de timer irá ler um sensor diferente e armazenar os dados brutos
  switch (SensorIndex) {
    case 0:
      if (!Leitura_Sensor_1_Finalizada) {
        ReadADXL(I2C_1, SENSOR_ADDRESS_1, buffer_1[count_1]);
        if(count_1<TAMANHO_VETOR_1-1){
          count_1++;
        }else{
          Leitura_Sensor_1_Finalizada = true;
        }
      }
      SensorIndex = 0; // Próximo sensor da leitura
      // Sempre antes do brak testar se todos os sensore finalizaram suas leituras
      if(Leitura_Sensor_1_Finalizada){
        timerAlarmDisable(timer_0);
      }
      break;
  }

}
#endif

#if SENSOR == true
void InicializeADXL(TwoWire &wire, uint8_t address, uint8_t range) {
  wire.beginTransmission(address);
  wire.write(DATA_FORMAT);
  wire.write(range);
  wire.endTransmission();

  wire.beginTransmission(address);
  wire.write(POWER_CTL);
  wire.write(MEASURE_MODE);
  wire.endTransmission();

  switch (range){ // DEFINE A SENSIBILIDADE COM BASE NO RANGE ESCOLHIDO
    case 0x00:
      sensitivy = 0.0039; //2G
      break;
      case 0x01:
      sensitivy = 0.0078; // 4G
      break;
      case 0x02:
      sensitivy = 0.0156; // 8G
      break;
      case 0x03:
      sensitivy = 0.0312; // 16G
      break;
    }
}

void IRAM_ATTR ReadADXL(TwoWire &wire, uint8_t address, volatile int16_t *raw_data) {
  wire.beginTransmission(address);
  wire.write(DATAX0);
  wire.endTransmission();
  wire.requestFrom(address, 6);

  if (wire.available() == 6) {
    raw_data[0] = (wire.read() | (wire.read() << 8)); // X axis
    raw_data[1] = (wire.read() | (wire.read() << 8)); // Y axis
    raw_data[2] = (wire.read() | (wire.read() << 8)); // Z axis
  }
}

void ConvertData(volatile int16_t *raw_data, volatile float &x_ms_2, volatile float &y_ms_2, volatile float &z_ms_2) {
  const float sensitivy = 0.0156; // Sensibilidade fixa para simplificação
  const float Gravity = 9.81; // Constante gravitacional

  float x_g = raw_data[0] * sensitivy;
  float y_g = raw_data[1] * sensitivy;
  float z_g = raw_data[2] * sensitivy;

  x_ms_2 = x_g * Gravity;
  y_ms_2 = y_g * Gravity;
  z_ms_2 = z_g * Gravity;
}

void ConvertAndPrintData(volatile int16_t buffer[50][3], int buffer_size, float sensitivity) {
  const float GRAVITY = 9.81; // Constante gravitacional em m/s²

  // Converte e imprime os dados para os 50 elementos no buffer
  for (int i = 0; i < buffer_size; i++) {
    // Extrai os valores brutos dos eixos X, Y e Z
    int16_t x_raw = buffer[i][0];
    int16_t y_raw = buffer[i][1];
    int16_t z_raw = buffer[i][2];

    // Converter os valores brutos em G-forces (gravidade)
    float x_g = x_raw * sensitivity;
    float y_g = y_raw * sensitivity;
    float z_g = z_raw * sensitivity;

    // Converter as G-forces em m/s²
    float x_ms2 = x_g * GRAVITY;
    float y_ms2 = y_g * GRAVITY;
    float z_ms2 = z_g * GRAVITY;

    // Printar os valores convertidos
    Serial.print(i);
    Serial.print(" | ");
    Serial.print(x_ms2);
    Serial.print(" | ");
    Serial.print(y_ms2);
    Serial.print(" | ");
    Serial.println(z_ms2);
  }
  
  Serial.println(); // Nova linha após todos os dados serem impressos
}

#endif