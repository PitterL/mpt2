/*
 * t15.c
 *
 * Created: 6/9/2019 10:39:14 PM
 *  Author: A41450
 */ 
#ifdef OBJECT_T15

#include <string.h>
#include "../tslapi.h"
#include "txx.h"

t15_data_t t15s_data_status[MXT_TOUCH_KEYARRAY_T15_INST];
ssint object_t15_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	t15_data_t *ptr = &t15s_data_status[0];
	qtouch_config_t *qdef = (qtouch_config_t *)def;
	u8 i;

	for (i = 0; i < MXT_TOUCH_KEYARRAY_T15_INST; i++) {
		object_txx_init(&ptr[i].common, rid, def, (object_t15_t *)mem + i, cb);
		ptr[i].btndef = &qdef->buttons[i];
		rid += MXT_TOUCH_KEYARRAY_T15_RIDS;
	}
	
	return 0;
}

void t15_set_unsupport_area(t15_data_t *ptr)
{
	const qbutton_config_t *btndef = (qbutton_config_t *)ptr->btndef;
	object_t15_t *mem = (object_t15_t *) ptr->common.mem;
	
	// T15 use Y as sensor channel
	mem->xorigin = /*btndef->node.origin*/ 0;
	mem->xsize = /*btndef->node.size*/ 0;
	mem->yorigin = btndef->node.origin;
	mem->ysize = btndef->node.size;

#ifndef OBJECT_WRITEBACK	
	if (mem->xsize || mem->ysize)
		mem->ctrl |= MXT_T15_CTRL_ENABLE;
	else
		mem->ctrl &= ~MXT_T15_CTRL_ENABLE;

	mem->ctrl |= MXT_T15_CTRL_RPTEN;	//Need enable report if not write permission
#endif

#ifdef OBJECT_WRITEBACK
	mem->akscfg = 0;
	mem->rsv = 0;
#endif
}

void t15_data_sync(t15_data_t *ptr, u8 rw)
{
	object_t15_t *mem = (object_t15_t *) ptr->common.mem;
	const qbutton_config_t *btndef = (qbutton_config_t *)ptr->btndef;
	
	u8 i;
	
	txx_cb_param_t params_sensor[] = {
		{ KEY_PARAMS_AKS_GROUP, &mem->akscfg, sizeof(mem->akscfg) },
		{ NODE_PARAMS_GAIN, &mem->blen, sizeof(mem->blen) },
		{ KEY_PARAMS_THRESHOLD, &mem->tchthr, sizeof(mem->tchthr) },
		{ DEF_TOUCH_DET_INT, &mem->tchdi, sizeof(mem->tchdi) },
		{ DEF_ANTI_TCH_DET_INT, &mem->tchdi, sizeof(mem->tchdi) },
		{ KEY_PARAMS_HYSTERESIS, &mem->tchhyst, sizeof(mem->tchhyst)}
	};
	
	if (rw == OP_WRITE) {	//write		
		t15_set_unsupport_area(ptr);
	}
	
	// Sensor channel parameter
	for (i = btndef->node.origin; i < btndef->node.origin + btndef->node.size; i++) {
		object_txx_op(&ptr->common, params_sensor, ARRAY_SIZE(params_sensor), i, rw);
		
		if (rw == OP_READ)
			break;
	}
	
	if (rw == OP_READ) {	// read
		t15_set_unsupport_area(ptr);
	}
}

void object_t15_start(u8 loaded)
{
	t15_data_t *ptr = &t15s_data_status[0];
	u8 i;
	
	if (loaded)
		return;
	
	for (i = 0; i < MXT_TOUCH_KEYARRAY_T15_INST; i++) {
		t15_data_sync(ptr + i, 1);
	}
}

void object_t15_process(u8 rw)
{
	t15_data_t *ptr = &t15s_data_status[0];
	u8 i;
	
	for (i = 0; i < MXT_TOUCH_KEYARRAY_T15_INST; i++) {
		t15_data_sync(ptr + i, rw);
	}
}

void t15_report_status(u8 rid, const t15_button_status_t *btn, const mpt_api_callback_t *cb)
{	
	object_t5_t message;
	
	memset(&message, 0, sizeof(message));
		
	message.reportid = rid;
	message.data[0] = btn->status ? MXT_T15_DETECT : 0;
	message.data[1] = btn->data[0];
	message.data[2] = btn->data[1];
	message.data[3] = btn->data[2];
	message.data[4] = btn->data[3];
	
	MPT_API_CALLBACK(cb, cb_write_message)(&message);
}

void object_t15_report_status(u8 force)
{
	t15_data_t *ptr = &t15s_data_status[0];
	u8 i;
	
	if (!force)
		return;
	
	for (i = 0; i < MXT_TOUCH_KEYARRAY_T15_INST; i++) {
		t15_report_status(ptr[i].common.rid, &ptr[i].button, ptr[i].common.cb);	
	}
}

u16 object_t15_get_button_base_ref(u8 inst)
{
	t15_data_t *ptr =  &t15s_data_status[0];
	object_t15_t *mem;
	
	if (inst >= MXT_TOUCH_KEYARRAY_T15_INST)
		return 0;
	
	mem = (object_t15_t *) ptr[inst].common.mem;
	if (!(mem->ctrl & MXT_T15_CTRL_ENABLE))
		return 0;
	
	return (SENSOR_BASE_REF_VALUE << /*NODE_GAIN_DIG*/(((object_t15_t *)ptr[inst].common.mem)->blen & 0xF));
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
		
		if (mem->ctrl & MXT_T15_CTRL_ENABLE) {
			if (id >= btndef->node.origin &&  id < btndef->node.origin + btndef->node.size) {
				offset = id - btndef->node.origin;
				status = ptr[i].button.status;
				if (pressed)
					status |= BIT(offset);
				else
					status &= ~BIT(offset);
			
				if (status != ptr[i].button.status) {
					ptr[i].button.status = status;
				
					if (mem->ctrl & MXT_T15_CTRL_RPTEN)
						t15_report_status(ptr[i].common.rid, &ptr[i].button, ptr[i].common.cb);
				}
				break;
			}
		}
	}
	
	return 0;
}

#endif