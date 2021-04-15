#include <atmel_start.h>
#ifdef USE_MPTT_WRAPPER
#include "mpt2/interface.h"
#endif
extern volatile uint8_t measurement_done_touch;

int main(void)
{
	ssint state = 0;

	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

#ifdef USE_MPTT_WRAPPER
	state = mptt_start();
#endif
	cpu_irq_enable();
	
	/* Replace with your application code */
	while (1) {
#ifdef USE_MPTT_WRAPPER
		mptt_pre_process();
#endif
		if (state == 0)
			touch_process();

#ifdef USE_MPTT_WRAPPER
		mptt_process(measurement_done_touch);
#endif		
		if (measurement_done_touch /*== 1*/) {	//0: not done, 1 : success, any other value : error
			measurement_done_touch = 0;

#ifdef USE_MPTT_WRAPPER
			mptt_post_process();
#endif
		}

#ifdef USE_WDT
        CLR_WDT();
#endif
	}
}
