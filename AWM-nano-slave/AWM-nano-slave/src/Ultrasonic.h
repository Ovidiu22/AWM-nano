/*******************************/
/*** Created by Ovidiu Sabau ***/
/*******	AWM-nano	 *******/
/*******************************/


#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#ifndef F_CPU
#define F_CPU 16000000ul
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* Macros */
/* Trigger pin */
#define U_Port_trig		PORTD
#define U_DDR_trig		DDRD
#define U_TRIG_pin		PIND3

/* Echo pin */
#define U_Port_echo		PORTD
#define U_DDR_echo		DDRD
#define U_ECHO_pin		PIND2
/* Power pin */
#define U_Port_pwr		PORTD
#define U_DDR_pwr		DDRD
#define U_PWR_pin		PIND7

#define ULTR_PWR_ON		PORTD |= (1<<U_PWR_pin)
#define ULTR_PWR_OFF	PORTD &= ~(1<<U_PWR_pin)

/* Prototype functions */
void configUltrasonicPorts(void);
void enable_interrupts(void);
uint8_t getDistance_main(uint8_t *);
float getDistance(void);
void triggerUltrasonic(void);
void init_ultrasonic(void);


#endif /* ULTRASONIC_H_ */