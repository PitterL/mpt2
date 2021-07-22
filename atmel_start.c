#include <atmel_start.h>

#if USE_MPTT_WRAPPER
#include "mpt2/interface.h"
#endif

#if defined(OBJECT_T15_USE_STATE_CB) || defined(OBJECT_T9_USE_STATE_CB)
#include "led.h"
#endif

/**
 * Initializes MCU, drivers and middleware in the project
 **/
void atmel_start_init(void)
{
	system_init();

#ifdef USE_MPTT_WRAPPER
	mptt_interface_init();
#endif

#if defined(OBJECT_T15_USE_STATE_CB) || defined(OBJECT_T9_USE_STATE_CB)
	leds_init();
#endif

	touch_init();
}
