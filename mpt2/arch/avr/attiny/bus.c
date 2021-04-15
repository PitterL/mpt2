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

#include <string.h>

#include "include/types.h"
#include "arch/pinconf.h"
#include "arch/bus.h"
#include "tsl.h"

#ifdef MPTT_BUS_MONITOR
#include "arch/sysctrl.h"
inline void bus_monitor_reset();
inline void bus_monitor_stop();
#endif

#define CHG_DUTY_CYCLES	4
#define CHG_SET_DUTY_ON_CYCLE 0

bus_monitor_t data_bus;

u8 current_tick()
{
	static u8 ticks = 0;
	
	ticks++;
	if (ticks >= CHG_DUTY_CYCLES)
		ticks = 0;
	
	return ticks;
}

void bus_state_change(u8 state)
{
    bus_monitor_t * const bus = &data_bus;

    switch(state) {
        case BUS_READ:
            bus->counter[BUS_READ] = 0;
            bus->state = state;
#ifdef MPTT_BUS_MONITOR		
			bus_monitor_reset();
#endif
            break;
        case BUS_WRITE:
            bus->counter[BUS_WRITE] = 0;
            bus->state = state;
#ifdef MPTT_BUS_MONITOR
			bus_monitor_reset();
#endif
            break;
        default:
            memset(bus, 0, sizeof(*bus));
#ifdef MPTT_BUS_MONITOR
			bus_monitor_stop();
#endif
    }
}

void bus_assert_irq(u8 assert, bool retrigger)
{
	//bus_monitor_t *bus = &data_bus;
	const u8 ticks = current_tick();
	
	/*
	if (!bus->state == BUS_STOP)	// THis stop is a state, not mean real STOP signal at bus
		return;
	*/
	
	if (!assert) {
		gpio_release_chg();
	} else {
		if (ticks == CHG_SET_DUTY_ON_CYCLE) {
			gpio_assert_chg();
		} else {
#ifdef OBJECT_T18
			if (retrigger) {
				if (ticks == CHG_SET_DUTY_ON_CYCLE + 1) {
					gpio_release_chg();
					} else if (ticks == CHG_SET_DUTY_ON_CYCLE + 2) {
					gpio_assert_chg();
				}
			}
#endif
		}
	}
}

ssint handle_bus_event(u8 state, u8 *val)
{
	bus_monitor_t *bus = &data_bus;
	u16 count;
	ssint result = 0;

	switch(state) {
		case BUS_WRITE:
			count = bus->counter[BUS_WRITE];
			if (count < sizeof(bus->regaddr)) {
				bus->regaddr.val[count] = *val;
                count++;
			} else {
				result = tsl_mem_write(bus->regaddr.value, count - sizeof(bus->regaddr), *val);
                if (result > 0) {
                    count += result;
                }
			}
            bus->counter[BUS_WRITE] = count;
		break;
		case BUS_READ:
			count = bus->counter[BUS_READ];
			result = tsl_mem_read(bus->regaddr.value, count, val);
            if (result > 0) {
                bus->counter[BUS_READ] = count + result;
            }
		break;
		case BUS_STOP:
			if (bus->counter[BUS_WRITE] > sizeof(bus->regaddr) || bus->counter[BUS_READ]) {
				memset(bus, 0, sizeof(*bus));
			}
            tsl_end();
		break;
		case BUS_COLLISION:
		case BUS_ERROR:
			; // Here we could report communication error
		default:
			memset(bus, 0, sizeof(*bus));
	}
	
	return result;
}

u8 mptt_get_bus_state(void) 
{
	return data_bus.state;
}


#ifdef MPTT_BUS_MONITOR

extern bool get_bus_line_level(void);

#define MONITOR_TICKS_STOP 0
#define MONITOR_TICKS_START 1
#define MONITOR_TICKS_RESET_TIME 200
/*============================================================================
void mptt_bus_monitor_ticks(u8 tick)
------------------------------------------------------------------------------
Purpose: update the bus monitor bit heart
Input  : tick, suggest 1 each time, or because of overflow
Output : none
Notes  :
============================================================================*/
void mptt_bus_monitor_ticks(u8 tick)
{	
	if (data_bus.current <= MONITOR_TICKS_STOP)	// the monitor is not running
		return;

	if (data_bus.current >= MONITOR_TICKS_RESET_TIME) {	/* 200 ms monitor */
		if (!get_bus_line_level()) {
			sys_reset();
		}

		bus_monitor_reset();
	} else {
		data_bus.current += tick;
	}
}

void bus_monitor_reset()
{
	data_bus.current = MONITOR_TICKS_START;
}

void bus_monitor_stop()
{
	data_bus.current = MONITOR_TICKS_STOP;
}
#endif