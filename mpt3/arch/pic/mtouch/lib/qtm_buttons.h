/*============================================================================
Filename : qtm_buttons.h
Project : QTouch Modular Library
Purpose : Touch post-proc
------------------------------------------------------------------------------
Version Number : 1.1
Last Updated   : 01/Feb/2017
Updated By     : Feargal Cleary
------------------------------------------------------------------------------
Copyright (c) 2017 Atmel. All rights reserved.
------------------------------------------------------------------------------
Revision 0.1 - Touch keys post-processing Alpha
Revision 0.2 - Bug fixes -> Beta
Revision 0.3 - API Updates
Revision 0.4 - Validation build with qtm_
Revision 1.0 - Release to Atmel Start
Revision 1.1 - Post-release validation fixes

============================================================================*/

#ifndef __PTC_CONFIG_BTNS_MODULE__
#define __PTC_CONFIG_BTNS_MODULE__

/*----------------------------------------------------------------------------
include files
----------------------------------------------------------------------------*/
#include "include/qtm_touch_key_0x0002_api.h"

/* Module #2 */
#define QTM_MODULE_ID_KEYS        0x0002u

/* Version 1.0: RC */ 
#define QTM_MODULE_VERSION          0x11u

/*----------------------------------------------------------------------------
manifest constants
----------------------------------------------------------------------------*/
#define CAL_SEQ_1_COUNT 8u
#define CAL_SEQ_2_COUNT (CAL_SEQ_1_COUNT >> 1u)

#define NULL_POINTER 0u

/* 
Node status byte: 
Bit 0 = Enabled
Bit 1 = CAL REQ
Bits 4:2 = Node state - Below
Bits 5 = Reserved
Bit 6 = TAU_CAL_DONE
Bits 7 = Reserved
*/

/*----------------------------------------------------------------------------
Prototypes
----------------------------------------------------------------------------*/


#endif
