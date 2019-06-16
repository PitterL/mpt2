
/*
 * interface.h
 *
 * Created: 6/13/2019 5:55:22 AM
 *  Author: A41450
 */ 

#ifndef _MPT_INTERFACE_H
#define _MPT_INTERFACE_H

/* Call init before Touch init */
void mptt_interface_init(void);

/* Call start after Touch inited */
void mptt_start(void);

/* Call process when sampling finished each time*/
void mptt_process(void);

#define OFFSET_CONFIG_IN_EEPROM 0

#endif