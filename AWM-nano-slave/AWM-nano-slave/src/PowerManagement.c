/*******************************/
/*** Created by Ovidiu Sabau ***/
/*******	AWM-nano	 *******/
/*******************************/

#include "PowerManagement.h"

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>




/* *****************************************************************
Name:		PwrMngmnt_main()
Inputs:		none
Outputs:	none
Description:main function of power management
******************************************************************** */
void PwrMngmnt_main(void)
{
	/* Initialize watchdog */
	PwrMngmnt_init();

	/* Re-enter sleep mode. */
	enterSleep();
}

/* *****************************************************************
Name:		PwrMngmnt_init()
Inputs:		none
Outputs:	none
Description:initialization of power management / watchdog
******************************************************************** */
void PwrMngmnt_init(void)
{
	/* Disable global interrupts */
	cli(); 
	
	/* Clear the reset flag. */
	MCUSR &= ~(1<<WDRF); // WDRF is set if a WDG system reset occurs
  
	/* In order to change WDE or the prescaler, we need to
	* set WDCE (This will allow updates for 4 clock cycles).
	*/
	WDTCSR |= (1<<WDCE) | (1<<WDE);

	/* set new watchdog timeout prescaler value */
	//WDTCSR = 1<<WDP3; /* 4.0 seconds */
	WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
	
	/* Enable global interrupts */
	sei();
	
	/* Enable watchdog interrupt */
	WDTCSR |= (1 << WDIE);
}

/* *****************************************************************
Name:		enterSleep()
Inputs:		none
Outputs:	none
Description:configures the sleep mode
******************************************************************** */
void enterSleep(void)
{
	/* Set sleep mode */
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	/* Enable sleep */
	sleep_enable();
  
	/* Now enter sleep mode. */
	sleep_mode();

	/* The program will continue from here after the WDT timeout*/
	sleep_disable(); /* First thing to do is disable sleep. */
  
	/* Re-enable the peripherals. */
	power_all_enable();
}