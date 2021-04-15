/*+
 * timer.c
 *
 * Created: 12/11/2019 1:27:27 PM
 *  Author: A41450
 */ 

#include <tmr1.h>

#include "include/types.h"

extern bool mtouch_time_toScan;

uint8_t interrupt_cnt;
#ifdef OBJECT_T25
/* set to non zero suspend the sampling */
uint8_t qtlib_suspend = 0;
#endif

extern uint8_t adclib_time_elapsed_since_update;
extern void MTOUCH_ScanScheduler(void);
/*============================================================================
void touch_timer_handler(void)
------------------------------------------------------------------------------
Purpose: This function updates the time elapsed to the touch key module to
         synchronize the internal time counts used by the module.
Input  : none
Output : none
Notes  :
============================================================================*/
void touch_timer_handler(void)
{
#ifdef OBJECT_T25
	//suspend mode
    if (qtlib_suspend)
        return;
#endif

    //inactive mode
	if (!adclib_time_elapsed_since_update)
		return;

	interrupt_cnt++;

	if (interrupt_cnt >= adclib_time_elapsed_since_update) {
		interrupt_cnt = 0;
		/* Count complete - Measure touch sensors */
		MTOUCH_ScanScheduler();
	}
}

void sampling_timer_start(void)
{
    TMR1_SetInterruptHandler(touch_timer_handler);
    TMR1_StartTimer();
}

/*============================================================================
void touch_start(void)
------------------------------------------------------------------------------
Purpose: Start touch process. sampling timer are start in this function.
Input  : none
Output : none
Notes  :
============================================================================*/
void touch_start(void)
{
    sampling_timer_start();
}