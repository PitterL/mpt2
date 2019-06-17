/*
 * t37.c
 *
 * Created: 6/9/2019 12:23:41 PM
 *  Author: A41450
 */ 

#include <string.h>
#include "../tslapi.h"
#include "txx.h"

t37_data_t t37_data_status;

ssint object_t37_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	t37_data_t *t37_ptr = &t37_data_status;

	object_txx_init(&t37_ptr->common, rid, def, mem, cb);
	
	return 0;
}

void object_t37_start(void) 
{
	
}

void copy_data_to_buffer(u8 cmd, u8 page, u8 relative, u16 data)
{
	t37_data_t *ptr = &t37_data_status;
	object_t37_t *mem = (object_t37_t *)ptr->common.mem;
	u8 pagesize;
	
	pagesize = T37_DATA_SIZE;
	while(relative >= pagesize) {
		relative -= pagesize;
		page--;
	}
	
	if (page == 0)
		mem->data[relative] = data;	
}

void check_and_empty_object_t37(u8 dbgcmd, u8 page)
{
	t37_data_t *ptr = &t37_data_status;
	object_t37_t *mem = (object_t37_t *)ptr->common.mem;
	
	if (dbgcmd != mem->mode || page != mem->page) {
		memset(mem, 0, sizeof(*mem));
		mem->mode = dbgcmd;
		mem->page = page;
	}
}

void object_t37_set_data_page(u8 cmd, u8 page)
{
	t37_data_t *ptr = &t37_data_status;
	
	ptr->status.cmd = cmd;
	ptr->status.page = page;
}

void object_t37_set_sensor_data(u8 channel, u16 reference, u16 signal, u16 cap)
{
	t37_data_t *ptr = &t37_data_status;
	s16 data;
	u8 pos = T37_DATA_SIZE + 1;

	switch(ptr->status.cmd) {
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
			if (channel >= QTOUCH_CONFIG_VAL(ptr->common.def, matrix_xsize)) {
				pos = channel - QTOUCH_CONFIG_VAL(ptr->common.def, matrix_xsize);
			}else {
				// FIXME: studio not match with protocol?
				/*
				pos = channel >> 1;
				if (channel & 0x1) {	//Odd
					pos += QTOUCH_CONFIG_VAL(ptr->common.def, matrix_ysize);
				}
				pos += QTOUCH_CONFIG_VAL(ptr->common.def, matrix_ysize);
				*/
				pos = channel + QTOUCH_CONFIG_VAL(ptr->common.def, matrix_ysize);
			}
			if (ptr->status.cmd == MXT_DIAGNOSTIC_SC_REF)
				data = reference;
			else {
				/* 
					cc value formula:
					 (val & 0x0F)*0.00675 + ((val >> 4) & 0x0F)*0.0675 + ((val >> 8) & 0x0F)*0.675 + ((val >> 12) & 0x3) * 6.75
					
					Here, multiply 1000 for calculation:
				*/
				data = (cap & 0x0F) * 7 + ((cap >> 4) & 0x0F) * 68 + ((cap >> 8) & 0x0F) * 675 + ((cap >> 12) & 0x3) * 6750;		
			}
		default:
		;
	}
	
	check_and_empty_object_t37(ptr->status.cmd, ptr->status.page);
	if (pos < T37_DATA_SIZE)
		copy_data_to_buffer(ptr->status.cmd, ptr->status.page, pos, (u16)data);
}