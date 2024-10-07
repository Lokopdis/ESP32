#ifndef I2C_MANIPULATION_H

#define I2C_MANIPULATION_H
#include "Wire.h"

class I2CRegisterManip{
    public:
        I2CRegisterManip(uint8_t I2CAddress); // I2C Address Contructor

        // Bit/Byte Manipulation in specific I2C Adress
        uint8_t readBit(uint8_t reg, uint8_t bitPos);
        uint8_t readByte(uint8_t reg);
        void writeBit(uint8_t reg, uint8_t bitPos, bool value);
        void writeByte(uint8_t reg, uint8_t value);
        uint8_t checkAddress(uint8_t reg);

    private:
        uint8_t _address; // I2C Address 
};
#endif