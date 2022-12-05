/*
 * board.h
 *
 * Created: 7/12/2021 9:31:48 AM
 *  Author: A41450
 */ 


#ifndef BOARD_H_
#define BOARD_H_

/*----------------------------------------------------------------------------
 *     Package information
 *----------------------------------------------------------------------------*/
/*
The below definition is used for cupdi package tool.
	Firmware version is a 32bit hexi value, which will be packed into eeprom segment.
	Fuse content is a hexi byte array. THe information will be packed into fuse segment.
	Warning: Support '//' comment mark, but not support '/ * * /' comment mark inside the definition
*/
/* Project code*/
#define PROJECT_CODE 0x51313001	/*Q10, v0.1*/

/* Fuse content or configuration words*/
#define FUSES_CONTENT { 0x0 }	/* 6 Words */

/*
	TWI slave pin map
																		SCL		SDA
	TWI0		PORTMUX_TWI00_bm	GPIOA		3			2
					PORTMUX_TWI01_bm 	GPIOC		3			2

	TWI1		PORTMUX_TWI10_bm	GPIOF		3			2
					PORTMUX_TWI11_bm	GPIOB		3			2
*/
#define USE_TWI TWI0
#define TWI_PORT GPIOC
#define TWI_SCL_PIN	3
#define TWI_SDA_PIN 2
#define TWI_MUX PORTMUX_TWI01_bm

/*
	CHG pin map
*/
#define CHG_PORT GPIOB
#define CHG_PIN 2

#endif /* BOARD_H_ */