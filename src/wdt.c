/**
 * \file
 *
 * \brief WDT related functionality implementation.
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
 * \addtogroup doc_driver_wdt
 *
 * \section doc_driver_wdt_rev Revision History
 * - v0.0.0.1 Initial Commit
 *
 *@{
 */

#include <wdt.h>

/**
 * \brief Initialize Watchdog Timer
 *
 * \return 0
 */
int8_t WDT_0_init()
{

	// ccp_write_io((void*)&(WDT.CTRLA),WDT_PERIOD_OFF_gc /* Watch-Dog timer Off */
	//		 | WDT_WINDOW_OFF_gc /* Window mode off */);

	return 0;
}

/*
	USE_MPTT_WRAPPER
	 brief set Watchdog Timer value
	 val: the timeout value of watch dog value, watchdog timeout value should be equal or larger than this value or will be disabled
	 return: the period current used, less than zero mean failed
*/
int8_t WDT_0_set_period_value_ge(uint16_t val)
{
	uint8_t i, curr = 0;
	uint16_t period;
	
	if (WDT.STATUS & 0x80) // Locked
		return -1;

	for (i = 1; i <= 0xB; i++) {	// zero is disable, Min step is 1,  Max step is 0xB
		period = (4 << i);
		if (period >= val)
			break;
	}


	curr = WDT.CTRLA & 0xF;
	if (i > curr) {
		if (i > 0xB)
			i = 0;	// disabled watch dog
			
		ccp_write_io((void*)&(WDT.CTRLA), i /* Watch-Dog timer step */
				 | WDT_WINDOW_OFF_gc /* Window mode off */);
	}
	
	return curr;
}

/*
	USE_MPTT_WRAPPER
	 brief get Watchdog Timer value
	 return: the timeout value of watch dog value
*/
uint16_t WDT_0_get_period_value(void)
{
	uint8_t curr = WDT.CTRLA & 0xF;
	
	if (curr) {
		return (4 << (uint16_t)curr);
	} else {
		return 0;
	}
}
