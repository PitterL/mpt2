/*
 * t37.c
 *
 * Created: 6/9/2019 12:23:41 PM
 *  Author: A41450
 */ 

#include <string.h>
#include "../mptt.h"
#include "t37.h"
#include "t6.h"
#include "t8.h"

t37_data_t t37_status_data;

ssint object_t37_init(u8 rid, const /*sensor_config_t*/void *cfg, void *mem, void *cb)
{
	t37_data_t *ptr = &t37_status_data;
	sensor_config_t *scfg = (sensor_config_t *)cfg;
	
	ptr->matrix_xsize = scfg->matrix_xsize;
	ptr->matrix_ysize = scfg->matrix_ysize;
	ptr->measallow = scfg->measallow;
	ptr->mem = (object_t37_t *)mem;
	
	return 0;
}

void object_t37_start(void) 
{
	
}

void copy_data_to_buffer(u8 cmd, u8 page, u8 relative, u16 data)
{
	t37_data_t *ptr = &t37_status_data;
	object_t37_t *mem = (object_t37_t *)ptr->mem;
	int pos, pagesize;
	
	pagesize = sizeof(mem->data);
	pos = relative - page * pagesize;
	if (pos >= 0 && pos < pagesize) {
		mem->data[pos] = data;	
	}
}

void check_and_empty_object_t37(u8 dbgcmd, u8 page)
{
	t37_data_t *ptr = &t37_status_data;
	object_t37_t *mem = (object_t37_t *)ptr->mem;
	
	if (dbgcmd != mem->mode || page != mem->page) {
		memset(mem, 0, sizeof(*mem));
		mem->mode = dbgcmd;
		mem->page = page;
	}
}

void object_t37_set_data_page(u8 dbgcmd, u8 page)
{
	t37_data_t *ptr = &t37_status_data;
	
	ptr->cmd = dbgcmd;
	ptr->page = page;
}

void object_t37_set_sensor_data(u8 channel, u16 reference, u16 signal, u16 cap)
{
	t37_data_t *ptr = &t37_status_data;
	s16 pos = -1, data;

	switch(ptr->cmd) {
		case MXT_DIAGNOSTIC_PAGEUP:
		break;
		case MXT_DIAGNOSTIC_PAGEDOWN:
		break;
		case MXT_DIAGNOSTIC_MC_DELTA: 
		break;
		case MXT_DIAGNOSTIC_MC_REF:
		break;
		case MXT_DIAGNOSTIC_DC_DATA:
		break;
		case MXT_DIAGNOSTIC_DEVICE_INFO:
		break;
		case MXT_DIAGNOSTIC_PRODUCT_DATA:
		break;
		case MXT_DIAGNOSTIC_SC_DELTA:
			pos = channel;
			data = signal - reference;
		break;
		case MXT_DIAGNOSTIC_SC_REF:
		case MXT_DIAGNOSTIC_SC_SIGNAL:
			// re-organize the data order, see protocol
			if (channel < ptr->matrix_ysize) {
				pos = channel;
			}else {
				pos = channel - ptr->matrix_ysize;
				if (!(pos & 0x1)) //Even
					pos = ptr->matrix_ysize + (pos >> 1);
				else {	//Odd
					pos = (ptr->matrix_ysize << 1) + (pos >> 1);
				}
			}
			if (ptr->cmd == MXT_DIAGNOSTIC_SC_REF)
				data = reference;
			else {
				/* cc value formula:
					 (val & 0x0F)*0.00675 + ((val >> 4) & 0x0F)*0.0675 + ((val >> 8) & 0x0F)*0.675 + ((val >> 12) & 0x3) * 6.75
				*/
				// so multiply 1000 here
				data = (cap & 0x0F) * 7 + ((cap >> 4) & 0x0F) * 68 + ((cap >> 8) & 0x0F) * 675 + ((cap >> 12) & 0x3) * 6750;		
			}
		default:
		;
	}
	
	check_and_empty_object_t37(ptr->cmd, ptr->page);
	if (pos >= 0)
		copy_data_to_buffer(ptr->cmd, ptr->page, pos, data);
}