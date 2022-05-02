/*******************************/
/*** Created by Ovidiu Sabau ***/
/*******	AWM-nano	 *******/
/*******************************/

#ifndef POWERMANAGEMENT_H_
#define POWERMANAGEMENT_H_

#define WDG_TRIG	WDTCSR |= (1 << WDIE)

/* Function prototypes */
void PwrMngmnt_main(void);
void PwrMngmnt_init(void);
void enterSleep(void);

#endif /* POWERMANAGEMENT_H_ */