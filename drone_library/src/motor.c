#include "ch.h"
#include "hal.h"
#include "motor.h"

#if HAL_USE_PWM == TRUE

static bool motor_configured = false;

static PWMConfig pwmcfg = {
  CLOCK_FREQUENCY,                                    /* 10kHz PWM clock frequency.   */
  PWM_FREQUENCY,                                    /* Initial PWM period 0.1S.       */
  NULL,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  0,
  0,
  0
};


void motorStart(){
  if(motor_configured)return;
  motor_configured = true;
  pwmStart(&PWM_DRIVER, &pwmcfg);
  pwmEnableChannel(&PWM_DRIVER, PWM_CHANNEL_1, PWM_PERCENTAGE_TO_WIDTH(&PWM_DRIVER, 0));
  pwmEnableChannel(&PWM_DRIVER, PWM_CHANNEL_2, PWM_PERCENTAGE_TO_WIDTH(&PWM_DRIVER, 0));
  pwmEnableChannel(&PWM_DRIVER, PWM_CHANNEL_3, PWM_PERCENTAGE_TO_WIDTH(&PWM_DRIVER, 0));
  pwmEnableChannel(&PWM_DRIVER, PWM_CHANNEL_4, PWM_PERCENTAGE_TO_WIDTH(&PWM_DRIVER, 0));
  palSetPadMode(PWM_PORT, PWM_PIN_1, PAL_MODE_ALTERNATE(PWM_ALTERNATE_FUNCTION));
  palSetPadMode(PWM_PORT, PWM_PIN_2, PAL_MODE_ALTERNATE(PWM_ALTERNATE_FUNCTION));
  palSetPadMode(PWM_PORT, PWM_PIN_3, PAL_MODE_ALTERNATE(PWM_ALTERNATE_FUNCTION));
  palSetPadMode(PWM_PORT, PWM_PIN_4, PAL_MODE_ALTERNATE(PWM_ALTERNATE_FUNCTION));
}

void motorStop(){
  if(!motor_configured)return;
  pwmDisableChannel(&PWM_DRIVER, PWM_CHANNEL_1);
  pwmDisableChannel(&PWM_DRIVER, PWM_CHANNEL_2);
  pwmDisableChannel(&PWM_DRIVER, PWM_CHANNEL_2);
  pwmDisableChannel(&PWM_DRIVER, PWM_CHANNEL_2);
  pwmStop(&PWM_DRIVER);
  motor_configured = false;
}

void setMotorSpeed(uint8_t motor, uint16_t speed){
  pwmEnableChannel(&PWM_DRIVER, motor-1, PWM_PERCENTAGE_TO_WIDTH(&PWM_DRIVER, speed));
}

#endif
