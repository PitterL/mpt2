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

#include <atmel_start.h>
#include <atomic.h>

#include <touch.h>
#include <nvmctrl_basic.h>
#include "mptt.h"

enum {
	BUS_STOP = 0,
	BUS_READ,
	BUS_WRITE,
	BUS_COLLISION,
	BUS_ERROR,
	NUM_BUS_STATES
};

typedef struct bus_monitor {
	int counter[NUM_BUS_STATES];
	union {
		/*struct {
			u8 low;
			u8 high;
		} bytes;*/
		u8 val[2];
		u16 value;
	} regaddr;
	
	int state;
} bus_monitor_t;

bus_monitor_t data_bus;

int handle_bus_event(int state, u8 *val)
{
	bus_monitor_t *bus = &data_bus;
	int count;
	int result = 0;
	
	switch(state) {
		case BUS_WRITE:
			count = bus->counter[BUS_WRITE]++;
			if (count < sizeof(bus->regaddr)) {
				bus->regaddr.val[count] = *val;
			}else {
				result = mpt_mem_write(bus->regaddr.value + count - sizeof(bus->regaddr), *val);
			}
		break;
		case BUS_READ:
			count = bus->counter[BUS_READ]++;
			result = mpt_mem_read(bus->regaddr.value, count, val);
		break;
		case BUS_STOP:
			if (bus->counter[BUS_WRITE] > sizeof(bus->regaddr) || bus->counter[BUS_READ])
				memset(bus, 0, sizeof(*bus));
		break;
		case BUS_COLLISION:
		case BUS_ERROR:
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
	} else {
		I2C_send_nack();
	}
}

u8 tbuf[64];
u8 tcount;
void bus_read_handler(int flag)
{ 
	u8 val = 0x5A;

	handle_bus_event(BUS_READ, &val);
	I2C_write(val);
	
	tbuf[tcount++] = val;
}

void bus_write_handler(void)
{
	u8 val;
	int result;
	
	val = I2C_read();
	result = handle_bus_event(BUS_WRITE, &val);
	if (result) {
		I2C_send_nack();
	}else {
		I2C_send_ack();
	}
}

void bus_stop_handler(void)
{
	if (tcount) {
		memset(tbuf, 0, sizeof(tbuf));
		tcount = 0;
	}
	
	handle_bus_event(BUS_STOP, NULL);	
}

void bus_error_handler(void)
{
	handle_bus_event(BUS_ERROR, NULL);
}

int bus_init(void) 
{
	bus_monitor_t *bus = &data_bus;
	
	bus->state = BUS_STOP;
	
	I2C_set_read_callback(bus_read_handler);
	I2C_set_write_callback(bus_write_handler);
	I2C_set_address_callback(bus_address_handler);
	I2C_set_stop_callback(bus_stop_handler);
	I2C_set_collision_callback(bus_error_handler);
	I2C_set_bus_error_callback(bus_error_handler);
	
	return 0;
}

int bus_start(void) 
{
	I2C_open();
		
	return 0;
}

//extern qtm_surface_cs_control_t qtm_surface_cs_control1;
//extern qtm_touch_key_data_t qtlib_key_data_set1[DEF_NUM_SENSORS];

/* Container structure for sensor group */
extern qtm_acquisition_control_t qtlib_acq_set1;

/* Acquisition set 1 - General settings */
extern qtm_acq_node_group_config_t ptc_qtlib_acq_gen1;

/* Surface Config */
extern qtm_surface_cs_config_t qtm_surface_cs_config1;

/* Surface Data */
extern qtm_surface_contact_data_t qtm_surface_cs_data1;

void bus_assert_irq(void)
{
	bus_monitor_t *bus = &data_bus;
	int count;
		
	if (bus->state == BUS_READ ||
		bus->state == BUS_WRITE) {
		CHG_set_dir(PORT_DIR_IN);		
	}else {
		count = mpt_get_message_count();
		if (count) {
			CHG_set_level(0);
			CHG_set_dir(PORT_DIR_OUT);
		}
	}
}

void inf_system_reset(void)
{
	RSTCTRL.SWRR = 0x1;
}

void inf_read_mem(u16 address, u8 *data, int len)
{
	/* Read Memory, Flash or EEPROM */
}

void inf_write_mem(u16 address, u8 *data, int len)
{
	/* Write Memory, Flash or EEPROM */	
}

int inf_load_cfg(u8 *data, int len)
{
	if (len > OFFSET_CONFIG_IN_EEPROM + EEPROM_SIZE)
		return -2;

	/* Read EEPROM */		
	FLASH_0_read_eeprom_block(OFFSET_CONFIG_IN_EEPROM, data, len);
	
	return 0;
}

int inf_save_cfg(const u8 *data, int len)
{	
	if (len > OFFSET_CONFIG_IN_EEPROM + EEPROM_SIZE)
		return -2;

	/* Write EEPROM */	
	return FLASH_0_write_eeprom_block(OFFSET_CONFIG_IN_EEPROM, data, len);
} 

void tch_calibration(void)
{
	qtm_acq_node_group_config_t *qtacq = &ptc_qtlib_acq_gen1;
	int i;
	
	for (i = 0; i < (int)qtacq->num_sensor_nodes; i++) {
		qtm_calibrate_sensor_node(&qtlib_acq_set1, i);
	}
}

hal_interface_info_t interface_hal;
int mpt_interface_init(void)
{
	const qtm_surface_cs_config_t *qtcfg = &qtm_surface_cs_config1;
	hal_interface_info_t *hal = &interface_hal;
	
	hal->matrix_xsize = qtcfg->number_of_keys_h;
	hal->matrix_ysize = qtcfg->number_of_keys_v;
	
	hal->fn_load_cfg = inf_load_cfg;
	hal->fn_save_cfg = inf_save_cfg;
	hal->fn_calibrate = tch_calibration;
	hal->fn_reset = inf_system_reset;
	//hal->fn_mem_write = inf_write_mem;
	//hal->fn_mem_read =  inf_read_mem;
	
	mpt_init(hal);

	bus_init();

	return 0;
}

int mpt_start(void)
{	
	mpt_chip_start();

	bus_start();
	
	mpt_chip_reportall();
	
	return 0;
}

void mpt_process(void)
{
	const qtm_surface_contact_data_t *qtsf = &qtm_surface_cs_data1;

	t9_set_pointer_location(0, qtsf->qt_surface_status,  qtsf->h_position, qtsf->v_position);
	bus_assert_irq();
}