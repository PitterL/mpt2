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

void t25_report_status(t25_data_t *ptr)
{
	object_t25_t *mem = (object_t25_t *)ptr->common.mem;
	
	if (!ptr->cmd)
		return;

	ptr->cmd = mem->cmd = MXT_T25_CMD_COMPLETED;

	if (mem->ctrl & MXT_T25_CTRL_RPTEN)
		object_txx_report_msg(&ptr->common, &ptr->cache.data, sizeof(ptr->cache.data));

	//memset(&ptr->cache, 0 ,sizeof(ptr->cache));
}

void object_t25_report_status(u8 force)
{
	t25_data_t *ptr = &t25_data_status;
	
	if (force) {
		object_txx_report_msg(&ptr->common, &ptr->cache.data, sizeof(ptr->cache.data));
	}else {
		t25_report_status(ptr);
	}
}

void object_t25_data_sync(u8 rw)
{
	t25_data_t *ptr = &t25_data_status;
	object_t25_t *mem = (object_t25_t *)ptr->common.mem;
	
	if (rw != OP_WRITE)
		return;
		
	if (!(mem->ctrl & MXT_T25_CTRL_ENABLE))
		return;
			
	switch(mem->cmd) {
		case MXT_T25_CMD_COMPLETED:
		case MXT_T25_CMD_TEST_SIGNAL_LIMIT:
		case MXT_T25_CMD_TEST_ALL:
			memset(&ptr->cache, 0 ,sizeof(ptr->cache));
			//ptr->cache.data.result = MXT_T25_INFO_RESULT_NO_DATA;
			ptr->cmd = mem->cmd;
			break;
		default:
			ptr->cache.data.result = MXT_T25_INFO_RESULT_INVALID;
			t25_report_status(ptr);
	}
}

#ifdef OBJECT_T15
ssint inspect_button_data(t25_data_t *ptr, u8 channel, u16 reference, u16 cap)
{
	const qtouch_config_t *qdef = (qtouch_config_t *)ptr->common.def;
	const qbutton_config_t *qbtn = &qdef->buttons[0];
	object_t25_t *mem = (object_t25_t *) ptr->common.mem;
	u8 i, id, st, end;
	u16 baseref;
	
	for ( i = 0; i < qdef->num_button; i++) {
		st = qbtn[i].node.origin;
		end = qbtn[i].node.origin + qbtn[i].node.size;
		if (channel >=  st && channel < end) {
			baseref = object_t15_get_button_base_ref(i);
			if (baseref) {	//If Zero, this will failed
				id = MXT_TOUCH_MULTI_T9_INST + i;
				if (cap < mem->siglim[id].lo || cap > mem->siglim[id].up ||
					reference < baseref - mem->sigrangelim[id] || reference > baseref + mem->sigrangelim[id]) {
					ptr->cache.data.result = MXT_T25_INFO_RESULT_SIGNAL_LIMIT;
					ptr->cache.data.info[0] = MXT_TOUCH_KEYARRAY_T15;
					ptr->cache.data.info[1] = i;
	#ifdef OBJECT_T25_EXTENSION
					ptr->cache.data.info[2] = channel;
					ptr->cache.data.info[3] = cap & 0xff;	//LSB
					ptr->cache.data.info[4] = (cap >> 8);
					ptr->cache.data.info[5] = reference & 0xff;
					ptr->cache.data.info[6] = (reference >> 8);
	#endif
					t25_report_status(ptr);
				}
			}
			ptr->cache.counter[INSPECT_BUTTON]++;
			break;
		}
	}
	
	if ((ptr->cache.counter[INSPECT_BUTTON] >= qdef->num_button_channel_count))
		return 0;
	else
		return -2;
}
#endif

#ifdef OBJECT_T9
ssint inspect_surface_slider_data(t25_data_t *ptr, u8 channel, u16 reference, u16 cap)
{
	const qtouch_config_t *qdef = (qtouch_config_t *)ptr->common.def;
	object_t25_t *mem = (object_t25_t *) ptr->common.mem;
	u8 i;
	u16 baseref;
	
	for ( i = 0; i < qdef->num_surfaces_slider; i++) {
		//Note: Since there is only 1 Gain in surface, the first X gain will be decided as base gain.
		baseref = object_t9_get_surface_slider_base_ref(i);	//FIXME: here need add T104 support for baseref get												
		if (baseref) {	//If Zero, this will skip	
			if (cap < mem->siglim[i].lo || cap > mem->siglim[i].up ||
				reference < baseref - mem->sigrangelim[i] || reference > baseref + mem->sigrangelim[i]) {
				ptr->cache.data.result = MXT_T25_INFO_RESULT_SIGNAL_LIMIT;
				ptr->cache.data.info[0] = MXT_TOUCH_MULTI_T9;
				ptr->cache.data.info[1] = i;
#ifdef OBJECT_T25_EXTENSION
				ptr->cache.data.info[2] = channel;
				ptr->cache.data.info[3] = cap & 0xff;	//LSB
				ptr->cache.data.info[4] = (cap >> 8);
				ptr->cache.data.info[5] = reference & 0xff;
				ptr->cache.data.info[6] = (reference >> 8);
#endif
				t25_report_status(ptr);
			}
		}

		ptr->cache.counter[INSPECT_SURFACE_SLIDER]++;
		break;
	}
	
	if ((ptr->cache.counter[INSPECT_SURFACE_SLIDER] >= qdef->num_surfaces_slider_channel_count))
		return 0;
	else
		return -2;
}
#endif

void inspect_finished(t25_data_t *ptr)
{
	memset(&ptr->cache, 0 ,sizeof(ptr->cache));
	ptr->cache.data.result = MXT_T25_INFO_RESULT_PASS;
	
	t25_report_status(ptr);
}

void t25_inspect_sensor_data(t25_data_t *ptr, u8 channel, u16 reference, u16 cap)
{
	ssint result = 0;

#ifdef OBJECT_T9
	result = inspect_surface_slider_data(ptr, channel, reference, cap);
#endif

#ifdef OBJECT_T15
	if(result == 0) {
		result = inspect_button_data(ptr, channel, reference, cap);
	}
#endif

	if (result == 0) {
		inspect_finished(ptr);
	}
}

void object_api_t25_set_sensor_data(u8 channel, u16 reference, u16 signal, u16 cap)
{
	t25_data_t *ptr = &t25_data_status;
	
	if (!ptr->cmd)
		return;
		
	t25_inspect_sensor_data(ptr, channel, reference, cap);
}

#endif