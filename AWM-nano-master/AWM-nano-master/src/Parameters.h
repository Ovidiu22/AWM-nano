/*******************************/
/*** Created by Ovidiu Sabau ***/
/*******	AWM-nano	 *******/
/*******************************/

#ifndef PARAMETERS_H_
#define PARAMETERS_H_

	#define MEAS_ROBUST 1			// cm
	#define WATER_LEVEL_MIN 184		// cm - distance sensor -> lowest level
	#define WATER_LEVEL_MAX 23		// cm - distance sensor -> highest level
	#define WATER_LEVEL_DERIV 50	// cm/min	- max fill / empty speed
	#define FULL_TANK_THRESHOLD 145	// cm
	#define EMPTY_TANK_THRESHOLD 25	// cm
	#define PUMP_FULL_TANK		145	// cm - point at which pump stops


#endif /* PARAMETERS_H_ */