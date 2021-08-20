#include "ch.h"
#include "hal.h"
#include "battery.h"

#define ADC_GRP_NUM_CHANNELS   1
#define ADC_GRP_BUF_DEPTH      10

static adcsample_t battery_samples[ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH];

static float adcAvg = 0;
static bool battery_configured = false;

static thread_t *batteryThd;

/*
 * ADC conversion group.
 * Mode:        Linear buffer, 2 samples of 1 channel, SW triggered.
 * Channels:    IN7.
 */
static const ADCConversionGroup batteryAdcCfg = {
  .circular     = false,
  .num_channels = ADC_GRP_NUM_CHANNELS,
  .end_cb       = NULL,
  .error_cb     = NULL,
  .cfgr         = ADC_CFGR_CONT,
  .tr1          = ADC_TR_DISABLED,
  .tr2          = ADC_TR_DISABLED,
  .tr3          = ADC_TR_DISABLED,
  .awd2cr       = 0U,
  .awd3cr       = 0U,
  .smpr         = {
    0,
    0
  },
  .sqr          = {
    ADC_SQR1_SQ1_N(BATTERY_ADC_CHANNEL),
    0,
    0,
    0
  }
};


static THD_WORKING_AREA(waADC, 128);
static THD_FUNCTION(threadADC, arg) {
  (void)arg;
  chRegSetThreadName("adc");
  while (chThdShouldTerminateX() == false) {
    chThdSleepMilliseconds(500);
    adcConvert(&BATTERY_ADC_DRIVER, &batteryAdcCfg, battery_samples, ADC_GRP_BUF_DEPTH);
    adcAvg = 0;
    for(uint8_t i = 0; i < ADC_GRP_BUF_DEPTH; i++){
      adcAvg+= battery_samples[i];
    }
    adcAvg/=ADC_GRP_BUF_DEPTH;
  }
}

void batteryStart(){
  if(battery_configured)return;
  battery_configured = true;
  chThdSleepMilliseconds(1000);
  palSetPadMode(BATTERY_PORT, BATTERY_PIN, PAL_MODE_INPUT_ANALOG);

  adcAvg = BATTERY_ADC_DRIVER.state;
  adcStart(&BATTERY_ADC_DRIVER, NULL);
  batteryThd = chThdCreateStatic(waADC, sizeof(waADC), NORMALPRIO, threadADC, NULL);
}

void batteryStop(){
  if(!battery_configured)return;
  battery_configured = false;
  chThdTerminate(batteryThd);
  adcStop(&BATTERY_ADC_DRIVER);
}

float getVoltage(){
  return adcAvg;
}
