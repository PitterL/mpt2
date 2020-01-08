/**
 * \file
 *
 * \brief mptt interface wrapper.
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

#include "include/types.h"
#include "arch/pinconf.h"
#include "arch/bus.h"
#include "arch/sysctrl.h"
#include "arch/flash.h"
#include "arch/tslapi.h"
#include "interface.h"
#include "tsl.h"

const hal_interface_info_t interface_hal = {
	.assert_irq = bus_assert_irq,
	.reset = sys_reset,
#ifdef FLASH_SAVE_CONFIG
	.load_cfg = inf_load_cfg,
	.save_cfg = inf_save_cfg,
#endif
};

void mptt_interface_init(void)
{
	tsl_init(&interface_hal);
	bus_init();
}

void mptt_start(void)
{	
	tsl_start();
	bus_start();
}

void mptt_pre_process(void)
{
	tsl_pre_process();
}

void mptt_process(uint8_t done)
{
	tsl_process(done);	
}

void mptt_post_process(void)
{
	tsl_post_process();
}