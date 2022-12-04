/*============================================================================
Filename : freq_hop.h
Project  : Modular Library
Purpose  : Frequency hop operation on selected frequencies, Median filter
------------------------------------------------------------------------------
Version Number : 1.1
Last Updated   : 13/02/2019
Updated By     : Kamal
------------------------------------------------------------------------------
Copyright (c) 2019 Atmel. All rights reserved.
------------------------------------------------------------------------------
Revision 0.1 - Initial build
Revision 0.2 - API Updates
Revision 0.3 - Validation for null pointers
Revision 0.4 - Toolchain Update
Revision 1.0 - Release build
Revision 1.1 - Number of sensor field "num_sensors" is updated to uint16_t in
				"qtm_freq_hop_0x0006_api.h" to support 256 channels
===========================================================================*/

#ifndef __FREQ_HOP_MODULE__
#define __FREQ_HOP_MODULE__

/* Module #6 */
#define QTM_MODULE_ID_FREQ_HOP        0x0006u

/* Version 1.0: Beta (Major.Minor) */
#define QTM_MODULE_VERSION          0x11u


/*----------------------------------------------------------------------------
  include files
----------------------------------------------------------------------------*/
#include "include/qtm_freq_hop_0x0006_api.h"

/*----------------------------------------------------------------------------
  manifest constants
----------------------------------------------------------------------------*/
#define NULL_POINTER 0u

/*----------------------------------------------------------------------------
  Prototypes
----------------------------------------------------------------------------*/



#endif /* __FREQ_HOP_MODULE__ */
