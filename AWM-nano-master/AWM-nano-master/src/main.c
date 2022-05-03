/*******************************/
/*** Created by Ovidiu Sabau ***/
/*******	AWM-nano	 *******/
/*******************************/

// Tank specific data is found in Parameters.h

#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "nRF24L01.h"
#include "PowerManagement.h"
#include "CoreMaster.h"
#include "LCD.h"

#define RX_MAX_ITER 100
#define PWR_MNGMNT_CYCLE 30


/*----------------- Global variables ---------------*/

volatile int f_wdt = 1;

uint8_t pmpSt = 0;	// Status pump
uint8_t ledSt = 0;	// Status LED water level 
uint8_t pwrMngmntCycle = 200;

/*----------------- Main function -------------------*/
int main(void)
{
/*---------- Local variables ----------*/
	uint8_t *val;
	uint8_t RX_iter;
	uint8_t newData = 1; 
	uint8_t display_cycles = 0;	// Used for time display
	uint8_t f_meas_available = 0;	// Flag for the presence of a previous measurement

/*---------- Initialization -----------*/
	initLCD();			displayLCD_main(1, "Initializing: ", 0, "%");	//Initializing...
						displayLCD_main(2, "--------------------", 0, "%");	_delay_ms(250);	//------
						displayLCD_main(3, "    Hello folks!", NONE, "NONE");	_delay_ms(250);	//personal message
	init_SPI(); 		displayLCD_main(1, "Initializing: ", 33, "%");	_delay_ms(250);
	init_nrf24l01();	displayLCD_main(1, "Initializing: ", 67, "%");	_delay_ms(250);
	init_WL_mgmt();		displayLCD_main(1, "Initializing: ", 100, "%");	_delay_ms(500);
	
/*-------------- Cycle loop -----------*/
	while(f_wdt == 1)
	{
		pwrMngmntCycle += 1;
		display_cycles += 1;	// Used for counting passed cycles since last display update (measurement or error)
		if (pwrMngmntCycle >= PWR_MNGMNT_CYCLE)
		{
			init_nrf24l01_rx();
			RX_iter = 0;	
			displayConnecting( f_meas_available );
			do
			{
				_delay_ms(1000); // 1 sec
				newData = 1;
				RX_iter += 1;
				reset();
				receive_data();	// 2 sec waiting -> 3 sec a complete waiting cycle
				if (RX_iter > RX_MAX_ITER)	// 
				{
					displayNoResp(f_meas_available);
					newData = 0;
					pmpSt = 0;	// No connection with slave -> turn off pump
					ledSt = 0;	// No connection with slave -> turn off LED
					display_cycles = 0;	// Reset display update after running a cycle
					break;
				}
			} while ((GetReg(STATUS) & (1<<RX_DR)) <= 0 );		// Bit 6 set on CONFIG Register -> RX successful
			turnOff_nrf24l01(); 	// TUrn off radio
			
			/* Set power management cycle for the case connection is lost -> waiting for less time than between measurements */
			pwrMngmntCycle = 24;	// Save 3.2 min waiting time
			
			/* New measurements are available */
			if ( newData == 1 )
			{
				/* Evaluate the response from Slave */
				f_meas_available = 1;	// At least one measurement available since last start
				val = WriteToNrf(R,R_RX_PAYLOAD,val,5);
				MasterCoreMain( val[0], val[1], val[2] ); // (Slave-respose, measurement value, battery SOC)
				/* Reset Power_management cycle number */
				pwrMngmntCycle = 0;
			}
			display_cycles = 0;	// Reset display update after running a cycle
			
			/* Pump and LED control */
			controlPump();			// Control of water pump
			controlLED();			// Control of LED
			
		}		// end if (pwrMngmntCycle >= PWR_MNGMNT_CYCLE)
		UpdateMeasurementsTime(newData, display_cycles);
		PwrMngmnt_main();	
	}		// end while(f_wdt == 1)
	return 0;
}


/* Watchdog interrupt */
ISR(WDT_vect)
{
	/* set the flag. */
	if(f_wdt == 0)
	{
		f_wdt = 1;
	}
	//else there is an error -> flag was not cleared
}
