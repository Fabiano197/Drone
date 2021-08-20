#ifndef TOF_H
#define TOF_H

#include "hal.h"
#include "vl53l0x_api.h"

#define VL53L0X_ADDR 0x52
#define USE_I2C_2V8

typedef uint8_t VL53L0X_AccuracyMode;
#define VL53L0X_DEFAULT_MODE ( (VL53L0X_AccuracyMode) 0)
#define VL53L0X_HIGH_ACCURACY ( (VL53L0X_AccuracyMode) 1)
#define VL53L0X_LONG_RANGE ( (VL53L0X_AccuracyMode) 2)
#define VL53L0X_HIGH_SPEED ( (VL53L0X_AccuracyMode) 3)

void tofStart(void);

void tofStop(void);

VL53L0X_Error tofInit(VL53L0X_Dev_t* device);

uint16_t getDistance(void);

VL53L0X_Error VL53L0X_configAccuracy(VL53L0X_Dev_t* device, VL53L0X_AccuracyMode accuracy);

VL53L0X_Error VL53L0X_startMeasure(VL53L0X_Dev_t* device, VL53L0X_DeviceModes mode);

VL53L0X_Error VL53L0X_getLastMeasure(VL53L0X_Dev_t* device);

VL53L0X_Error VL53L0X_stopMeasure(VL53L0X_Dev_t* device);

#endif
