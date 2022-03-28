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
The below definition is used for cupdi package tool.
	Firmware version is a 32bit hexi value, which will be packed into eeprom segment.
	Fuse content is a hexi byte array. THe information will be packed into fuse segment.
	Warning: Support '//' comment mark, but not support '/ * * /' comment mark inside the definition
*/
/* Project code*/
#define PROJECT_CODE 0x4950421D	/*IP42, v1.D*/

/* Fuse content */
// BOD level 2(2.6v Sampled 1Khz at Sleep, Enabled at Active), OSC 16Mhz, NVM protect after POR, EEPROM erased, WDT(2048ms)
#define FUSES_CONTENT {0x09, 0x46, 0x7D, 0xFF, 0x00, 0xF6, 0xFF, 0x00, 0x00, 0xFF, 0xC5 }	/* BYTE order */


#endif /* PACK_H_ */