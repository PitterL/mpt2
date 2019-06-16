#include <atmel_start.h>

#if USE_MPTT_WRAPPER
#include "mpt2/interface.h"
#endif

/**
 * Initializes MCU, drivers and middleware in the project
 **/
void atmel_start_init(void)
{
	system_init();

#if USE_MPTT_WRAPPER
	mptt_interface_init();
#endif

	touch_init();
}
