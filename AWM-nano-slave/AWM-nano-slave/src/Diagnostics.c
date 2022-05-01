/*******************************/
/*** Created by Ovidiu Sabau ***/
/*******	AWM-nano	 *******/
/*******************************/ 

#ifndef F_CPU
#define F_CPU 16000000ul
#endif

#include <avr/io.h>
#include "Diagnostics.h"

/* *****************************************************************
Name:		init_RGB()
Inputs:		none
Outputs:	none
Description:initializes the RGB Led
******************************************************************** */
void init_RGB(void)
{
	DDRD |= (1<<RED_pin) | (1<<BLUE_pin) | (1<<GREEN_pin);
}

/* *****************************************************************
Name:		set_RGB()
Inputs:		desired color
Outputs:	none
Description:sets the desired color on the RGB Led
******************************************************************** */
void set_RGB(uint8_t color)
{
	set_RGB_low;
	switch (color)
	{
		set_RGB_low;
		case red:
			set_RED_high;
			break;
		
		case blue:
			set_BLUE_high;
			break;
		
		case green:
			set_GREEN_high;
			break;
	}
}
