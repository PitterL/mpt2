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
enum {
	DATA_NEW,
	DATA_AVE,	
};

void copy_node_data_to_buffer(u8 cmd, u8 page, u8 relative, u16 data, u8 mode)
{
	t37_data_t *ptr = &t37_data_status;
	object_t37_t *mem = (object_t37_t *)ptr->common.mem;
	u8 pagesize;
#define DATA_DIRTY_MAGIC_MASK BIT(0)

	pagesize = T37_DATA_SIZE;
	while(relative >= pagesize) {
		relative -= pagesize;
		page--;
	}
	
	if (page == 0) {
		if (mode == DATA_NEW)
			mem->data[relative] = data;
		else {
			if (mem->data[relative] & DATA_DIRTY_MAGIC_MASK)	//MASK bit mean it has filled data before
				mem->data[relative] = (((s16)data >> 1) + ((s16)mem->data[relative] >> 1)) | DATA_DIRTY_MAGIC_MASK;	//avg
			else
				mem->data[relative] = data | DATA_DIRTY_MAGIC_MASK;
		}
	}
}

void copy_row_data_to_buffer(u8 cmd, u8 page, u8 row, u16 data) 
{
	t37_data_t *ptr = &t37_data_status;
	u8 i;
	u8 pos = row * QTOUCH_CONFIG_VAL(ptr->common.def, matrix_ysize);
	
	for ( i = 0; i < QTOUCH_CONFIG_VAL(ptr->common.def, matrix_ysize); i++ )
		copy_node_data_to_buffer(cmd, page, pos + i, data, DATA_AVE);	
}

void copy_col_data_to_buffer(u8 cmd, u8 page, u8 col, u16 data)
{
	t37_data_t *ptr = &t37_data_status;
	u8 i;
	u8 pos = col;
	
	for ( i = 0; i < QTOUCH_CONFIG_VAL(ptr->common.def, matrix_xsize); i++ ) {
		copy_node_data_to_buffer(cmd, page, pos, data, DATA_AVE);
		pos += QTOUCH_CONFIG_VAL(ptr->common.def, matrix_ysize);
	}
}

void check_and_empty_object_t37(u8 dbgcmd, u8 page)
{
	t37_data_t *ptr = &t37_data_status;
	object_t37_t *mem = (object_t37_t *)ptr->common.mem;
	
	if (dbgcmd != mem->mode || page != mem->page) {
		mem->mode = dbgcmd;
		mem->page = page;
		memset(mem->data, 0, sizeof(mem->data));
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
	s8 pos; 	//If t37 buffer size more than 128, this value will over flow

	check_and_empty_object_t37(ptr->status.cmd, ptr->status.page);

	switch(ptr->status.cmd) {
		case MXT_DIAGNOSTIC_PAGEUP:
		break;
		case MXT_DIAGNOSTIC_PAGEDOWN:
		break;
		case MXT_DIAGNOSTIC_KEY_DELTA:
		case MXT_DIAGNOSTIC_KEY_REF:
		case MXT_DIAGNOSTIC_KEY_SIGNAL:
			if (ptr->status.cmd == MXT_DIAGNOSTIC_KEY_DELTA)
				data = signal- reference;
			else if (ptr->status.cmd == MXT_DIAGNOSTIC_KEY_REF)
				data = reference;
			else
				data = signal;	/* MXT_DIAGNOSTIC_KEY_SIGNAL */
			pos = channel;
			copy_node_data_to_buffer(ptr->status.cmd, ptr->status.page, pos, (u16)data, DATA_NEW);
		break;
		case MXT_DIAGNOSTIC_MC_DELTA: 
		case MXT_DIAGNOSTIC_MC_REF:
			// re-organize the data order, see protocol
			if (ptr->status.cmd == MXT_DIAGNOSTIC_MC_DELTA)
				data = signal- reference;
			else
				data = reference;
			if (channel < QTOUCH_CONFIG_VAL(ptr->common.def, matrix_xsize)) {
				copy_row_data_to_buffer(ptr->status.cmd, ptr->status.page, channel, data);
			}else {
				copy_col_data_to_buffer(ptr->status.cmd, ptr->status.page, channel - QTOUCH_CONFIG_VAL(ptr->common.def, matrix_xsize), data);
			}
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
			copy_node_data_to_buffer(ptr->status.cmd, ptr->status.page, pos, (u16)data, DATA_NEW);
		break;
		case MXT_DIAGNOSTIC_SC_REF:
		case MXT_DIAGNOSTIC_SC_SIGNAL:
			// re-organize the data order, see protocol
			if (channel >= QTOUCH_CONFIG_VAL(ptr->common.def, matrix_xsize)) {
				pos = channel - QTOUCH_CONFIG_VAL(ptr->common.def, matrix_xsize);
			}else {
				// FIXME: studio not match with protocol?
				if (QTOUCH_CONFIG_VAL(ptr->common.def, matrix_xsize) > QTOUCH_CONFIG_VAL(ptr->common.def, matrix_ysize)) {
					pos = channel >> 1;
					if (channel & 0x1) {	//Odd
						pos += QTOUCH_CONFIG_VAL(ptr->common.def, matrix_ysize);
					}
					pos += QTOUCH_CONFIG_VAL(ptr->common.def, matrix_ysize);
				}else {
					pos = channel + QTOUCH_CONFIG_VAL(ptr->common.def, matrix_ysize);
				}
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
			copy_node_data_to_buffer(ptr->status.cmd, ptr->status.page, pos, (u16)data, DATA_NEW);
		break;
		default:
		;
	}
}