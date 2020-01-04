
/*
 * interface.h
 *
 * Created: 6/13/2019 5:55:22 AM
 *  Author: A41450
 */ 

#ifndef _MPT_INTERFACE_H
#define _MPT_INTERFACE_H

#include "types.h"

/* Call init before Touch init */
void mptt_interface_init(void);

/* Call start after Touch inited */
void mptt_start(void);

/* Call pre_process before each sampling*/
void mptt_pre_process(void);

/* Call mptt_process at each sampling*/
void mptt_process(uint8_t done);

/* Call process when sampling finished each time*/
void mptt_post_process(void);

enum {
	BUS_STOP = 0,	//Default is STOP status, this is not real STOP signal at BUS
	BUS_READ,
	BUS_WRITE,
	BUS_COLLISION,
	BUS_ERROR,
	NUM_BUS_STATES
};

/* query current bus state */
u8 mptt_get_bus_state(void);

#define OFFSET_CONFIG_IN_EEPROM 0

#endif