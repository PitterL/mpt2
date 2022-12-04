#include <atmel_start.h>
#ifdef USE_MPTT_WRAPPER
#include "mpt3/interface.h"
extern volatile uint8_t measurement_done_touch;
#endif

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	/* Replace with your application code */
#ifdef USE_MPTT_WRAPPER
	mptt_start();

	cpu_irq_enable();

	/* Replace with your application code */
	while (1)
	{
		mptt_run(measurement_done_touch);
	}
#endif
}
