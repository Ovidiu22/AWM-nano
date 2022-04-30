/*******************************/
/*** Created by Ovidiu Sabau ***/
/*******	AWM-nano	 *******/
/*******************************/

#ifndef F_CPU
#define F_CPU 16000000ul
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "SOC.h"


/* *****************************************************************
Name:		getBatterySOC()
Inputs:		none
Outputs:	battery state of charge
Description:calculates battery state and reports a low level over diag
******************************************************************** */
uint8_t getBatterySOC( void )
{
	/* Function initialization */
	// AREF = AVcc
	ADMUX = (1<<REFS0);
	
		//ADMUX sets as well which port is used for measurements -> last three bits (MUX2, MUX1, MUX0)
		// [0 0 0] -> ADC0
		// [0 0 1] -> ADC1 ...
		
	// ADC Enable and prescaler of 128
	// 16000000/128 = 125000
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);

	/* Function body */
	// select the corresponding channel 0~7
	// ANDing with ’7? will always keep the value
	// of ‘ch’ between 0 and 7
	uint8_t ch = 0 & 0b0000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing
	
	// start single convertion
	// write ’1? to ADSC
	ADCSRA |= (1<<ADSC);
	
	// wait for conversion to complete
	// ADSC becomes ’0? again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	
	uint16_t ADCValue = ADC;
	float voltage = ADCValue / 2.03; // *( 5 (VReff) / 1023 (ADC denomination) * 100 (*) (relative SOC calculus, see below) ); 
	// factor was experimentally adapted in order to correspond to 100% at 4.15V
	
	/* Calculus of state of charge 
	soc = (meas_voltage - U_min) / (U_max - U_min) * 100
	U_max = 4.15V
	U_min = 3V
	=> soc = (meas_voltage(*) - U_min*100) / (U_max - U_min) = (meas_voltage(*) - 300) / 1.15
	*/
	float numer = (voltage - 300);
	if (numer < 0) {numer = 0;}
	float denum = 1.15;		
	float soc_bat = numer / denum;

	uint8_t soc_battery = soc_bat;
	if (soc_battery > 100) {soc_battery = 100;}
		
	return soc_battery;
}