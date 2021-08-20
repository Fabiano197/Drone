#ifndef I2C1_H
#define I2C1_H
#include "hal.h"
#if HAL_USE_I2C == TRUE

#define I2C_PORT GPIOB
#define I2C_CLOCK_PIN 6
#define I2C_DATA_PIN 7
#define I2C_ALTERNATE_FUNCTION 4
#define I2C_DRIVER I2CD1

void i2cBusStart(void);

void i2cBusStop(void);

int8_t i2cBusSend(uint8_t addr, uint8_t reg, uint8_t value);

#endif
#endif
