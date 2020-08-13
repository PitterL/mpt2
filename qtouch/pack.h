/*
 * pack.h
 *
 * Created: 8/12/2020 3:25:45 PM
 *  Author: A41450
 */ 


#ifndef PACK_H_
#define PACK_H_

/*----------------------------------------------------------------------------
 *     Package information
 *----------------------------------------------------------------------------*/
/*
The below definition is used for cupdi(v1.6) package tool.
	Firmware version is a 32bit hex value, which will be packed into eeprom segment.
	Fuse content is a hex byte array, NULL indicate ignored byte. THe information will be packed into fuse segment.
	Warning: Support '//' comment mark, but not support '/ * * /' comment mark inside the definition
*/
/* Firmware version*/
#define PROJECT_CODE 0x51543712	/*QT7, 1.2*/

/* Fuse content */
// BOD level 2(2.6v Sampled 1Khz at Sleep, Enabled at Active), OSC 16Mhz, NVM protect after POR, EEPROM erased
#define FUSES_CONTENT {0x00, 0x46, 0x7D, 0xFF, 0x00, 0xF6, 0xFF, 0x00, 0x00, 0xFF, NULL }	/*BYTE order, ignore set as NULL or the value*/

#endif /* PACK_H_ */