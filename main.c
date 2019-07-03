#include <atmel_start.h>
#include "mpt2/interface.h"
#ifndef USE_MPTT_WRAPPER
#include <i2c_slave_example.h>
#endif
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
#else
			I2C_test_i2c_slave();	//For memory test purpose
#endif

		}
	}
}
