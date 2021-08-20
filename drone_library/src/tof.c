#include "ch.h"
#include "hal.h"
#include "tof.h"
#include "i2c_bus.h"

static bool tof_configured = false;

static bool VL53L0X_configured = false;

static uint16_t dist_mm = 0;

static thread_t *tofThd;

static VL53L0X_Error status;

static THD_WORKING_AREA(waTOF, 2048);
static THD_FUNCTION(threadTOF, arg) {
  (void)arg;
  chRegSetThreadName("tof");
  status = VL53L0X_ERROR_NONE;

  static VL53L0X_Dev_t device;

  device.I2cDevAddr = VL53L0X_ADDR;
  device.comms_speed_khz = 100;

  status = tofInit(&device);

  if(status == VL53L0X_ERROR_NONE){
      VL53L0X_configAccuracy(&device, VL53L0X_LONG_RANGE);
  }
  if(status == VL53L0X_ERROR_NONE){
      VL53L0X_startMeasure(&device, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
  }
  if(status == VL53L0X_ERROR_NONE){
      VL53L0X_configured = true;
  }

  while (chThdShouldTerminateX() == false) {
    if(VL53L0X_configured){
        VL53L0X_getLastMeasure(&device);
        dist_mm = device.Data.LastRangeMeasure.RangeMilliMeter;
    }
    chThdSleepMilliseconds(100);
  }
}



VL53L0X_Error tofInit(VL53L0X_Dev_t* device){

    VL53L0X_Error status = VL53L0X_ERROR_NONE;

    uint8_t VhvSettings;
    uint8_t PhaseCal;
    uint32_t refSpadCount;
    uint8_t isApertureSpads;

//init
    if(status == VL53L0X_ERROR_NONE)
    {
        // Structure and device initialisation
        status = VL53L0X_DataInit(device);
    }

    if(status == VL53L0X_ERROR_NONE)
    {
        // Get device info
        //status = VL53L0X_GetDeviceInfo(device, &(device->DeviceInfo));
    }

    if(status == VL53L0X_ERROR_NONE)
    {
        // Device Initialization
        status = VL53L0X_StaticInit(device);
    }

//calibration
    if(status == VL53L0X_ERROR_NONE)
    {
        // SPAD calibration
        status = VL53L0X_PerformRefSpadManagement(device,
                &refSpadCount, &isApertureSpads);
    }

    if(status == VL53L0X_ERROR_NONE)
    {
        // Calibration
        status = VL53L0X_PerformRefCalibration(device,
                &VhvSettings, &PhaseCal);
    }

    return status;
}

VL53L0X_Error VL53L0X_configAccuracy(VL53L0X_Dev_t* device, VL53L0X_AccuracyMode accuracy){

    VL53L0X_Error status = VL53L0X_ERROR_NONE;

//Activation Limits
    if (status == VL53L0X_ERROR_NONE) {
        status = VL53L0X_SetLimitCheckEnable(device,
                VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
    }

    if (status == VL53L0X_ERROR_NONE) {
        status = VL53L0X_SetLimitCheckEnable(device,
                VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
    }

//specific accuracy config
//copied from ST example and API Guide
    switch(accuracy){

        case VL53L0X_DEFAULT_MODE:
            if (status == VL53L0X_ERROR_NONE) {
                status = VL53L0X_SetLimitCheckEnable(device,
                        VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD, 1);
            }

            if (status == VL53L0X_ERROR_NONE) {
                status = VL53L0X_SetLimitCheckValue(device,
                        VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD,
                        (FixPoint1616_t)(1.5*0.023*65536));
            }
            break;

        case VL53L0X_HIGH_ACCURACY:
            if (status == VL53L0X_ERROR_NONE) {
                status = VL53L0X_SetLimitCheckValue(device,
                        VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,
                        (FixPoint1616_t)(0.25*65536));
            }
            if (status == VL53L0X_ERROR_NONE) {
                status = VL53L0X_SetLimitCheckValue(device,
                        VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,
                        (FixPoint1616_t)(18*65536));
            }
            if (status == VL53L0X_ERROR_NONE) {
                status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(device,
                        200000);
            }
            break;

        case VL53L0X_LONG_RANGE:
            if (status == VL53L0X_ERROR_NONE) {
                status = VL53L0X_SetLimitCheckValue(device,
                        VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,
                        (FixPoint1616_t)(0.1*65536));
            }
            if (status == VL53L0X_ERROR_NONE) {
                status = VL53L0X_SetLimitCheckValue(device,
                        VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,
                        (FixPoint1616_t)(60*65536));
            }
            if (status == VL53L0X_ERROR_NONE) {
                status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(device,
                        33000);
            }
            if (status == VL53L0X_ERROR_NONE) {
                status = VL53L0X_SetVcselPulsePeriod(device,
                        VL53L0X_VCSEL_PERIOD_PRE_RANGE, 18);
            }
            if (status == VL53L0X_ERROR_NONE) {
                status = VL53L0X_SetVcselPulsePeriod(device,
                        VL53L0X_VCSEL_PERIOD_FINAL_RANGE, 14);
            }
            break;

        case VL53L0X_HIGH_SPEED:
            if (status == VL53L0X_ERROR_NONE) {
                status = VL53L0X_SetLimitCheckValue(device,
                        VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,
                        (FixPoint1616_t)(0.25*65536));
            }
            if (status == VL53L0X_ERROR_NONE) {
                status = VL53L0X_SetLimitCheckValue(device,
                        VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,
                        (FixPoint1616_t)(32*65536));
            }
            if (status == VL53L0X_ERROR_NONE) {
                status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(device,
                        30000);
            }
            break;
        default:
            status = VL53L0X_ERROR_INVALID_PARAMS;
            break;
    }

    return status;
}

VL53L0X_Error VL53L0X_startMeasure(VL53L0X_Dev_t* device, VL53L0X_DeviceModes mode){

    VL53L0X_Error status = VL53L0X_ERROR_NONE;

    status = VL53L0X_SetDeviceMode(device, mode);

    if (status == VL53L0X_ERROR_NONE) {
        status = VL53L0X_StartMeasurement(device);
    }

    return status;
}

VL53L0X_Error VL53L0X_getLastMeasure(VL53L0X_Dev_t* device){
    return VL53L0X_GetRangingMeasurementData(device,
            &(device->Data.LastRangeMeasure));
}

VL53L0X_Error VL53L0X_stopMeasure(VL53L0X_Dev_t* device){
    return VL53L0X_StopMeasurement(device);
}

void tofStart(){
  if(tof_configured)return;
  tof_configured = true;
  i2cBusStart();
  chThdSleepMilliseconds(50);
  tofThd = chThdCreateStatic(waTOF, sizeof(waTOF), NORMALPRIO, threadTOF, NULL);
  return;
}

void tofStop(){
  if(!tof_configured)return;
  tof_configured = false;
  chThdTerminate(tofThd);
}

uint16_t getDistance(){
  return dist_mm;
}
