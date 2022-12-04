
/*
 * delay.h
 *
 * Created: 8/29/2022 10:12:00 AM
 *  Author: A41450
 */ 

#ifndef _ARCH_DELAY_H
#define _ARCH_DELAY_H

#ifdef __OPTIMIZE__
#include <clock_config.h>
#include <util/delay.h>
#	define delay_us(_v)	_delay_us(_v)
#	define delay_ms(_v)	_delay_ms(_v)
#else
#	define delay_us(_v)
#	define delay_ms(_v)
#endif

#endif