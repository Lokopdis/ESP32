#include "I2CRegisterManip.h"
#include <Arduino.h>

// Constructor
I2CRegisterManip::I2CRegisterManip(uint8_t I2CAddress){
    _address = I2CAddress; // I2C Device Address
    Wire.begin(); // Start I2C Comunication
}

// Bit Read
uint8_t I2CRegisterManip::readBit(uint8_t reg, uint8_t bitPos){
    uint8_t byteValue = readByte(reg); // Read Full Byte
    return (byteValue>>bitPos) & 0X01; // Select specified Bit
}

// Byte Read
uint8_t I2CRegisterManip::readByte(uint8_t reg){
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.endTransmission(false);

    Wire.requestFrom(_address, (uint8_t)1);
    if (Wire.available()){
        return Wire.read(); // Returno Data
    }
    return 0; // Returns 0 if there is no valid data
}


// Bit Write
void I2CRegisterManip::writeBit(uint8_t reg, uint8_t bitPos, bool value){
    uint8_t bytevalue = readByte(reg);
    if (value){
        bytevalue |= (1 << bitPos);
    } else{
        bytevalue &= ~(1 << bitPos);
    }
    writeByte(reg, bytevalue);
}

// Byte Write
void I2CRegisterManip::writeByte(uint8_t reg, uint8_t value){
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

// I2C Check
uint8_t I2CRegisterManip::checkAddress(uint8_t reg){
    Wire.beginTransmission(_address);
    Wire.write(reg);
    uint8_t error = Wire.endTransmission();
    if (Serial.available()){
        switch (error){
        case 0:
            Serial.println("Transmissão I2C bem-sucedida!");
            break;
        case 1:
            Serial.println("Erro: Limite do buffer excedido!");
            break;
        case 2:
            Serial.println("Erro: Dispositibo não respondeu!");
            break;
        case 3:
            Serial.println("Erro: Erros de transmissão!");
            break;
        case 4:
            Serial.println("Erro: Timeout!");
            break;
        default:
            Serial.println("Erro desconhecido!");
            break;
        }
    }

    return error;
}

