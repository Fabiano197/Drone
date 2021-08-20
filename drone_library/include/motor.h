#ifndef MOTOR_H
#define MOTOR_H

#if HAL_USE_PWM == TRUE


/* Note: If preprogrammed timer pins do not suit,
 * it is possible to take any arbitrary pin by using a callback function) */
#define CLOCK_FREQUENCY 100000//Frequency of clock
#define PWM_FREQUENCY 1000//Frequency of PWM (Period = PWM_FREQUENCY/CLOCK_FREQUENCY s)
#define PWM_DRIVER PWMD3
#define PWM_PORT GPIOC
#define PWM_PIN_1 6
#define PWM_PIN_2 7
#define PWM_PIN_3 8
#define PWM_PIN_4 9
#define PWM_CHANNEL_1 0
#define PWM_CHANNEL_2 1
#define PWM_CHANNEL_3 2
#define PWM_CHANNEL_4 3
#define PWM_ALTERNATE_FUNCTION 2


void motorStart(void);

void motorStop(void);

/**
 * @brief   Sets motor speed
 *
 * @param[in] motor     Select Motor (between 1 and 4)
 * @param[in] speed     Select Speed (between 0 and 10'000)
 *
 */
void setMotorSpeed(uint8_t motor, uint16_t speed);

#endif

#endif
