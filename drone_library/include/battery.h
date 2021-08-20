#ifndef BATTERY_H
#define BATTERY_H

#define BATTERY_ADC_DRIVER ADCD1
#define BATTERY_ADC_CHANNEL ADC_CHANNEL_IN7
#define BATTERY_PORT GPIOC
#define BATTERY_PIN 1

void batteryStart(void);

void batteryStop(void);

float getVoltage(void);

#endif
