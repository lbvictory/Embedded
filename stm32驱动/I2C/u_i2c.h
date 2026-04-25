#ifndef __U_I2C
#define __U_I2C

#include "stm32f1xx.h"

typedef enum{
    I2C_1,
    I2C_2,
    I2C_COUNT,
} I2C_ID;

void I2C_Start(I2C_ID id);

void I2C_SendAddress(uint8_t addr, I2C_ID id);

void I2C_Stop(I2C_ID id);

void I2C_SendByte(uint32_t data, I2C_ID id);

#endif