/**
 * \file
 *
 * \brief system control wrapper.
 *
 (c) 2018 Microchip Technology Inc. and its subsidiaries.

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

#include <xc.h>
#include "arch/sysctrl.h"
#include "drivers/wdt.h"

void sys_reset(void)
{
	PCON0bits.RI = 1;
}

void sys_clr_wdt(void)
{
	wdt_clr();
}

uint16_t sys_get_wdt_period(void)
{
   return wdt_get_period_value();
}

void sys_sleep(sleep_mode_t m)
{
   if (m == SLEEP_DOZE) {
      DOZEN = 1;
      
      NOP();
      
      DOZEN = 0;
   } else if (m == SLEEP_DEEP) {
      /* Enable MAIN Regulator in LP mode. */
      VREGCONbits.VREGPM = 1;

      SLEEP();

      /* Enable ULP Regulator. */
      VREGCONbits.VREGPM = 2;
   }
}