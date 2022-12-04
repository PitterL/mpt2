/*============================================================================
Filename : 0x001d_adc_q10_acq_runtime.c
Project : QTouch Modular Library
Purpose : Acquisition module - Q10 and compatible
------------------------------------------------------------------------------
Version Number : 0.1
Last Updated : 4/10/2022
Updated By : Pitter Liao
------------------------------------------------------------------------------
Copyright (c) 2022 Microchip. All rights reserved.
------------------------------------------------------------------------------
Revision 0.1 - Initialize, from `ptc_driver_t1617_4p_runtime.c` v2.1
============================================================================*/

#ifndef __PTC_CONFIG_Q10_4P__
#define __PTC_CONFIG_Q10_4P__
/*----------------------------------------------------------------------------
  include files
----------------------------------------------------------------------------*/
#include "include/qtm_acq_q10_0x001b_api.h"

/* Module ID */
#define QTM_MODULE_ID		0x001D

/* Version: 0.1 Initial */
#define QTM_MODULE_VERSION		0x01u

/*----------------------------------------------------------------------------
  manifest constants
----------------------------------------------------------------------------*/
/* Calibration target +/- signal */
#define CC_CAL_PRECISION 2
#define CC_CAL_DIFF_PRECISION 2

/* Measurement configs */

/* Shield Options */

/* Proportional signal loss allowed when testing auto-tune */

/* Max limit on reasonable cal - 2 Tau at 31pF => ??? counts */

/* Analog gain settings for integration cap */

/* ADC Properties */
#define ADC_MAX_READ 1023u
#define ADC_MID_READ ((ADC_MAX_READ + 1) >> 1u)

#define MUTL_START_CCCAL      0x0234u
#define SELFCAP_START_CCCAL   0x02u

/* CCCAL Bit values in DELTA (@Cint max) */
#define CROUGH_MAX_MUTL         31u
#define CROUGH_BITVAL         16

/* RSEL | PRSC Byte */

/* XY to PORT-PIN */

#define QTM_ACQ_SEQUENTIAL 0u
#define QTM_ACQ_WINDOWCOMP 1u

#define NULL_POINTER 0u

#endif /*__PTC_CONFIG_T817SEQ__ */
