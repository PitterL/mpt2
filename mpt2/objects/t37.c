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
#ifdef OBJECT_T37_DEBUG_PLATFORM_INFO
	qtouch_config_t *qdef = (qtouch_config_t *)def;
#endif

	object_txx_init(&t37_ptr->common, rid, def, mem, cb);
	
#ifdef OBJECT_T37_DEBUG_PLATFORM_INFO
	if (qdef) {
		if (qdef->chip_cb.get_sigrow) {
			t37_ptr->sigrow = qdef->chip_cb.get_sigrow(&t37_ptr->sigrow_len);
		}
	#ifdef MPTT_FUSE_CHECK
		if (qdef->chip_cb.get_fuse) {
			t37_ptr->fuse = qdef->chip_cb.get_fuse(&t37_ptr->fuse_len);
		}
	#endif
	}
#endif
	return 0;
}

void object_t37_start(void) 
{
	
}

void copy_page_data(u8 cmd, u8 page, u8 offset, const void* data, u8 len)
{
	t37_data_t *ptr = &t37_data_status;
	object_t37_t *mem = (object_t37_t *)ptr->common.mem;
	const u8 pagesize = T37_DATA_SIZE;
	u8 maxlen;
	
	maxlen = pagesize - offset;
	if (len > maxlen) {
		len = maxlen;
	}
	
	memcpy((u8 *)mem->data + offset, data, len);
}

void copy_data_to_target_page(u8 cmd, u8 page, u8 offset, const void* data, u8 len)
{
	const u8 pagesize = T37_DATA_SIZE;
	u8 current = page;
	
	while(offset >= pagesize) {
		offset -= pagesize;
		current--;
	}
	
	//data value for current page, copy to page buffer
	if (current == 0) {
		copy_page_data(cmd, page, offset, data, len);
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
#ifdef OBJECT_T8_RSD_SWITCH
	u8 refmode = DBG_NORMAL;
	refmode = object_api_t8_ref_mode();
#endif
	s16 delta = (s16)signal- (s16)reference;
	switch(cmd) {
		case MXT_DIAGNOSTIC_PTC_DELTA:
		case MXT_DIAGNOSTIC_KEY_DELTA:
		case MXT_DIAGNOSTIC_MC_DELTA: 
		case MXT_DIAGNOSTIC_SC_DELTA:
#ifdef OBJECT_T8_RSD_SWITCH
			if (refmode == DBG_CAP)
				return signal;
			else
#endif
				return (u16)delta;
		case MXT_DIAGNOSTIC_PTC_REF:
		case MXT_DIAGNOSTIC_KEY_REF:
		case MXT_DIAGNOSTIC_MC_REF:
		case MXT_DIAGNOSTIC_SC_REF:
#ifdef OBJECT_T8_RSD_SWITCH
			if (refmode == DBG_CAP)
				return cap;
			else
#endif
				return reference;
		case MXT_DIAGNOSTIC_PTC_SIGNAL:
		case MXT_DIAGNOSTIC_MC_SIGNAL: 
		case MXT_DIAGNOSTIC_KEY_SIGNAL:
		case MXT_DIAGNOSTIC_SC_SIGNAL:
#ifdef OBJECT_T8_RSD_SWITCH
			if (refmode == DBG_CAP)
				return (u16)delta;
			else
#endif
				return cap;
		default:
			;
	};
	
	return 0;
}

void t37_put_data(t37_data_t *ptr, u8 cmd, u8 page, u8 channel, u16 value)
{
	u8 pos; 	//If t37 buffer size more than 128, this value will over flow
		
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
			copy_data_to_target_page(cmd, page, pos << 1, &value, sizeof(value));
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
		    copy_data_to_target_page(cmd, page, pos << 1, &value, sizeof(value));
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
			copy_data_to_target_page(cmd, page, pos << 1, &value, sizeof(value));
		break;
#endif
		default:
			;
	};
}

