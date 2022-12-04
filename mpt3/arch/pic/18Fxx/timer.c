/*+
 * timer.c
 *
 * Created: 12/11/2019 1:27:27 PM
 *  Author: A41450
 */ 

#include <common/include/types.h>
#include <tmr1.h>
#include <arch/cpu.h>

extern volatile uint16_t timer1ReloadVal;

/*============================================================================
void Timer_set_period(const uint8_t val, bool pit_en)
------------------------------------------------------------------------------
Purpose: This function sets the time interval on the RTC/Timer peripheral based
         on the user configuration, the RTC timer has been configured as 1 quarter ms unit
Input  : Time interval(ms), pit enable/disable
Output : none
Notes  :
============================================================================*/
void Timer_set_period(const uint16_t val, const bool unused)
{
	if (val) {
		timer1ReloadVal = (uint16_t)(0 - (val * LFINTOSC_FREQ_KHZ));

		TMR1_Reload();
	} else {
    TMR1_StopTimer();
	}
}

void Timer_set_callback(void (* handler)(void)) 
{
	TMR1_SetInterruptHandler(handler);
}
