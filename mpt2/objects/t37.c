/*
 * t37.c
 *
 * Created: 6/9/2019 12:23:41 PM
 *  Author: A41450
 */ 
#ifdef OBJECT_T37
#include <string.h>
#include "arch/tslapi.h"
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

void copy_node_data_to_buffer(u8 cmd, u8 page, u8 relative, u16 data)
{
	t37_data_t *ptr = &t37_data_status;
	object_t37_t *mem = (object_t37_t *)ptr->common.mem;
	const u8 pagesize = T37_DATA_SIZE;
#define DATA_DIRTY_MAGIC_MASK BIT(0)

	mem->mode = cmd;
	mem->page = page;
	
	while(relative >= pagesize) {
		relative -= pagesize;
		page--;
	}
	
	if (page == 0) {
		//data set to current page buffer
		mem->data[relative] = data;
	}
}

void check_and_empty_object_t37_mem(u8 dbgcmd, u8 page)
{
	t37_data_t *ptr = &t37_data_status;
	object_t37_t *mem = (object_t37_t *)ptr->common.mem;
	
	/* check the command and page, if not match, clear the data */
	if ((dbgcmd != mem->mode || page != mem->page )) {
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
	
	// FIXME:
	// If clear data, the debug view may flick in insight, but if not clear, be careful about large channel matrix
	check_and_empty_object_t37_mem(cmd, page);
}

u16 t37_get_data(u8 cmd, u8 channel, u16 reference, u16 signal, u16 cap)
{
	u8 refmode = DBG_NORMAL;
#ifdef OBJECT_T8
	refmode = object_api_t8_ref_mode();
#endif
	switch(cmd) {
		case MXT_DIAGNOSTIC_PTC_DELTA:
		case MXT_DIAGNOSTIC_KEY_DELTA:
		case MXT_DIAGNOSTIC_MC_DELTA: 
		case MXT_DIAGNOSTIC_SC_DELTA:
			if (refmode == DBG_CAP)
				return signal;
			else
				return (u16)((s16)signal- (s16)reference);
		case MXT_DIAGNOSTIC_PTC_REF:
		case MXT_DIAGNOSTIC_KEY_REF:
		case MXT_DIAGNOSTIC_MC_REF:
		case MXT_DIAGNOSTIC_SC_REF:
			if (refmode == DBG_CAP)
				return cap;
			else
				return reference;
		case MXT_DIAGNOSTIC_PTC_SIGNAL:
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
#ifdef OBJECT_T15
		case MXT_DIAGNOSTIC_KEY_DELTA:
		case MXT_DIAGNOSTIC_KEY_REF:
		case MXT_DIAGNOSTIC_KEY_SIGNAL:
		case MXT_DIAGNOSTIC_PTC_DELTA:
		case MXT_DIAGNOSTIC_PTC_REF:
		case MXT_DIAGNOSTIC_PTC_SIGNAL:
#endif
		case MXT_DIAGNOSTIC_MC_DELTA:
		case MXT_DIAGNOSTIC_MC_REF:
		case MXT_DIAGNOSTIC_MC_SIGNAL:
			pos = channel;
			copy_node_data_to_buffer(cmd, page, pos, data);
		break;
#ifdef OBJECT_T111
		case MXT_DIAGNOSTIC_SC_REF:
		case MXT_DIAGNOSTIC_SC_SIGNAL:
		    // re-organize the data order, see protocol
		    if (channel < QT_MATRIX_X_SIZE(ptr->common.def)) {
    		    // X channel more, X placed as alternative ascending; Y channels more, x placed as ordered ascending
    		    if (QT_MATRIX_X_SIZE(ptr->common.def) > QT_MATRIX_Y_SIZE(ptr->common.def)) {
        		    pos = channel >> 1;
        		    if (channel & 0x1) {	//Odd
#ifdef DEBUG_FORMAT_HAWKEYE
                        pos += (QT_MATRIX_X_SIZE(ptr->common.def) >> 1);
#else
                        pos += QT_MATRIX_Y_SIZE(ptr->common.def);
#endif 
       		    }
        		    pos += QT_MATRIX_Y_SIZE(ptr->common.def);
        	    } else {
        		    pos = channel + QT_MATRIX_Y_SIZE(ptr->common.def);
    		    }
    	    } else if(channel < QT_MATRIX_X_SIZE(ptr->common.def) + QT_MATRIX_Y_SIZE(ptr->common.def)) {
    		    pos = channel - QT_MATRIX_X_SIZE(ptr->common.def);
    	    } else {
    		    pos = channel;
		    }
		    copy_node_data_to_buffer(cmd, page, pos, data);
            break;
        case MXT_DIAGNOSTIC_SC_DELTA:
			//Y channel First
			if (channel < QT_MATRIX_X_SIZE(ptr->common.def)) {
				pos = channel + QT_MATRIX_Y_SIZE(ptr->common.def);
			} else if(channel < QT_MATRIX_X_SIZE(ptr->common.def) + QT_MATRIX_Y_SIZE(ptr->common.def)) {
				pos = channel - QT_MATRIX_X_SIZE(ptr->common.def);
			} else {
				pos = channel;
			}
			copy_node_data_to_buffer(cmd, page, pos, data);
		break;
#endif
		default:
			;
	};
}

void object_api_t37_set_sensor_data(u8 channel, u16 reference, u16 signal, u16 cap)
{
	t37_data_t *ptr = &t37_data_status;
	u16 data;

	/* check whether we have command */
	if (!ptr->status.cmd)
		return;

	data = t37_get_data(ptr->status.cmd, channel, reference, signal, cap);
	t37_put_data(ptr, ptr->status.cmd, ptr->status.page, channel, data);
}

#endif