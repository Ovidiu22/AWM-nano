/*******************************/
/*** Created by Ovidiu Sabau ***/
/*******	AWM-nano	 *******/
/*******************************/

#ifndef COREMASTER_H_
#define COREMASTER_H_

#define PUMP_PORT DDRB
#define LED_PORT DDRD
#define PUMP_PIN PINB0
#define LED_PIN PIND2
#define PUMP_TURN_ON			PORTB |= (1<<PUMP_PIN);
#define PUMP_TURN_OFF			PORTB &= ~(1<<PUMP_PIN);
#define LED_TURN_ON				PORTD |= (1<<LED_PIN); 
#define LED_TURN_OFF			PORTD &= ~(1<<LED_PIN);

/* Master core specific functions */
void MasterCoreMain (uint8_t , uint8_t , uint8_t, uint8_t, uint8_t );
//void initButton (void);
uint8_t *getMeasurementsMetrics( uint8_t, uint8_t );
uint8_t getActualLevel( uint8_t );
uint8_t getFilEmptyTime( uint8_t );
uint8_t getForecastId(void);
void displayMeasurements( uint8_t, uint8_t * );
void setPumpStatus( uint8_t, uint8_t * );
void setLevelLEDStatus( uint8_t, uint8_t * );
void init_WL_mgmt(void);

void displayConnecting( uint8_t );
void displayNoResp ( uint8_t );
void controlPump(void);
void controlLED(void);
void UpdateMeasurementsTime( uint8_t, uint8_t );

/* Macros */
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#endif /* COREMASTER_H_ */