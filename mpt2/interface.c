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

#if defined(__GNUC__)
#include <avr/sleep.h>
#elif defined(__ICCAVR__)
#include <intrinsics.h>
#define sleep_cpu() __sleep()
#endif

#include "include/types.h"
#include "arch/pinconf.h"
#include "arch/bus.h"
#include "arch/cpu.h"
#include "arch/sysctrl.h"
#include "arch/flash.h"
#include "arch/tslapi.h"
#include "interface.h"
#include "tsl.h"

const hal_interface_info_t interface_hal = {
	.assert_irq = bus_assert_irq,
	.reset = sys_reset,
#ifdef MPTT_SAVE_CONFIG
	.load_cfg = inf_load_cfg,
	.save_cfg = inf_save_cfg,
#endif
};

static ssint mptt_state = -1;

/**
 * \brief MPTT framework initialization, 
	include software layer and communication bus
 */
void mptt_interface_init(void)
{
	/* Initialize Touch Software Layer */
	tsl_init(&interface_hal);

	/* Initialize Data Bus for communication, like I2C/SPI.. */
	bus_init();
}

/**
 * \brief MPTT framework active, 
	include software layer and communication bus
 * @return: 0: if successful, other value mean something error(mostly like the pin fault detect failed)
 */
ssint mptt_start(void)
{	
	ssint result;

	result = tsl_start();
	if (result) {
		mptt_state = result;
		return result;
	}
	
	bus_start();

	mptt_state = 0;
	
	return 0;
}

/**
 * \brief MPTT framework pre-work before touch process in progress,
 */
void mptt_pre_process(void)
{
	tsl_pre_process();
}

/**
 * \brief MPTT framework work when each touch process,
 */
void mptt_process(uint8_t done)
{
	tsl_process(done);	
}

/**
 * \brief MPTT framework post-work when touch process measure done,
 */
void mptt_post_process(void)
{
	tsl_post_process();
}

extern volatile uint8_t measurement_done_touch;
void mptt_run(void)
{
	uint8_t busy = 0;
	mptt_pre_process();

	if (mptt_state == 0) {
		busy = touch_process();
	}
	
	mptt_process(measurement_done_touch);

	if (measurement_done_touch /*== 1*/) {	//0: not done, 1 : success, any other value : error
		measurement_done_touch = 0;

		mptt_post_process();
		
		if (!busy && mptt_get_bus_state() == BUS_STOP)
			sleep_cpu();
	}
}

