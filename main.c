#include <atmel_start.h>
#include "mpt2/mptt.h"
#include <i2c_slave_example.h>

extern volatile uint8_t measurement_done_touch;

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

#if USE_MPTT_WRAPPER
	mpt_start();
#endif
	cpu_irq_enable();
	
	/* Replace with your application code */
	while (1) {
		touch_process();
		if (measurement_done_touch == 1) {
			measurement_done_touch = 0;

#if USE_MPTT_WRAPPER			
			mpt_process();
#endif
		}
	}
}
