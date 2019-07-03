/*
 * t25.c
 *
 * Created: 6/9/2019 10:28:17 PM
 *  Author: A41450
 */ 
#ifdef OBJECT_T25

#include <string.h>
#include "../tslapi.h"
#include "txx.h"

t25_data_t t25_data_status;
ssint object_t25_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	t25_data_t *t25_ptr = &t25_data_status;

	object_txx_init(&t25_ptr->common, rid, def, mem, cb);
	
	return 0;
}

void object_t25_start(u8 unused)
{
	
}

static void report_status(t25_data_t *ptr, const void *data, u8 size)
{
#ifdef OBJECT_T5
	object_t5_t message;
	
	//memset(&message, 0, sizeof(message));
	if (size > sizeof(message.data));
		size = sizeof(message.data);

	message.reportid = ptr->common.rid;
	memcpy(message.data, data, size);
	
	MPT_API_CALLBACK(ptr->common.cb, cb_write_message)(&message);
#endif
}

void t25_report_status(t25_data_t *ptr)
{
	object_t25_t *mem = (object_t25_t *)ptr->common.mem;
	
	if (!(mem->ctrl & MXT_T25_CTRL_RPTEN))
		return;
	
	if (ptr->cache.result) {
		report_status(ptr, &ptr->cache, sizeof(ptr->cache));
		memset(&ptr->cache, 0 ,sizeof(ptr->cache));
	
		mem->cmd = MXT_T25_CMD_COMPLETED;
	}
}

void object_t25_report_status(u8 force)
{
	t25_data_t *ptr = &t25_data_status;
	
	if (force || ptr->cache.result) {
		report_status(ptr, &ptr->cache, sizeof(ptr->cache));
	}
}

void object_t25_process(u8 rw)
{
	t25_data_t *ptr = &t25_data_status;
	object_t25_t *mem = (object_t25_t *)ptr->common.mem;
	
	if (!(mem->ctrl & MXT_T25_CTRL_ENABLE))
		return;
		
	switch(mem->cmd) {
		case MXT_T25_CMD_COMPLETED:
		case MXT_T25_CMD_TEST_SIGNAL_LIMIT:
		case MXT_T25_CMD_TEST_ALL:
			break;
		default:
			ptr->cache.result = MXT_T25_INFO_RESULT_INVALID;
	}
	
	t25_report_status(ptr);
}

#ifdef OBJECT_T15
void t25_inspect_button_data(t25_data_t *ptr, u8 channel, u16 reference, u16 cap)
{
	const qtouch_config_t *qdef = (qtouch_config_t *)ptr->common.def;
	const qbutton_config_t *qbtn = &qdef->buttons[0];
	object_t25_t *mem = (object_t25_t *) ptr->common.mem;
	u8 i, id, st, end;
	u16 baseref;
	
	for ( i = 0; i < qdef->num_buttons; i++) {
		st = qbtn[i].node.origin;
		end = qbtn[i].node.origin + qbtn[i].node.size;
		if (channel >=  st && channel < end) {
			baseref = object_t15_get_button_base_ref(i);
			id = MXT_TOUCH_MULTI_T9_INST + i;
			if (cap < mem->siglim[id].lo || cap > mem->siglim[id].up ||
				reference < baseref - mem->sigrangelim[id] || reference > baseref + mem->sigrangelim[id]) {
				ptr->cache.result = MXT_T25_INFO_RESULT_SIGNAL_LIMIT;
				ptr->cache.info[0] = MXT_TOUCH_KEYARRAY_T15;
				ptr->cache.info[1] = i;
#ifdef OBJECT_T25_EXTENSION
				ptr->cache.info[2] = channel;
				ptr->cache.info[3] = cap & 0xff;	//LSB
				ptr->cache.info[4] = (cap >> 8);
				ptr->cache.info[5] = reference & 0xff;
				ptr->cache.info[6] = (reference >> 8);
#endif
			}else {
				if ( i + 1 == qdef->num_buttons && channel + 1 == end) {	//Last channel
					if (!ptr->cache.result)
						ptr->cache.result = MXT_T25_INFO_RESULT_PASS;
				}
			}
			break;
		}
	}
	
	if (ptr->cache.result)
		t25_report_status(ptr);
}
#endif

#ifdef OBJECT_T9
void t25_inspect_surface_slider_data(t25_data_t *ptr, u8 channel, u16 reference, u16 cap)
{
	const qtouch_config_t *qdef = (qtouch_config_t *)ptr->common.def;
	const qsurface_config_t *qsurf = &qdef->surface_sliders[0];
	object_t25_t *mem = (object_t25_t *) ptr->common.mem;
	u8 i;
	u16 baseref;

	for ( i = 0; i < qdef->num_surfaces_slider; i++) {
		if ((channel >=  qsurf[i].xnode.origin && channel < qsurf[i].xnode.origin + qsurf[i].xnode.size) ||
			(channel >=  qsurf[i].ynode.origin && channel < qsurf[i].ynode.origin + qsurf[i].ynode.size)) {
			//Note: Since there is only 1 Gain in surface, the first X gain will be decided as base gain.
			baseref = object_t9_get_surface_slider_base_ref(i);	//FIXME: here need add T104 support for baseref get
			if (cap < mem->siglim[i].lo || cap > mem->siglim[i].up ||
				reference < baseref - mem->sigrangelim[i] || reference > baseref + mem->sigrangelim[i]) {
				ptr->cache.result = MXT_T25_INFO_RESULT_SIGNAL_LIMIT;
				ptr->cache.info[0] = MXT_TOUCH_MULTI_T9;
				ptr->cache.info[1] = i;
#ifdef OBJECT_T25_EXTENSION
				ptr->cache.info[2] = channel;
				ptr->cache.info[3] = cap & 0xff;	//LSB
				ptr->cache.info[4] = (cap >> 8);
				ptr->cache.info[5] = reference & 0xff;
				ptr->cache.info[6] = (reference >> 8);
#endif
			}else {
				if ( i + 1 == qdef->num_surfaces_slider && channel + 1 == qsurf[i].ynode.origin + qsurf[i].ynode.size) {	//Last channel
					if (!ptr->cache.result)
						ptr->cache.result = MXT_T25_INFO_RESULT_PASS;
				}
			}
			break;
		}
	}
	
	if (ptr->cache.result)
		t25_report_status(ptr);
}
#endif

void object_api_t25_set_sensor_data(u8 channel, u16 reference, u16 signal, u16 cap)
{
	t25_data_t *ptr = &t25_data_status;
	object_t25_t *mem = (object_t25_t *) ptr->common.mem;
	
	if (!(mem->ctrl & MXT_T25_CTRL_ENABLE))
		return;
		
	if (!(mem->cmd == MXT_T25_CMD_TEST_SIGNAL_LIMIT || mem->cmd == MXT_T25_CMD_TEST_ALL))
		return;
	
#ifdef OBJECT_T15
	t25_inspect_button_data(ptr, channel, reference, cap);
#endif
#ifdef OBJECT_T9
	t25_inspect_surface_slider_data(ptr, channel, reference, cap);
#endif
}

#endif