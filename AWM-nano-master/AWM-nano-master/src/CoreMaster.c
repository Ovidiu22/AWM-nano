/*******************************/
/*** Created by Ovidiu Sabau ***/
/*******	AWM-nano	 *******/
/*******************************/

#include <avr/interrupt.h>
#include <avr/io.h>
#include "CoreMaster.h"
#include "Parameters.h"
#include "LCD.h"

extern uint8_t pwrMngmntCycle;
extern uint8_t pmpSt;
extern uint8_t ledSt;

void init_WL_mgmt(void)
{
	PUMP_PORT |= (1<<PUMP_PIN);
	LED_PORT |= (1<<LED_PIN);
}

/* *****************************************************************
Name:		MasterCoreMain()
Inputs:		response from Slave: Routine Control response, payload and Slave diagnostics
Outputs:	none
Description:core functionality of the Master: Slave response evaluation
******************************************************************** */
void MasterCoreMain( uint8_t RC_resp, uint8_t distance, uint8_t bat_soc, uint8_t meas_cycles, 0 )
{
	/* Select identifier default diagnostic for fatal error */
	if (RC_resp != 0x47)	// Correct answer from Slave
	{
		clearScreen();
		displayLCD_main(1, TxtDescr[7].descr, NONE, "NONE");	// Something went wrong with Master-Slave communication
		displayLCD_main(2, TxtDescr[8].descr, NONE, "NONE");	// Something went wrong with Master-Slave communication
		pmpSt = 0;
	}
	else
	{
		displayLCD_main(2, TxtDescr[11].descr, bat_soc, "%"); // Eagle battery
		
		/* Get water level */
		uint8_t level_act = getActualLevel(distance);		
		displayMeasurements( level_act );	// Display level, time to fill, diagnostics		
		setPumpStatus( level_act );			// Control of water pump
	}
}

/* *****************************************************************
Name:		getActualLevel()
Inputs:		response from Slave: measured distance
Outputs:	water level
Description:calculates the actual water level based on the measured distance
			Information is tank specific (max. depth of the tank, ...)
******************************************************************** */
uint8_t getActualLevel(uint8_t distance) 
{
	distance = MIN(distance, WATER_LEVEL_MIN); // Limit lower level to the taps level; 
	uint8_t level_act = WATER_LEVEL_MIN - distance;
	
	return level_act;
}

/* *****************************************************************
Name:		displayMeasurements()
Inputs:		Slave diagnostics and pointer to measurement metrics (level, filling time and forecast id)
Outputs:	none
Description:displays water level, evolution forecast or slave diagnostics
******************************************************************** */
void displayMeasurements( uint8_t level_act )
{
	displayLCD_main(1, "CLEAR", NONE, "NONE");	// Clear row 1
	displayLCD_main(3, "CLEAR", NONE, "NONE");	// Clear row 2

	char full_level[] = "/160 cm";
	displayLCD_main(1, "Level: ", level_act, full_level);	// Level: 
}

/* *****************************************************************
Name:		setPumpStatus()
Inputs:		Slave diagnostics and pointer to measurement metrics (level, filling time and forecast id)
Outputs:	none
Description:Sets pump flag (switch on or off)
******************************************************************** */
void setPumpStatus( uint8_t level_act )
{
	// Pump control
	if (  level_act < EMPTY_TANK_THRESHOLD )
	{
		pmpSt = 1;	// Set status pump can be activated
	}
	else if ( level_act > PUMP_FULL_TANK )
	{
		pmpSt = 0;	// Set status pump has to be switched off
	}
}

/* *****************************************************************
Name:		setLevelLEDStatus()
Inputs:		Slave diagnostics and pointer to measurement metrics (level, filling time and forecast id)
Outputs:	none
Description:Sets LED flag (switch on or off)
******************************************************************** */
void setLevelLEDStatus( uint8_t level_act )
{
	// WL LED control
	if ( level_act < EMPTY_TANK_THRESHOLD )		// Water below above min. threshold
	{
		ledSt = 1;	// Turn on LED
	}
	else if ( level_act > EMPTY_TANK_THRESHOLD )	// Water level above min. threshold
		ledSt = 0;	// Turn off LED
	}
}

/* *****************************************************************
Name:		controlPump()
Inputs:		Pump status (global variable)
Outputs:	none
Description:Switches the pump on or off
******************************************************************** */
void controlPump(void)
{
	if ( pmpSt == 1 )
	{
		PUMP_TURN_ON;	// Turn on pump
	}
	else if ( pmpSt == 0 )
	{
		PUMP_TURN_OFF;	// Turn off pump
	}
}

/* *****************************************************************
Name:		controlPump()
Inputs:		LED status (global variable)
Outputs:	none
Description:Switches the LED on or off
******************************************************************** */
void controlLED(void)
{
	if ( ledSt == 1 )
	{
		LED_TURN_ON;	// Turn on pump
	}
	else if ( ledSt == 0 )
	{
		LED_TURN_OFF;	// Turn off pump
	}
}

/* *****************************************************************
Name:		UpdateMeasurementsTime()
Inputs:		Flag if new measurements available and number of cycles since last measurement
Outputs:	none
Description:Displays elapsed time since last measurements
******************************************************************** */
void UpdateMeasurementsTime( uint8_t newData, uint8_t cycles )
{
	if (newData == 1)	// Display elapsed time since last measurement, but not since last connection loss
	{
		uint8_t elapsedTime = cycles / 7; // min
		displayLCD_main(4, TxtDescr[9].descr, elapsedTime, TxtDescr[10].descr ); // Updated x min ago
	}
}

/* *****************************************************************
Name:		displayConnecting()
Inputs:		Flag if new measurements available
Outputs:	none
Description:Displays connecting attempt to Slave depending if first 
			or a subsequent measurement
******************************************************************** */
void displayConnecting( uint8_t f_meas_available )
{
	if (f_meas_available == 0)	// No previous measurements since last start
	{
		clearScreen();
		displayLCD_main(1, TxtDescr[3].descr, NONE, "NONE" );	// Listening to Eagle..
		displayLCD_main(2, TxtDescr[4].descr, NONE, "NONE" );	// This may take up to
		displayLCD_main(3, TxtDescr[5].descr, NONE, "NONE" );	// x minutes.
	}
	else	// Put the "Listening to Eagle" part on 4th row after previous measurements
	{
		displayLCD_main(4, TxtDescr[3].descr, NONE, "NONE" );	// Listening to Eagle..
	}
}

/* *****************************************************************
Name:		displayNoResp()
Inputs:		Flag if new measurements available
Outputs:	none
Description:Displays "no connection" to Slave depending if first or 
			a subsequent measurement
******************************************************************** */
void displayNoResp ( uint8_t f_meas_available )
{
	if (f_meas_available == 0) // If no previous measurements available
	{
		clearScreen();
		displayLCD_main(1, TxtDescr[6].descr, NONE, "NONE" ); // No resp. from Eagle. on line 1
	}
	else	// If previous measurements available
	{
		displayLCD_main(4, TxtDescr[6].descr, NONE, "NONE" ); // No resp. from Eagle. on line 4
	}
}