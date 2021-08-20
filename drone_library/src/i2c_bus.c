#include "i2c_bus.h"
#include "ch.h"
#include "hal.h"
#if HAL_USE_I2C == TRUE

static bool i2c_configured = false;

static const I2CConfig i2ccfg = {
    STM32_TIMINGR_PRESC(15U) |
    STM32_TIMINGR_SCLDEL(4U) | STM32_TIMINGR_SDADEL(2U) |
    STM32_TIMINGR_SCLH(15U)  | STM32_TIMINGR_SCLL(21U),
    0,
    0
};
static i2cflags_t errors = 0;

void i2cBusStart(){
    if(i2c_configured)return;
    i2c_configured = true;

    palSetPadMode(I2C_PORT, I2C_CLOCK_PIN , PAL_MODE_OUTPUT_OPENDRAIN );
    //16 clock pulses
    for(uint8_t i = 0 ; i < 32 ; i++){
        palTogglePad(I2C_PORT, I2C_CLOCK_PIN);
        chThdSleepMilliseconds(1);
    }
    //make sure the output is high
    palSetPad(I2C_PORT, I2C_CLOCK_PIN);

    palSetPadMode(I2C_PORT, I2C_CLOCK_PIN, PAL_MODE_ALTERNATE(I2C_ALTERNATE_FUNCTION) | PAL_STM32_OTYPE_OPENDRAIN);
    palSetPadMode(I2C_PORT, I2C_DATA_PIN, PAL_MODE_ALTERNATE(I2C_ALTERNATE_FUNCTION) | PAL_STM32_OTYPE_OPENDRAIN);

    i2cStart(&I2C_DRIVER, &i2ccfg);
}

void i2cBusStop(){
    if(!i2c_configured)return;
    i2c_configured = false;
    i2cStop(&I2C_DRIVER);
}

int8_t i2cBusSend(uint8_t addr, uint8_t reg, uint8_t value) {

    uint8_t txbuf[2] = {reg, value};
    uint8_t rxbuf[1] = {0};
    msg_t status;

    i2cAcquireBus(&I2C_DRIVER);
    if(I2C_DRIVER.state == I2C_READY) {
        status = i2cMasterTransmitTimeout(&I2C_DRIVER, addr>>1, txbuf, 2, rxbuf, 0, TIME_MS2I(50));
        if (status != MSG_OK){
            errors = i2cGetErrors(&I2C_DRIVER);
            if(I2C_DRIVER.state == I2C_LOCKED){
                i2cBusStop();
                i2cBusStart();
            }
            i2cReleaseBus(&I2C_DRIVER);
            return errors;
        }
    }
    i2cReleaseBus(&I2C_DRIVER);

    return status;
}

int8_t i2cBusReceive(uint8_t addr, uint8_t reg, uint8_t *value) {

    uint8_t txbuf[1] = {reg};
    uint8_t rxbuf[1] = {0};

    i2cAcquireBus(&I2C_DRIVER);
    if(I2C_DRIVER.state == I2C_READY) {
        msg_t status = i2cMasterTransmitTimeout(&I2C_DRIVER, addr, txbuf, 1, rxbuf, 1, TIME_MS2I(50));
        if (status != MSG_OK){
            errors = i2cGetErrors(&I2C_DRIVER);
            if(I2C_DRIVER.state == I2C_LOCKED){
                i2cBusStop();
                i2cBusStart();
            }
            i2cReleaseBus(&I2C_DRIVER);
            return errors;
        }
    }
    i2cReleaseBus(&I2C_DRIVER);

    *value = rxbuf[0];

    return MSG_OK;
}

int8_t read_reg_multi(uint8_t addr, uint8_t reg, uint8_t *buf, int8_t len) {

    i2cAcquireBus(&I2C_DRIVER);
    if(I2C_DRIVER.state == I2C_READY) {
        msg_t status = i2cMasterTransmitTimeout(&I2C_DRIVER, addr, &reg, 1, buf, len,TIME_MS2I(50));
        if (status != MSG_OK){
            errors = i2cGetErrors(&I2C_DRIVER);
            if(I2C_DRIVER.state == I2C_LOCKED){
                i2cBusStop();
                i2cBusStart();
            }
            i2cReleaseBus(&I2C_DRIVER);
            return errors;
        }
    }
    i2cReleaseBus(&I2C_DRIVER);

    return MSG_OK;
}

#endif
