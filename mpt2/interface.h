
/*
 * interface.h
 *
 * Created: 6/13/2019 5:55:22 AM
 *  Author: A41450
 */ 

#ifndef _MPT_INTERFACE_H
#define _MPT_INTERFACE_H

#include "include/types.h"

/* Call init before Touch init */
void mptt_interface_init(void);

/* Call start after Touch inited */
ssint mptt_start(void);

/* Call run at main while loop */
void mptt_run(uint8_t done);

/* Call pre_process before each sampling*/
void mptt_pre_process(void);

/* Call mptt_process at each sampling*/
void mptt_process(void);

/* Call process when sampling finished each time*/
void mptt_post_process(uint8_t done);

#endif