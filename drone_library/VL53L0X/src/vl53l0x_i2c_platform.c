#include "vl53l0x_i2c_platform.h"
#include "vl53l0x_def.h"
#include "hal.h"
#include "ch.h"
#include "i2c_bus.h"
#include "tof.h"

//#define I2C_DEBUG

int32_t VL53L0X_write_multi(uint8_t address, uint8_t index, uint8_t  *pdata, int32_t count) {

  systime_t timeout = TIME_MS2I(50); // 50 ms
  msg_t rdymsg = MSG_OK;

  uint8_t txbuff[32];
  uint8_t rxbuff[0];
  uint8_t nbDatas = count+1;

  txbuff[0] = index;

  while(count > 0){
    txbuff[count] = pdata[count-1];
    count--;
  }
  i2cAcquireBus(&I2C_DRIVER);
  if(I2C_DRIVER.state == I2C_READY) {
      rdymsg = i2cMasterTransmitTimeout(&I2C_DRIVER, address>>1, txbuff, nbDatas, rxbuff, 0, timeout);
  }

  if (rdymsg != MSG_OK){
    if(I2C_DRIVER.state == I2C_LOCKED){
      i2cBusStop();
      i2cBusStart();
    }
    i2cReleaseBus(&I2C_DRIVER);
    return VL53L0X_ERROR_CONTROL_INTERFACE;
  }
  i2cReleaseBus(&I2C_DRIVER);
  return VL53L0X_ERROR_NONE;
}

int32_t VL53L0X_read_multi(uint8_t address,  uint8_t index, uint8_t  *pdata, int32_t count) {

  systime_t timeout = TIME_MS2I(50); // 50 ms
  msg_t rdymsg = MSG_OK;

  uint8_t txbuff[1] = {index};

  i2cAcquireBus(&I2C_DRIVER);
  rdymsg = i2cMasterTransmitTimeout(&I2C_DRIVER, address>>1, txbuff, 1, pdata, count, timeout);

  if (rdymsg != MSG_OK){
    if(I2C_DRIVER.state == I2C_LOCKED){
      i2cBusStop();
      i2cBusStart();
    }
    i2cReleaseBus(&I2C_DRIVER);
    return VL53L0X_ERROR_CONTROL_INTERFACE;
  }
  i2cReleaseBus(&I2C_DRIVER);
  return VL53L0X_ERROR_NONE;
}

int32_t VL53L0X_write_byte(uint8_t address, uint8_t index, uint8_t data) {
  return VL53L0X_write_multi(address, index, &data, 1);
}

int32_t VL53L0X_write_word(uint8_t address, uint8_t index, uint16_t data) {
  uint8_t buff[2];
  buff[1] = data & 0xFF;
  buff[0] = data >> 8;
  return VL53L0X_write_multi(address, index, buff, 2);
}

int32_t VL53L0X_write_dword(uint8_t address, uint8_t index, uint32_t data) {
  uint8_t buff[4];

  buff[3] = data & 0xFF;
  buff[2] = data >> 8;
  buff[1] = data >> 16;
  buff[0] = data >> 24;

  return VL53L0X_write_multi(address, index, buff, 4);
}

int32_t VL53L0X_read_byte(uint8_t address, uint8_t index, uint8_t *data) {
  return VL53L0X_read_multi(address, index, data, 1);
}

int32_t VL53L0X_read_word(uint8_t address, uint8_t index, uint16_t *data) {
  uint8_t buff[2];
  int r = VL53L0X_read_multi(address, index, buff, 2);

  uint16_t tmp;
  tmp = buff[0];
  tmp <<= 8;
  tmp |= buff[1];
  *data = tmp;

  return r;
}

int32_t VL53L0X_read_dword(uint8_t address, uint8_t index, uint32_t *data) {
  uint8_t buff[4];
  int r = VL53L0X_read_multi(address, index, buff, 4);

  uint32_t tmp;
  tmp = buff[0];
  tmp <<= 8;
  tmp |= buff[1];
  tmp <<= 8;
  tmp |= buff[2];
  tmp <<= 8;
  tmp |= buff[3];

  *data = tmp;

  return r;
}
