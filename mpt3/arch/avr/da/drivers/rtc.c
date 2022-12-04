/**
 * \file
 *
 * \brief RTC related functionality implementation.
 *
 (c) 2020 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms,you may use this software and
    any derivatives exclusively with Microchip products.It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/**
 * \defgroup doc_driver_rtc_init RTC Init Driver
 * \ingroup doc_driver_rtc
 *
 * \section doc_driver_rtc_rev Revision History
 * - v0.0.0.1 Initial Commit
 *
 *@{
 */
#include "rtc.h"

/**
 * \brief Initialize rtc interface
 * \return Initialization status.
 */
int8_t Timer_init()
{

	while (RTC.STATUS > 0) { /* Wait for all register to be synchronized */
	}

	// RTC.CMP = 0x0; /* Compare: 0x0 */

	// RTC.CNT = 0x0; /* Counter: 0x0 */

	RTC.CTRLA = RTC_PRESCALER_DIV1_gc   /* 1 */
	            | 1 << RTC_RTCEN_bp     /* Enable: enabled */
	            | 1 << RTC_RUNSTDBY_bp; /* Run In Standby: enabled */

	// RTC.PER = 0xffff; /* Period: 0xffff */

	RTC.CLKSEL = RTC_CLKSEL_OSC1K_gc; /* 32kHz divided by 32 */

	// RTC.DBGCTRL = 0 << RTC_DBGRUN_bp; /* Run in debug: disabled */

	RTC.INTCTRL = 0 << RTC_CMP_bp    /* Compare Match Interrupt enable: enabled */
	              | 0 << RTC_OVF_bp; /* Overflow Interrupt enable: disabled */

	// RTC.PITCTRLA = RTC_PERIOD_OFF_gc /* Off */
	//		 | 0 << RTC_PITEN_bp; /* Enable: disabled */

	// RTC.PITDBGCTRL = 0 << RTC_DBGRUN_bp; /* Run in debug: disabled */

	// RTC.PITINTCTRL = 0 << RTC_PI_bp; /* Periodic Interrupt: disabled */

	return 0;
}

/*============================================================================
void Timer_set_period(const uint8_t val, bool pit_en)
------------------------------------------------------------------------------
Purpose: This function sets the time interval on the RTC/Timer peripheral based
         on the user configuration, the RTC timer has been configured as 1 quarter ms unit
Input  : Time interval(ms), pit enable/disable
Output : none
Notes  :
============================================================================*/

/* USE_MPTT_WRAPPER, 
   We use Overflow interrupt instead of Compare interrupt */
void Timer_set_period(const uint16_t val, const bool pit)
{
	/* Disable Clock and PIT */

	while (RTC.STATUS & RTC_CTRLABUSY_bm)
		;                               /* wait for RTC synchronization */
	RTC.CTRLA &= ~(1u << RTC_RTCEN_bp); /* Clock disabled */

	if (!pit) {
		/* Disable PIT */
		while (RTC.PITSTATUS & RTC_CTRLBUSY_bm)
			;
		RTC.PITCTRLA &= ~(1u << RTC_PITEN_bp); /* PIT disabled */
	}
		
	if (val != 0) {
		while (RTC.STATUS & RTC_PERBUSY_bm)
			;          /* wait for RTC synchronization */
		if (val <= 0x3FFF)
			RTC.PER = (val & 0x3FFF) << 2; /* Set period register, RTC cycle use 1/quarter ms */
		else
			RTC.PER = 0xFFFF;
		RTC.INTCTRL |= (1 << RTC_OVF_bp);
	} else { /* if the value is zero, disable interrupt */
		RTC.INTCTRL &= ~(1 << RTC_OVF_bp);
	}

	while (RTC.STATUS & RTC_CNTBUSY_bm)
		;         /* wait for RTC synchronization */
	RTC.CNT = 0u; /* Clear count register */
	
	if (val && pit) {
		/* Enable Clock and PIT */
		while (RTC.PITSTATUS & RTC_CTRLBUSY_bm)
			;
		RTC.PITCTRLA |= 1 << RTC_PITEN_bp;	/* PIT enabled */
	}
	

	/* Enable Clock */
	while (RTC.STATUS & RTC_CTRLABUSY_bm)
		;                           /* wait for RTC synchronization */
	RTC.CTRLA |= 1 << RTC_RTCEN_bp; /* clock Enabled */
}
