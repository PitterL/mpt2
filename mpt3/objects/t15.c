/*
 * t15.c
 *
 * Created: 6/9/2019 10:39:14 PM
 *  Author: A41450
 */ 
#ifdef OBJECT_T15

#include <string.h>
#include "txx.h"

t15_data_t t15s_data_status[MXT_TOUCH_KEYARRAY_T15_INST];
ssint object_t15_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	t15_data_t *ptr = &t15s_data_status[0];
	qtouch_config_t *qdef = (qtouch_config_t *)def;
	u8 i;

	for (i = 0; i < MXT_TOUCH_KEYARRAY_T15_INST; i++) {
		object_txx_init(&ptr[i].common, rid, def, (object_t15_t *)mem + i, cb);
		if (i < qdef->num_button)
			ptr[i].btndef = &qdef->buttons[i];
		rid += MXT_TOUCH_KEYARRAY_T15_RIDS;
	}
	
	return 0;
}

void t15_set_unsupport_area(t15_data_t *ptr)
{
	const qbutton_config_t *btndef = (qbutton_config_t *)ptr->btndef;
	object_t15_t *mem = (object_t15_t *) ptr->common.mem;
	
	if (!btndef)
		return;
	
	// T15 use Y as sensor channel
	mem->xorigin = /*btndef->node.origin*/ 0;
	mem->xsize = /*btndef->node.size*/ 0;
	mem->yorigin = btndef->node.origin;
	mem->ysize = btndef->node.size;

#ifndef OBJECT_WRITEBACK	
	if (mem->xsize || mem->ysize) {
		mem->ctrl |= MXT_T15_CTRL_ENABLE;
	} else {
		mem->ctrl &= ~MXT_T15_CTRL_ENABLE;
	}
	
	mem->ctrl |= MXT_T15_CTRL_RPTEN;	//Need enable report if not write permission
#endif
}

void t15_data_sync(t15_data_t *ptr, u8 inst, u8 rw)
{
	object_t15_t *mem = (object_t15_t *) ptr->common.mem;
	const qbutton_config_t *btndef = (qbutton_config_t *)ptr->btndef;
	uint8_t channel_group, channel_group_last;
	u8 i;
	
	txx_cb_param_t params_sensor[] = {
#ifdef API_KEY_PARAMS_AKS_GROUP
		{ API_KEY_PARAMS_AKS_GROUP, &mem->akscfg, sizeof(mem->akscfg) },
#endif
#ifdef API_KEY_PARAMS_THRESHOLD
		{ API_KEY_PARAMS_THRESHOLD, &mem->tchthr, sizeof(mem->tchthr) },
#endif
#ifdef API_KEY_PARAMS_HYSTERESIS
		{ API_KEY_PARAMS_HYSTERESIS, &mem->tchhyst, sizeof(mem->tchhyst)},
#endif
	};

	txx_cb_param_t params_sensor_group[] = {
#ifdef API_DEF_TOUCH_DET_INT
		{ API_DEF_TOUCH_DET_INT, &mem->tchdi, sizeof(mem->tchdi) },
#endif
#ifdef API_DEF_ANTI_TCH_DET_INT
		{ API_DEF_ANTI_TCH_DET_INT, &mem->antitchdi, sizeof(mem->antitchdi)},
#endif
	};

	txx_cb_param_t params_channel[] = {
	#ifdef API_NODE_PARAMS_GAIN
		{ API_NODE_PARAMS_GAIN, &mem->blen, sizeof(mem->blen) },
	#endif
	};
	
	if ((mem->ctrl & MXT_T15_CTRL_ENABLE) || rw == OP_READ) {
		if (btndef) {
			// Sensor channel parameter
			channel_group_last = 0xff;
			for (i = btndef->node.origin; i < btndef->node.origin + btndef->node.size; i++) {
				object_txx_op(&ptr->common, params_sensor, ARRAY_SIZE(params_sensor), i, rw);
				
#ifndef OBJECT_T104
				channel_group = QTOUCH_MAP_CALL(ptr->common.def, to_channel)(i, true);
				if (channel_group_last != channel_group) {
					object_txx_op(&ptr->common, params_channel, ARRAY_SIZE(params_channel), channel_group, rw);
					channel_group_last = channel_group;
				}
#endif
				if (rw == OP_READ)
					break;
			}
		}

		if (inst == 0 || rw == OP_READ) {
			object_txx_op(&ptr->common, params_sensor_group, ARRAY_SIZE(params_sensor_group), 0, rw);
		}
	}
	
	t15_set_unsupport_area(ptr);
}

void object_t15_start(u8 loaded)
{
	object_t15_data_sync(loaded ? OP_WRITE : OP_READ);
}

void object_t15_data_sync(u8 rw)
{
	t15_data_t *ptr = &t15s_data_status[0];
	u8 i;
	
	for (i = 0; i < MXT_TOUCH_KEYARRAY_T15_INST; i++) {
		t15_data_sync(ptr + i, i, rw);
	}
}

void object_t15_report_status(u8 force)
{
	t15_data_t *ptr = &t15s_data_status[0];
	u8 i;
	
	if (!force)
		return;
	
	for (i = 0; i < MXT_TOUCH_KEYARRAY_T15_INST; i++) {
		object_txx_report_msg(&ptr[i].common, &ptr[i].button, sizeof(ptr[i].button));
	}
}

bool object_t15_is_enabled(u8 inst_id)
{
	t15_data_t *ptr = &t15s_data_status[0];
	object_t15_t *mem;

	if (inst_id < MXT_TOUCH_KEYARRAY_T15_INST) {
		mem = (object_t15_t *)ptr[inst_id].common.mem;
		return !!(mem->ctrl & MXT_T15_CTRL_ENABLE);
	}

	return false;
}

ssint object_api_t15_set_button_status(/* Slot id */u8 id, u8 pressed)
{
	t15_data_t *ptr =  &t15s_data_status[0];
	object_t15_t *mem = (object_t15_t *) ptr->common.mem;
	const qbutton_config_t *btndef;
	u8 offset, i;
	u32 status;
	
	for (i = 0, offset = 0; i < MXT_TOUCH_KEYARRAY_T15_INST; i++) {
		mem = (object_t15_t *) ptr[i].common.mem;
		btndef = (qbutton_config_t *)ptr[i].btndef;
		if (!btndef)
			continue;
				
		if (mem->ctrl & MXT_T15_CTRL_ENABLE) {
			if (id >= btndef->node.origin &&  id < btndef->node.origin + btndef->node.size) {
				offset = id - btndef->node.origin;
				status = ptr[i].button.keystate.value;
				if (pressed) {
#ifdef OBJECT_T126
					if (!object_api_t126_node_skipped(id))
#endif
					{
						status |= BIT32(offset);
					}
				} else {
					status &= ~BIT32(offset);
				}
				
				if (status != ptr[i].button.keystate.value) {
					ptr[i].button.keystate.value = status;
					ptr[i].button.status = status? MXT_T15_DETECT : 0;
				
					if (mem->ctrl & MXT_T15_CTRL_RPTEN) {
						object_txx_report_msg(&ptr[i].common, &ptr[i].button, sizeof(ptr[i].button));
#ifdef OBJECT_T15_USE_STATE_CB
						if (btndef->set_button_state) {
							btndef->set_button_state(i, status);
						}
#endif
					}
				}
				break;
			}
		}
	}
	
	return 0;
}

#endif