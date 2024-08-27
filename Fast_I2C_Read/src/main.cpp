#include <Arduino.h>
#include <Wire.h>

#define SDA_1 21
#define SCL_1 22
#define MPU6050_ADDR 0x68
#define TAXA_AMOSTRAGEM 500 // 1000 microssegundos = 1ms

hw_timer_t *timer_0 = NULL;
volatile bool flagSensorReady = false;

TwoWire I2C_1 = TwoWire(0);

#define TAMANHO_VETOR_1 1000
volatile int count_1 = 0;
volatile bool Leitura_Sensor_1_Finalizada = false;
volatile int16_t buffer_1[TAMANHO_VETOR_1][3];
volatile unsigned long timestamp_1[TAMANHO_VETOR_1];

void IRAM_ATTR SetFlagSensorReady() {
  flagSensorReady = true;  // Apenas sinaliza a flag
}

void InicializeMPU(TwoWire &wire) {
  wire.beginTransmission(MPU6050_ADDR);
  wire.write(0x6B); // Registrador PWR_MGMT_1
  wire.write(0x00); // Desperta o MPU6050
  wire.endTransmission();

  wire.beginTransmission(MPU6050_ADDR);
  wire.write(0x1C); // Registrador ACCEL_CONFIG
  wire.write(0x00); // Configura para ±2g
  wire.endTransmission();
}

void ReadMPU(volatile int16_t *raw_data) {
  I2C_1.beginTransmission(MPU6050_ADDR);
  I2C_1.write(0x3B); // Registrador de início da aceleração
  I2C_1.endTransmission(false);

  I2C_1.requestFrom(MPU6050_ADDR, 6, true);

  raw_data[0] = (I2C_1.read() << 8) | I2C_1.read(); // X
  raw_data[1] = (I2C_1.read() << 8) | I2C_1.read(); // Y
  raw_data[2] = (I2C_1.read() << 8) | I2C_1.read(); // Z
}

void ConvertAndPrintData(volatile int16_t buffer[50][3], volatile unsigned long timestamps[50], int buffer_size) {
  const float GRAVITY = 9.81;
  const float SENSITIVITY = 16384.0;

  for (int i = 0; i < buffer_size; i++) {
    int16_t x_raw = buffer[i][0];
    int16_t y_raw = buffer[i][1];
    int16_t z_raw = buffer[i][2];
    unsigned long timestamp = timestamps[i];

    float x_g = (float)x_raw / SENSITIVITY;
    float y_g = (float)y_raw / SENSITIVITY;
    float z_g = (float)z_raw / SENSITIVITY;

    float x_ms2 = x_g * GRAVITY;
    float y_ms2 = y_g * GRAVITY;
    float z_ms2 = z_g * GRAVITY;

    Serial.print(i);
    Serial.print(" | ");
    Serial.print(timestamp);
    Serial.print(" | ");
    Serial.print(x_ms2);
    Serial.print(" | ");
    Serial.print(y_ms2);
    Serial.print(" | ");
    Serial.println(z_ms2);
  }
  
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  I2C_1.begin(SDA_1, SCL_1);
  I2C_1.setClock(1000000);
  InicializeMPU(I2C_1);

  timer_0 = timerBegin(0, 80, true);
  timerAttachInterrupt(timer_0, &SetFlagSensorReady, true);
  timerAlarmWrite(timer_0, TAXA_AMOSTRAGEM, true);
  timerAlarmEnable(timer_0);
}

void loop() {
  if (flagSensorReady) {
    flagSensorReady = false;

    // Desabilita o timer durante a leitura
    timerAlarmDisable(timer_0);

    if (!Leitura_Sensor_1_Finalizada) {
      ReadMPU(buffer_1[count_1]);
      timestamp_1[count_1] = micros(); // Armazena o timestamp atual em microssegundos
      if (count_1 <= TAMANHO_VETOR_1) {
        count_1++;
      } else {
        Leitura_Sensor_1_Finalizada = true;
      }
    }

    // Reabilita o timer se necessário
    if (!Leitura_Sensor_1_Finalizada) {
      timerAlarmEnable(timer_0);
    } else {
      // Se a leitura estiver finalizada, desabilita o timer e encerra o código
      timerAlarmDisable(timer_0);
      ConvertAndPrintData(buffer_1, timestamp_1, TAMANHO_VETOR_1);
      while (true); // Encerra o código após a leitura finalizada
    }
  }
}
