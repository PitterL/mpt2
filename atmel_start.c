#include <atmel_start.h>
#include <mpt3/interface.h>

/**
 * Initializes MCU, drivers and middleware in the project
 **/
void atmel_start_init(void)
{
	system_init();

    /* Initialize MPTT interface */
    mptt_interface_init();
}
