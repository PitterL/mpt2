#include <atmel_start.h>
#ifdef USE_MPTT_WRAPPER
#include "mpt2/interface.h"
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
#ifdef USE_MPTT_WRAPPER
		mptt_run(measurement_done_touch);
#endif

#ifdef USE_WDT
		CLR_WDT();
#endif
	}
}
