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

int object_t37_init(u8 rid, const /*sensor_config_t*/void *cfg, void *mem)
{
	t37_data_t *ptr = &t37_status_data;
	sensor_config_t *scfg = (sensor_config_t *)cfg;
	
	ptr->xsize = scfg->matrix_xsize;
	ptr->ysize = scfg->matrix_ysize;
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

void object_t37_set_data_page(u8 page)
{
	t37_data_t *ptr = &t37_status_data;
	ptr->page = page;
}

void object_t37_set_sensor_data(u8 dbgcmd, u8 channel, u16 reference, u16 signal, u16 cap)
{
	t37_data_t *ptr = &t37_status_data;
	s16 pos = -1, data;

	switch(dbgcmd) {
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
			// Check 144u chip for the order
			if (channel < ptr->ysize) {
				pos = channel;
			}else {
				pos = channel - ptr->ysize;
				if (!(pos & 0x1)) //Even
					pos = ptr->ysize + (pos >> 1);
				else {	//Odd
					pos = (ptr->ysize << 1) + (pos >> 1);
				}
			}
			data = reference;
		break;
		default:
		;
	}
	
	check_and_empty_object_t37(dbgcmd, ptr->page);
	if (pos >= 0)
		copy_data_to_buffer(dbgcmd, ptr->page, pos, data);
}