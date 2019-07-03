/**
 * \file
 *
 * \brief I2C Slave driver example.
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

#include <include/atmel_start_pins.h>
#include <include/i2c_slave.h>
#include <nvmctrl_basic.h>
#include "types.h"
#include "tsl.h"
#include "interface.h"

enum {
	BUS_STOP = 0,	//Default is STOP status, this is not real STOP signal at BUS
	BUS_READ,
	BUS_WRITE,
	BUS_COLLISION,
	BUS_ERROR,
	NUM_BUS_STATES
};

typedef struct bus_monitor {
	size_t counter[NUM_BUS_STATES];
	union {
		struct {
			u8 low;
			u8 high;
		} bytes;
		u8 val[2];
		u16 value;
	} regaddr;

	u8 state;
} bus_monitor_t;

bus_monitor_t data_bus;

#define CHG_DUTY_CYCLES	4
#define CHG_SET_DUTY_ON_CYCLE 0

u8 current_tick()
{
	static u8 ticks = 0;
	
	ticks++;
	if (ticks >= CHG_DUTY_CYCLES)
		ticks = 0;
	
	return ticks;
}

void bus_assert_chg(void)
{
	CHG_set_level(0);
	CHG_set_dir(PORT_DIR_OUT);
}

void bus_release_chg(void)
{
	CHG_set_pull_mode(PORT_PULL_UP);
	CHG_set_dir(PORT_DIR_IN);
}

void bus_set_chg(u8 assert, bool retrigger)
{
	bus_monitor_t *bus = &data_bus;
	const u8 ticks = current_tick();
	
	if (!assert)
		return;
	
	if (!bus->state == BUS_STOP)	// THis stop is a state, not mean real STOP signal at bus
		return;
	
	if (ticks == CHG_SET_DUTY_ON_CYCLE) {
		bus_assert_chg();
	}else {
#ifdef OBJECT_T18
		if (retrigger) {
			if (ticks == CHG_SET_DUTY_ON_CYCLE + 1) {
				bus_release_chg();
				}else if (ticks == CHG_SET_DUTY_ON_CYCLE + 2) {
				bus_assert_chg();
			}
		}
#endif
	}
}

ssint handle_bus_event(u8 state, u8 *val)
{
	bus_monitor_t *bus = &data_bus;
	u16 count;
	ssint result = 0;
	
	switch(state) {
		case BUS_WRITE:
			count = bus->counter[BUS_WRITE]++;
			if (count < sizeof(bus->regaddr)) {
				bus->regaddr.val[count] = *val;
			}else {
				result = tsl_mem_write(bus->regaddr.value, count - sizeof(bus->regaddr), *val);
			}
		break;
		case BUS_READ:
			count = bus->counter[BUS_READ]++;
			result = tsl_mem_read(bus->regaddr.value, count, val);
		break;
		case BUS_STOP:
			if (bus->counter[BUS_WRITE] > sizeof(bus->regaddr) || bus->counter[BUS_READ]) {
				memset(bus, 0, sizeof(*bus));
			}
		break;
		case BUS_COLLISION:
		case BUS_ERROR:
			; // Here we could report communication error
		default:
			memset(bus, 0, sizeof(*bus));
	}
	
	return result;
}

void bus_address_handler(void)
{
	bus_monitor_t *bus = &data_bus;
	u8 addr8;
	
	addr8 = I2C_read();
	if ((addr8 >> 1) == I2C_SLAVE_ADDRESS) {
		if (addr8 & 0x1) {
			bus->state = BUS_READ;
		}else {
			bus->state = BUS_WRITE;
		}
		I2C_send_ack(); // or send_nack() if we don't want to ack the address
		
		bus_release_chg();
	} else {
		I2C_send_nack();
	}
}

void bus_read_handler(void)
{ 
	u8 val = 0xA5;	//Initial value for debug only
	ssint result;

	result = handle_bus_event(BUS_READ, &val);
	if (!result)
		I2C_write(val);
}

void bus_write_handler(void)
{
	u8 val;
	ssint result;
	
	val = I2C_read();
	result = handle_bus_event(BUS_WRITE, &val);
	if (result)
		I2C_send_nack();
	else
		I2C_send_ack();
}

void bus_stop_handler(void)
{
	handle_bus_event(BUS_STOP, NULL);
	tsl_end();
}

void bus_error_handler(void)
{
	handle_bus_event(BUS_ERROR, NULL);
}

void bus_init(void)
{
	bus_monitor_t *bus = &data_bus;
	
	bus->state = BUS_STOP;
	
	I2C_set_read_callback(bus_read_handler);
	I2C_set_write_callback(bus_write_handler);
	I2C_set_address_callback(bus_address_handler);
	I2C_set_stop_callback(bus_stop_handler);
	I2C_set_collision_callback(bus_error_handler);
	I2C_set_bus_error_callback(bus_error_handler);
}

void bus_start(void)
{
	I2C_open();
}

void sys_reset(void)
{
	//FIXME: this is working
	RSTCTRL.SWRR = 0x1;
}

#ifdef FLASH_SAVE_CONFIG
ssint inf_load_cfg(u8 *data, size_t len)
{
	if (len > OFFSET_CONFIG_IN_EEPROM + EEPROM_SIZE)
		return -2;

	/* Read EEPROM */
	FLASH_0_read_eeprom_block(OFFSET_CONFIG_IN_EEPROM, data, len);
	
	return 0;
}

ssint inf_save_cfg(const u8 *data, size_t len)
{
	if (len > OFFSET_CONFIG_IN_EEPROM + EEPROM_SIZE)
		return -2;

	/* Write EEPROM */
	return FLASH_0_write_eeprom_block(OFFSET_CONFIG_IN_EEPROM, data, len);
}
#endif

const hal_interface_info_t interface_hal = {
	.fn_set_chg = bus_set_chg,
	.fn_reset = sys_reset,
#ifdef FLASH_SAVE_CONFIG
	.fn_load_cfg = inf_load_cfg,
	.fn_save_cfg = inf_save_cfg,
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

void mptt_process(void)
{
	tsl_process();
}