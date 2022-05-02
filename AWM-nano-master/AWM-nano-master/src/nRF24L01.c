/*******************************/
/*** Created by Ovidiu Sabau ***/
/*******	AWM-nano	 *******/
/*******************************/

#ifndef F_CPU
#define F_CPU 16000000ul
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "nRF24L01.h"

/* *****************************************************************
Name:		init_SPI()
Inputs:		none
Outputs:	none
Description:Initialization of SPI pins
******************************************************************** */
void init_SPI()
{
	DDRB |= (1<<CEPin) | (1<<SSPin) | (1<<SCKPin) | (1<<MOSIPin) ;  //set MOSI, clock and SlaveSelect as output
	DDRB &= ~(1<<MISOPin);											//Set MOSI[MISO of nRF] as Input

	SPCR |= (1 << SPE) | (1 << MSTR); // | (1 << SPR0);					// Enable SPI as master
	
	set_SS_high;							//SlaveSelect To HIGH- not sending any command
	set_CE_low;								//CE Pin Low
}

/* *****************************************************************
Name:		spi_tranceiver()
Inputs:		input for the SPI buffer
Outputs:	SPI buffer
Description:Reads the SPI transceiver buffer
******************************************************************** */
uint8_t spi_tranceiver (uint8_t data)
{
	/* Load data in the buffer */
	SPDR = data;
	/* Wait for the complete transmission */
	while(!(SPSR & (1<<SPIF)));
	/* Return received data */
	return(SPDR);
}


/* *****************************************************************
Name:		GetReg()
Inputs:		name of register to be read 
Outputs:	data of the register
Description:Reads the called register
******************************************************************** */
uint8_t GetReg(uint8_t reg)
{
	_delay_us(10);						//Delay for 10us
	set_SS_low;							//Set SS Low - nRf starts listening for commands 10us after SS Low
	_delay_us(12);						//Delay for 12us
	spi_tranceiver(R_REGISTER + reg);	//R_Register --> Set to Reading Mode, "reg" --> The registry which will be read
	_delay_us(12);						//Dealy 12us
	reg = spi_tranceiver(NOP);			//Send DUMMY BYTE[NOP] to receive first byte in 'reg' register
	_delay_us(12);						//Delay 12us
	set_SS_high;						//SS High
	return reg;							//Return the registry read
}

/* *****************************************************************
Name:		reset()
Inputs:		none
Outputs:	none
Description:resets the STATUS register of interrupt flags
******************************************************************** */
void reset(void)
{
	_delay_us(10);
	set_SS_low;
	_delay_us(10);
	spi_tranceiver(W_REGISTER+STATUS);
	_delay_us(10);
	spi_tranceiver(0x70);
	_delay_us(10);
	set_SS_high;
}

/* *****************************************************************
Name:		WriteToNrf()
Inputs:		Read/Write command, name of register to be read, payload to be transmitted and length of payload
Outputs:	pointer to RX buffer
Description:controls the transmission or reception of payload over radio
******************************************************************** */
uint8_t *WriteToNrf(uint8_t ReadWrite, uint8_t reg, uint8_t *val, uint8_t antVal)
{
	//ReadWrite --> "R" or "W", reg --> 'register', *val --> array with package, antVal --> number of int in array
	if(ReadWrite == W)//If it is in READMODE, then addr is already 0x00
	{
		reg = W_REGISTER + reg;
	}
	static uint8_t ret[32];	//Array to be returned in the end

	_delay_us(10);						//Delay for 10us
	set_SS_low;							//Set SlaveSelect Low - nRf starts listening for commands 10us after CSN Low
	_delay_us(12);						//Delay for 12us
	spi_tranceiver(reg);				//"reg" --> Set nRf to write or read mode
	_delay_us(10);
	
	for(int i = 0; i<antVal; i++)
	{
		if(ReadWrite == R && reg != W_TX_PAYLOAD)
		{
			//READ A REGISTRY
			ret[i] = spi_tranceiver(NOP);		//Send dummy Byte to read data
			_delay_us(10);
		}
		else
		{
			//Write to nRF
			spi_tranceiver(val[i]);			//Send command one at a time
			_delay_us(10);
		}
	}
	set_SS_high;				//nRf into IDLE with CSN HIGH
	return ret;					//Return the data read
}

/* *****************************************************************
Name:		init_nrf24l01()
Inputs:		none
Outputs:	none
Description:initializes the nrf module for general purpose (RX / TX)
******************************************************************** */
void init_nrf24l01(void)
{
	_delay_ms(10);
	uint8_t val[5];
	val[0] = 0x70;		WriteToNrf(W,CONFIG,val,1);		//Set PRIM_RX in CONFIG reg to LOW [DONT POWER UP THE RADIO i.e keep it in power down mode]
	val[0] = 0x01;		WriteToNrf(W,EN_AA,val,1);		//Enable Auto Ack on Data Pipe 0
	val[0] = 0x01;		WriteToNrf(W,EN_RXADDR,val,1);	//Enable Data Pipe 0	
	val[0] = 0x03;		WriteToNrf(W,SETUP_AW,val,1);	//Set Address Width as 5Bytes
	val[0] = 0x2F;		WriteToNrf(W,SETUP_RETR,val,1);	//Set up Retries
	val[0] = 0x7A;		WriteToNrf(W,RF_CH,val,1);		//Set the CH
	val[0] = 0x26;		WriteToNrf(W,RF_SETUP,val,1);	//Set transfer rate	
	for(int i = 0;i<5;i++) { val[i] = 0x12; } 
		WriteToNrf(W,RX_ADDR_P0,val,5);					//Set Data Pipe 1 Addr
		WriteToNrf(W,TX_ADDR,val,5);					//Clock the addr of the receiving node in TX_ADDR
	val[0] = 0x5;			WriteToNrf(W,RX_PW_P0,val,1);	//Set correct payload width[4 bytes]
}

/* *****************************************************************
Name:		init_nrf24l01_rx()
Inputs:		none
Outputs:	none
Description:initializes the nrf module as receiver
******************************************************************** */
void init_nrf24l01_rx(void)
{
	_delay_ms(200);
	uint8_t val[1];
	val[0] = 0x1F;		WriteToNrf(W,CONFIG,val,1);		//Power up Radio as receiver
	_delay_ms(200);
}

/* *****************************************************************
Name:		turnOff_nrf24l01()
Inputs:		none
Outputs:	none
Description:Turns off the Radio module
******************************************************************** */
void turnOff_nrf24l01(void)
{
	_delay_ms(10);
	uint8_t val[1];
	val[0] = 0x00;		WriteToNrf(W,CONFIG,val,1);		//Power down Radio
	_delay_ms(10);
}

/* *****************************************************************
Name:		receive_data()
Inputs:		none
Outputs:	none
Description:Scans the air channel for possible signals
******************************************************************** */
void receive_data(void)
{
	_delay_ms(10);
	set_CE_high;
	_delay_ms(2000);
	set_CE_low;
	_delay_ms(10);
}