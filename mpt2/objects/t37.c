/*
 * t37.c
 *
 * Created: 6/9/2019 12:23:41 PM
 *  Author: A41450
 */ 
#ifdef OBJECT_T37
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
	const u8 pagesize = T37_DATA_SIZE;
#define DATA_DIRTY_MAGIC_MASK BIT(0)

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
	u8 pos = row * QTOUCH_CONFIG_MATRIX_Y(ptr->common.def);
	
	for ( i = 0; i < QTOUCH_CONFIG_MATRIX_Y(ptr->common.def); i++ )
		copy_node_data_to_buffer(cmd, page, pos + i, data, DATA_AVE);	
}

void copy_col_data_to_buffer(u8 cmd, u8 page, u8 col, u16 data)
{
	t37_data_t *ptr = &t37_data_status;
	u8 i;
	u8 pos = col;
	
	for ( i = 0; i < QTOUCH_CONFIG_MATRIX_X(ptr->common.def); i++ ) {
		copy_node_data_to_buffer(cmd, page, pos, data, DATA_AVE);
		pos += QTOUCH_CONFIG_MATRIX_Y(ptr->common.def);
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

void object_api_t37_set_data_page(u8 cmd, u8 page)
{
	t37_data_t *ptr = &t37_data_status;
	
	ptr->status.cmd = cmd;
	ptr->status.page = page;
}

u16 t37_get_data(u8 cmd, u8 channel, u16 reference, u16 signal, u16 cap)
{
	switch(cmd) {
		case MXT_DIAGNOSTIC_KEY_DELTA:
		case MXT_DIAGNOSTIC_MC_DELTA: 
		case MXT_DIAGNOSTIC_SC_DELTA:
			return (u16)((s16)signal- (s16)reference);
		case MXT_DIAGNOSTIC_KEY_REF:
		case MXT_DIAGNOSTIC_MC_REF:
		case MXT_DIAGNOSTIC_SC_REF:
			return reference;
		case MXT_DIAGNOSTIC_MC_SIGNAL: 
		case MXT_DIAGNOSTIC_KEY_SIGNAL:
		case MXT_DIAGNOSTIC_SC_SIGNAL:
			return cap;
		default:
			;
	};
	
	return 0;
}

void t37_put_data(t37_data_t *ptr, u8 cmd, u8 page, u8 channel, u16 data)
{
	s8 pos; 	//If t37 buffer size more than 128, this value will over flow
	
	switch(cmd) {
		case MXT_DIAGNOSTIC_KEY_DELTA:
		case MXT_DIAGNOSTIC_KEY_REF:
		case MXT_DIAGNOSTIC_KEY_SIGNAL:
			pos = channel;
			copy_node_data_to_buffer(cmd, page, pos, data, DATA_NEW);
			break;
		case MXT_DIAGNOSTIC_MC_DELTA:
		case MXT_DIAGNOSTIC_MC_REF:
		case MXT_DIAGNOSTIC_MC_SIGNAL:
			if (channel < QTOUCH_CONFIG_MATRIX_X(ptr->common.def)) {
				copy_row_data_to_buffer(cmd, page, channel, data);
			}else {
				copy_col_data_to_buffer(cmd, page, channel - QTOUCH_CONFIG_MATRIX_X(ptr->common.def), data);
			}
		break;
		case MXT_DIAGNOSTIC_SC_DELTA:
			//Y channel First
			if (channel < QTOUCH_CONFIG_MATRIX_X(ptr->common.def)) {
				pos = channel + QTOUCH_CONFIG_MATRIX_Y(ptr->common.def);
			}else {
				pos = channel - QTOUCH_CONFIG_MATRIX_X(ptr->common.def);
			}
			copy_node_data_to_buffer(cmd, page, pos, data, DATA_NEW);
		break;
		case MXT_DIAGNOSTIC_SC_REF:
		case MXT_DIAGNOSTIC_SC_SIGNAL:
			// re-organize the data order, see protocol
			if (channel >= QTOUCH_CONFIG_MATRIX_X(ptr->common.def)) {
				pos = channel - QTOUCH_CONFIG_MATRIX_X(ptr->common.def);
			}else {
				// X channel more, X placed as alternative ascending; Y channels more, x placed as ordered ascending 
				if (QTOUCH_CONFIG_MATRIX_X(ptr->common.def) > QTOUCH_CONFIG_MATRIX_Y(ptr->common.def)) {
					pos = channel >> 1;
					if (channel & 0x1) {	//Odd
						pos += QTOUCH_CONFIG_MATRIX_Y(ptr->common.def);
					}
					pos += QTOUCH_CONFIG_MATRIX_Y(ptr->common.def);
				}else {
					pos = channel + QTOUCH_CONFIG_MATRIX_Y(ptr->common.def);
				}
			}
			copy_node_data_to_buffer(cmd, page, pos, data, DATA_NEW);
		default:
			;
	};
}

void object_api_t37_set_sensor_data(u8 channel, u16 reference, u16 signal, u16 cap)
{
	t37_data_t *ptr = &t37_data_status;
	u16 data;
	
	check_and_empty_object_t37(ptr->status.cmd, ptr->status.page);

	data = t37_get_data(ptr->status.cmd, channel, reference, signal, cap);
	t37_put_data(ptr, ptr->status.cmd, ptr->status.page, channel, data);
}

#endif