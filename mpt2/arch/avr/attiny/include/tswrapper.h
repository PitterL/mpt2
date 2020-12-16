/*============================================================================
Filename : qtm_touch_key_api.h
Project : QTouch Modular Library
Purpose : Structs and definitions for use within modules
------------------------------------------------------------------------------
Copyright (c) 2019 Microchip. All rights reserved.
------------------------------------------------------------------------------
============================================================================*/

#ifndef TOUCH_API_TSWRAPPER_H
#define TOUCH_API_TSWRAPPER_H

#include <touch_api_ptc.h>

#ifdef OBJECT_T15
#ifdef TOUCH_API_KEYS_H
#define TOUCH_API_BUTTON
#endif
#endif

#ifdef OBJECT_T9
#ifdef TOUCH_API_SCROLLER_H
#define TOUCH_API_SCROLLER
#endif
#if (defined(TOUCH_API_SURFACE_CS2T_H) || defined(TOUCH_API_SURFACE_CS_H))
#define TOUCH_API_SURFACE
#endif
#endif

#ifndef POSITION_CHANGE
#ifdef SCROLLER_POSITION_CHANGE
#define POSITION_CHANGE SCROLLER_POSITION_CHANGE
#else
#define POSITION_CHANGE SURFACE_POSITION_CHANGE
#endif
#endif

#ifndef TOUCH_ACTIVE
#ifdef SCROLLER_POSITION_CHANGE
#define TOUCH_ACTIVE SCROLLER_POSITION_CHANGE
#else
#define TOUCH_ACTIVE SURFACE_POSITION_CHANGE
#endif
#endif

#ifndef SCR_RESOLUTION
#ifdef SCROLLER_RESOLUTION
#define SCR_RESOLUTION(m) SCROLLER_RESOLUTION(m)
#else
#define SCR_RESOLUTION(m) SURFACE_RESOLUTION(m)
#endif
#endif

#ifndef SCR_DEADBAND
#ifdef SCROLLER_DEADBAND
#define SCR_DEADBAND(m) SCROLLER_DEADBAND(m)
#else
#define SCR_DEADBAND(m) SURFACE_DEADBAND(m)
#endif
#endif

/* 
	cc value formula:
		(val & 0x0F)*0.00675 + ((val >> 4) & 0x0F)*0.0675 + ((val >> 8) & 0x0F)*0.675 + ((val >> 12) & 0x7) * 6.75
	Here, multiply 1000 for calculation:
*/

// 7 = 4 + 2 + 1 = 2^2 + 2^1 + 2^0
#define MUL_7(_v0) (((_v0) << 2) + ((_v0) << 1) + (_v0))

// 68 = 64 + 4 = 2^6 + 2^2
#define MUL_68(_v1) (((_v1) << 6) + ((_v1) << 2))

// 675 = 512 + 128 + 32 + 4 - 1 = 2^9 + 2^7 + 2^5 + 2^2 - 2^0
#define MUL_675(_v2) (((_v2) << 9) + ((_v2) << 7) + ((_v2) << 5) + ((_v2) << 2) - (_v2))

// 6750 = 4096 + 2048 + 512 + 64 + 32 - 2 = 2^12 + 2^11 + 2^9 + 2^6 + 2^5 - 2^1
#define MUL_6750(_v3) (((_v3) << 12) + ((_v3) << 11) + ((_v3) << 9) + ((_v3) << 6) + ((_v3) << 5) - ((_v3) << 2))

// Note: maxStudio use 16bit signed number
//#define CALCULATE_CAP(_v) (MUL_7((_v) & 0x0F) + MUL_68(((_v) >> 4) & 0x0F) + MUL_675(((_v) >> 8) & 0x0F) + MUL_6750(((_v) >> 12) & 0x03)) + MUL_6750(((_v) >> 14) & 0x03))

// Use 1/1000 pf as unit, the max value 675 * 8 = 54000, less than 16bit, but will show negative value in studio if more thant 32767
#define CALCULATE_CAP_DIV_1000(_v) (((_v) & 0x0F) * 7 + (((_v) >> 4) & 0x0F) * 68 + (((_v) >> 8) & 0x0F) *675 + ((((_v) >> 12) & 0x03) + (((_v) >> 14) & 0x03)) * 6750)

// Use 1/100 pf as unit
#define CALCULATE_CAP_DIV_100(_v) ((((_v) >> 2) & 0x02) * 3 + (((_v) >> 4) & 0x0F) * 7 + (((_v) >> 8) & 0x0F) *68 + ((((_v) >> 12) & 0x03) + (((_v) >> 14) & 0x03)) * 675)

#ifdef MPTT_CCAMP_DIV100
#	define CALCULATE_CAP CALCULATE_CAP_DIV_100
#else
#	define CALCULATE_CAP CALCULATE_CAP_DIV_1000
#endif

#define SENSOR_BASE_REF_VALUE 512u

#endif /* TOUCH_API_TSWRAPPER_H */