void t37_set_rsf_data(u8 cmd, u8 page, u8 channel, u16 reference, u16 signal, u16 cap)
{	
	t37_data_t *ptr = &t37_data_status;
	u16 data;
		
	data = t37_get_data(cmd, channel, reference, signal, cap);
	t37_put_data(ptr, cmd, page, channel, data);
}

#ifdef OBJECT_T126
void t37_set_low_power_data(u8 cmd, u8 page, u8 channel, u16 reference, u16 signal, u16 cap)
{
	dbg_low_power_data_t data;
	ssint node;
	bool lowpower;
	
	if (page > 0)
		return;
	
	node = object_api_t126_get_low_power_node();
	if (node != channel)
		return;
	
	lowpower = object_api_t126_get_low_power_status();
	data.delta = (u16)((s16)signal- (s16)reference);
	data.ref = reference;
	data.signal = signal;
	data.status = (u8)!lowpower;
	data.signal_raw = cap;
	
	copy_page_data(cmd, page, 0, &data, sizeof(data));
}
#endif
#ifdef OBJECT_T37_DEBUG_PLATFORM_INFO
void t37_set_diagnostic_platform_info(u8 cmd, u8 page)
{
	const t37_data_t * const ptr = &t37_data_status;

	if (page == 0) {
		if (ptr->sigrow) {
			copy_page_data(cmd, page, 0, ptr->sigrow, ptr->sigrow_len);
		}
	} else if (page == 1) {
#ifdef MPTT_FUSE_CHECK
		if (ptr->fuse) {
			copy_page_data(cmd, page, 0, ptr->fuse, ptr->fuse_len);
		}
#endif
	}
}
#endif
ssint object_api_t37_set_sensor_data(u8 channel, /*const cap_sample_value_t * const*/ const void * cv)
{
	t37_data_t *ptr = &t37_data_status;
	object_t37_t *mem = (object_t37_t *)ptr->common.mem;
	const cap_sample_value_t * const cval = (const cap_sample_value_t *)cv;
	ssint ret = 0;
	
	u8 cmd = ptr->status.cmd;
	u8 page = ptr->status.page;

	/* check whether we have command */

	switch(cmd) {
		case MXT_DIAGNOSTIC_PTC_DELTA:
		case MXT_DIAGNOSTIC_KEY_DELTA:
		case MXT_DIAGNOSTIC_MC_DELTA:
		case MXT_DIAGNOSTIC_SC_DELTA:
		case MXT_DIAGNOSTIC_PTC_REF:
		case MXT_DIAGNOSTIC_KEY_REF:
		case MXT_DIAGNOSTIC_MC_REF:
		case MXT_DIAGNOSTIC_SC_REF:
		case MXT_DIAGNOSTIC_PTC_SIGNAL:
		case MXT_DIAGNOSTIC_MC_SIGNAL:
		case MXT_DIAGNOSTIC_KEY_SIGNAL:
		case MXT_DIAGNOSTIC_SC_SIGNAL:
		{
			if (cval) {
				t37_set_rsf_data(cmd, page, channel, cval->reference, cval->signal, cval->cccap);
			}
		}
		break;
#ifdef OBJECT_T126
#ifdef OBJECT_T37_DEBUG_LOWPOWER_INFO
		case MXT_DIAGNOSTIC_LOW_POWER_MODE:
		{
			if (cval) {
				t37_set_low_power_data(cmd, page, channel, cval->reference, cval->signal, cval->cccap);
			}
		}
		break;
#endif
#endif
#ifdef OBJECT_T37_DEBUG_PLATFORM_INFO
		case MXT_DIAGNOSTIC_DEVICE_INFO:
		{
			t37_set_diagnostic_platform_info(cmd, page);
		}
		break;
#endif
		default:
			ret = -1;
	};
	
	if (ret == 0) {
		mem->mode = cmd;
		mem->page = page;
	}
	
	return ret;
}

#endif