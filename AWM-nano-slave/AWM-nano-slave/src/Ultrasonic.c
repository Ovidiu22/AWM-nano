/*******************************/
/*** Created by Ovidiu Sabau ***/
/*******	AWM-nano	 *******/
/*******************************/

#include <stdlib.h>
#include <stdio.h>
#include "Ultrasonic.h"

#ifndef F_CPU
#define F_CPU 16000000ul
#endif

extern uint16_t pulse;

/* *****************************************************************
Name:		configUltrasonicPorts()
Inputs:		none
Outputs:	nonefor the
Description:configures the pins  ultrasonic sensor
******************************************************************** */
void configUltrasonicPorts()
{
	/* ECHO - input - port ECHO_pin */
	U_DDR_echo &= ~(1<<U_ECHO_pin);
	U_Port_echo &= ~(1<<U_ECHO_pin);
	
	/* TRIG - output - port TRIG_pin */
	U_DDR_trig |= (1<<U_TRIG_pin);
	U_Port_trig &= ~(1<<U_TRIG_pin);
	
	/* Power - output - port */
	U_DDR_pwr |= (1<<U_PWR_pin);
	U_Port_pwr &= ~(1<<U_PWR_pin);
}

/* *****************************************************************
Name:		enable_interrupts()
Inputs:		none
Outputs:	none
Description:configures the interrupts and enables the global interrupts
******************************************************************** */
 void enable_interrupts()
 {
	EIMSK |= (1<<INT0);
	EICRA |= (1<<ISC00);
	sei();
 }

/* *****************************************************************
Name:		init_ultrasonic()
Inputs:		none
Outputs:	none
Description:main function for the ultrasonic initialization
******************************************************************** */
void init_ultrasonic(void)
{
	configUltrasonicPorts();
	enable_interrupts();
}

/* *****************************************************************
Name:		getDistance_main()
Inputs:		pointer to diagnosis
Outputs:	distance measurements
Description:triggers measurements and plausibility checks
******************************************************************** */
 uint8_t getDistance_main()
 {
 	uint8_t distance = 0;
	float dist = 0;
	float distance_array[2];
	float sum = 0;
	
	/* Power on the sensor */
	ULTR_PWR_ON; _delay_ms(500);
					
	/* Execute two measurements and calculate mean value for redundancy */
	for (uint8_t j = 0; j < 2; j++)
	{
		triggerUltrasonic();
		distance_array[j] = getDistance();
		sum +=distance_array[j];
		_delay_ms(50);
	}
	/* Power off the sensor */
	ULTR_PWR_OFF;
	
	dist = sum/2;
	distance = dist;
	return distance;
 }

/* *****************************************************************
Name:		triggerUltrasonic()
Inputs:		none
Outputs:	none
Description:Triggers the TRIG pin of the ultrasonic sensor
******************************************************************** */
 void triggerUltrasonic()
 {
		/* trigger ultrasonic */
		U_Port_trig |= (1<<U_TRIG_pin);
		_delay_us(15);

		/* stop trigger ultrasonic */
		U_Port_trig &= ~(1<<U_TRIG_pin);
		_delay_ms(20);
 }

/* *****************************************************************
Name:		getDistance()
Inputs:		none
Outputs:	measured distance
Description:converts the pulse length to measurements in cm
******************************************************************** */
float getDistance()
{
	/* factor to be multiplied by pulse
	f = speedOfSound (343 m/s) * 100 * 8 (prescale factor) / 2 (way-back) / F_CPU;  ( ~0.008575 ); */
	/* distance = pulse * factor or
	distance = pulse / (1/factor) */
	return pulse / 116.618;
}
