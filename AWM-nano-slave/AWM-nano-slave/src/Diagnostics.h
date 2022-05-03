/*******************************/
/*** Created by Ovidiu Sabau ***/
/*******	AWM-nano	 *******/
/*******************************/

#ifndef DIAGNOSTICS_H_
#define DIAGNOSTICS_H_

#define red		1
#define blue	2
#define green	3

#define RED_pin		PIND6
#define GREEN_pin	PIND5
#define BLUE_pin	PIND4

/* Macros */
#define set_RGB_low		PORTD &= ~((1<<RED_pin) | (1<<BLUE_pin) | (1<<GREEN_pin))
#define set_RED_high	PORTD |= (1<<RED_pin)
#define set_BLUE_high	PORTD |= (1<<BLUE_pin)
#define set_GREEN_high	PORTD |= (1<<GREEN_pin)

/* Prototype functions */
void init_RGB(void);
void set_RGB(uint8_t);
uint8_t getBatterySOC( void );

#endif /* DIAGNOSTICS_H_ */