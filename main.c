#include <atmel_start.h>
#include "led_driver.h"
#include "mpt2/mptt.h"

extern volatile uint8_t measurement_done_touch;

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	cpu_irq_enable();

#if ENABLE_LED == 1u
	init_led_driver();
#endif

#if USE_MPTT_WRAPPER
	mpt_start();
#endif
	/* Replace with your application code */
	while (1) {
		touch_process();
		if (measurement_done_touch == 1) {
			measurement_done_touch = 0;

#if USE_MPTT_WRAPPER			
			mpt_process();
#endif

#if ENABLE_LED == 1u
			led_decode_position();
#endif
		}
	}
}
