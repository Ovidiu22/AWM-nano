/*******************************/
/*** Created by Ovidiu Sabau ***/
/******* AWM-nano Slave  *******/
/*******************************/

#include <avr/interrupt.h>
#include <avr/wdt.h>


#include "nRF24L01.h"
#include "Ultrasonic.h"
#include "Diagnostics.h"
#include "SOC.h"
#include "PowerManagement.h"

/*------------------ Global variables ---------------*/
volatile uint16_t pulse;
volatile uint8_t iIRC = 0;
volatile int f_wdt = 1;
static uint8_t pwrMngmntCycle = 100;	// higher than PWR_MNGMNT_CYCLE-1

#define PWR_MNGMNT_CYCLE 31		// x 8 sec/cycle = time of sleep mode
#define TX_MAX_ITER 10			// Burst sending iterations (currently total duration of one cycle = 3 sec)
#define TRANSMIT_MAX_ITER_TOT 3	// cycles of burst sending iterations

/*---------- Slave core specific functions ----------*/
void initSlave(void);
uint8_t *SlaveCoreFunctionality(void);

/*----------------- Main function -------------------*/
int main(void)
{
/*---------- Local variables ----------*/
	uint8_t tx_iter = 0;
	uint8_t tx_iter_max = 0;
/*---------- Initialization -----------*/
	initSlave();
	
/*-------------- Cycle loop -----------*/
	while(f_wdt == 1)
	{
		pwrMngmntCycle += 1;
		if (pwrMngmntCycle >= PWR_MNGMNT_CYCLE)
		{		
			pwrMngmntCycle = 0;
			/* Slave core functionality */
			uint8_t *slave_resp;
			slave_resp = SlaveCoreFunctionality();
			uint8_t w_buf[5] = { slave_resp[0], slave_resp[1], slave_resp[2], 0, 0 };

			/* Prepare to transmit to the master */
			init_nrf24l01_tx();
			do
			{
				tx_iter += 1;
				reset();
				send_data(w_buf);	_delay_ms(58);	// delay adjusted so that 150 ms/cycle through measurements
				if (tx_iter > TX_MAX_ITER)	// 1.5 sec pro sending cycle (10 sending attempts)
				{
					_delay_ms(1500); // 1 cycle = 1.5 sec sending + 1.5 sec waiting = 3 sec	
					tx_iter = 0;
					tx_iter_max += 1;
					if (tx_iter_max >= TRANSMIT_MAX_ITER_TOT) // 3 cycles = 9 sec in total sending
					{
						tx_iter_max = 0;	// No receiver, therefore exit the loop and go back to sleep mode
						break;
					}
				}
			} while ((GetReg(STATUS) & (1<<TX_DS)) <= 0);	// Transmission successful
			turnOff_nrf24l01(); 	// Turn off radio

		}
	/* Power management */
	PwrMngmnt_main();
	}

	return 0;

}

/* *****************************************************************
Name:		*SlaveCoreFunctionality()
Inputs:		none
Outputs:	Pointer to array evaluation received payload
Description:evaluates the received payload from master and calculates the slave diagnostics
******************************************************************** */
uint8_t *SlaveCoreFunctionality(void)
{
	/* Local variables */
	static uint8_t slave_resp[5];
	uint8_t RC_resp = 0x47; // Always positive response
	uint8_t WaterLevel = 0;
	uint8_t soc_bat = 0;

	/* Start measurements */
	WaterLevel = getDistance_main();
	
	/* Battery State of Charge */
	//uint8_t soc_bat = getBatterySOC();

	/* Assign the values to the output variable */
	slave_resp[0] = RC_resp;
	slave_resp[1] = WaterLevel;
	slave_resp[2] = soc_bat;

	return slave_resp;
}

/* *****************************************************************
Name:		Ultrasonic Interrupt
Inputs:		none
Outputs:	internal timer
Description:calculates elapsed time of a measurement
******************************************************************** */
ISR(INT0_vect)
{
	switch (iIRC)
	{
		case 0: //when logic changes from LOW to HIGH
		{
			iIRC = 1;
			TCCR1B |= (1<<CS11);
			break;
		}
		case 1:
		{
			/* reset iIRC */
			iIRC = 0;
			/* stop counter */
			TCCR1B &= ~(1<<CS11);
			/* assign counter value to pulse */
			pulse = TCNT1;
			/* reset counter */
			TCNT1=0;
			break;
		}
	}
}

/* *****************************************************************
Name:		Watchdog Interrupt
Inputs:		none
Outputs:	f_wdt
Description:wakes up processor after internal timer limit reached (8 sec)
******************************************************************** */
ISR(WDT_vect)
{
	/* set the flag. */
	if(f_wdt == 0)
	{
		f_wdt = 1;
	}
	//else there is an error -> flag was not cleared
}

void initSlave(void) {
	init_RGB(); // Initialize LED
	set_RGB(red);	_delay_ms(100);

	init_SPI(); //Initialize SPI
	init_nrf24l01(); //Initialize radio module
	init_ultrasonic(); //Initialize ultrasonic module

	set_RGB_low;
}



