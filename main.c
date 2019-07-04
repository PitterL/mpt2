#include <atmel_start.h>
#include "mpt2/interface.h"
extern volatile uint8_t measurement_done_touch;

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

#ifdef USE_MPTT_WRAPPER
	mptt_start();
#endif
	cpu_irq_enable();
	
	/* Replace with your application code */
	while (1) {
		touch_process();
		if (measurement_done_touch == 1) {
			measurement_done_touch = 0;

#ifdef USE_MPTT_WRAPPER
			mptt_process();
#endif

		}
	}
}
