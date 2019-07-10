/*
 * t9.c
 *
 * Created: 6/9/2019 9:38:09 AM
 *  Author: A41450
 */ 
#ifdef OBJECT_T9

#include <string.h>
#include "../tslapi.h"
#include "txx.h"

t9_data_t t9s_data_status[MXT_TOUCH_MULTI_T9_INST];

ssint object_t9_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	t9_data_t *ptr = &t9s_data_status[0];
	qtouch_config_t *qdef = (qtouch_config_t *)def;
	u8 i;

	for (i = 0; i < MXT_TOUCH_MULTI_T9_INST; i++) {
		object_txx_init(&ptr[i].common, rid, def, (object_t9_t *)mem + i, cb);
		ptr[i].surdef = &qdef->surface_sliders[i];
		rid += MXT_TOUCH_MULTI_T9_RIDS;
	}
	
	return 0;
}

void t9_set_unsupport_area(t9_data_t *ptr)
{
	qtouch_config_t *qcfg = (qtouch_config_t *)ptr->common.def;
	const qsurface_config_t *surdef = (qsurface_config_t *)ptr->surdef;
	object_t9_t *mem = (object_t9_t *) ptr->common.mem;
	
	mem->xorigin = 0;
	mem->xsize = qcfg->matrix_nodes[NODE_X].size;
	mem->yorigin = 0;
	mem->ysize = qcfg->matrix_nodes[NODE_Y].size;

#ifndef OBJECT_WRITEBACK
	if (mem->xsize || mem->ysize)
		mem->ctrl |= MXT_T9_CTRL_ENABLE;
	else
		mem->ctrl &= ~MXT_T9_CTRL_ENABLE;
		
	mem->ctrl |= MXT_T9_CTRL_RPTEN;
#endif

	if (!mem->xrange || mem->xrange > surdef->resolution_max)
		mem->xrange = surdef->resolution_max;

	if (!mem->yrange || mem->yrange > surdef->resolution_max)
		mem->yrange = surdef->resolution_max;

#ifdef OBJECT_WRITEBACK
	mem->akscfg = 0;

	mem->mrgtimeout = 0;
	mem->movhystn = 0;
	//temp
	mem->numtouch = 1;
	mem->mrghyst = 0;
	mem->mrgthr = 0;
	mem->amphyst = 0;	
	
	mem->xloclip = 0;
	mem->xhiclip = 0;
	mem->yloclip = 0;
	mem->yloclip = 0;
	mem->xedgectrl = 0;
	mem->xedgedist = 0;
	mem->yedgectrl = 0;
	mem->yedgedist = 0;
	
	mem->jumplimit = 0;
	mem->xpitch = 0;
	mem->ypitch = 0;
#endif
}

void t9_data_sync(t9_data_t *ptr, u8 rw)
{
	const qsurface_config_t *surdef = (qsurface_config_t *)ptr->surdef;
	object_t9_t *mem = (object_t9_t *) ptr->common.mem;
	/*
	u16 xorigin = mem->xorigin, yorigin = mem->yorigin + ptr->surdef->nodes[NODE_X].origin + ptr->surdef->nodes[NODE_X].size;
	
	txx_cb_param_t params_channel[] = {
		// v for x, h for y
		{ SURFACE_CS_START_KEY_V, &xorigin, sizeof(xorigin) },
		{ SURFACE_CS_START_KEY_H, &yorigin, sizeof(yorigin) },
		{ SURFACE_CS_NUM_KEYS_V, &mem->xsize, sizeof(mem->xsize) },
		{ SURFACE_CS_NUM_KEYS_H, &mem->ysize, sizeof(mem->ysize) },
	};
	*/

	u8 i;
	txx_cb_param_t params_sensor[] = {
		{ NODE_PARAMS_GAIN, &mem->blen, sizeof(mem->blen) },
		{ KEY_PARAMS_THRESHOLD, &mem->tchthr, sizeof(mem->tchthr) },
		{ KEY_PARAMS_HYSTERESIS, &mem->tchhyst, sizeof(mem->tchhyst) }
	};
	
	nibble_t  movfilter;
	txx_cb_param_t params_touch[] = {
		{ DEF_TOUCH_DET_INT, &mem->tchdi, sizeof(mem->tchdi) },
		{ DEF_ANTI_TCH_DET_INT, &mem->nexttchdi, sizeof(mem->nexttchdi) },
		{ SURFACE_CS_POS_HYST, &mem->movhysti, sizeof(mem->movhysti) },
		//{ SURFACE_CS_POS_HYST, &mem->movhystn, sizeof(mem->movhystn) },
		{ SURFACE_CS_FILT_CFG, &movfilter, sizeof(movfilter) },
	};

	if (rw == OP_WRITE) {	//write		
		// Channel using
		t9_set_unsupport_area(ptr);
		movfilter.lo = (mem->movfilter >> 4);
	}
	
	// Note: for read operation, there first Y channel value be valid
	
	// Sensor channel parameter for X channel 
	if (rw == OP_WRITE) {
		for (i = surdef->nodes[NODE_X].origin; i < surdef->nodes[NODE_X].origin + surdef->nodes[NODE_X].size; i++) {
			object_txx_op(&ptr->common, params_sensor, ARRAY_SIZE(params_sensor), i, rw);		
		}
	}
	
	//  Sensor channel parameter for Y channel
	for (i = surdef->nodes[NODE_Y].origin; i < surdef->nodes[NODE_Y].origin + surdef->nodes[NODE_Y].size; i++) {
		object_txx_op(&ptr->common, params_sensor, ARRAY_SIZE(params_sensor), i, rw);
		if (rw == OP_READ)
			break;
	}
	
	// Touch parameters
	object_txx_op(&ptr->common, params_touch, ARRAY_SIZE(params_touch), 0, rw);
	
	if (rw == OP_READ) {	// read
		mem->movfilter = (movfilter.value << 4);
		t9_set_unsupport_area(ptr);
	}
}

void object_t9_start(u8 loaded)
{
	t9_data_t *ptr = &t9s_data_status[0];
	u8 i;
			
	if (loaded)
		return;
		
	for (i = 0; i < MXT_TOUCH_MULTI_T9_INST; i++) {
		t9_data_sync(ptr + i, OP_READ);
	}
}

void object_t9_process(u8 rw)
{
	t9_data_t *ptr = &t9s_data_status[0];
	u8 i;
	
	for (i = 0; i < MXT_TOUCH_MULTI_T9_INST; i++) {
		t9_data_sync(ptr + i, rw);
	}
}

void t9_report_status(u8 rid, const t9_point_status_t *pt, const mpt_api_callback_t *cb)
{
#ifdef OBJECT_T5
	object_t5_t message;
	
	memset(&message, 0, sizeof(message));
		
	message.reportid = rid;
	message.data[0] = pt->status;
	message.data[1] = (pt->pos.x >> 4);
	message.data[2] = (pt->pos.y >> 4);
	message.data[3] = ((pt->pos.x & 0xF) << 4) | (pt->pos.y & 0xF);
	message.data[4] = 1;
	message.data[5] = 1;
	
	MPT_API_CALLBACK(cb, cb_write_message)(&message);
#endif
}

void object_t9_report_status(u8 force)
{
	t9_data_t *ptr = &t9s_data_status[0];
	u8 i, j;
	
	if (!force)
		return;
	
	for (i = 0; i < MXT_TOUCH_MULTI_T9_INST; i++) {
		for (j = 0; j < MXT_TOUCH_MULTI_T9_RIDS; j++) {
			t9_report_status(ptr[i].common.rid + j, &ptr[i].points[j], ptr[i].common.cb);
		}
	}
}

//Note: Since there is only 1 Gain in surface, the first X gain will be decided as base gain. If different Gain set in touch.h, need extra code to detect
u16 object_t9_get_surface_slider_base_ref(u8 inst, u8 channel)
{
	t9_data_t *ptr =  &t9s_data_status[0];
	object_t9_t *mem;
	const qsurface_config_t *surdef;
	if (inst >= MXT_TOUCH_MULTI_T9_INST)
		return 0;
	
	mem = (object_t9_t *)ptr[inst].common.mem;
	if (!(mem->ctrl & MXT_T9_CTRL_ENABLE))
		return 0;
		
	surdef = (qsurface_config_t *)ptr->surdef;
	if (!((channel >=  surdef->nodes[NODE_X].origin && channel <surdef->nodes[NODE_X].origin + surdef->nodes[NODE_X].size) ||
		(channel >=  surdef->nodes[NODE_Y].origin && channel < surdef->nodes[NODE_Y].origin + surdef->nodes[NODE_Y].size)))
		return 0;
	
	return (SENSOR_BASE_REF_VALUE << /*NODE_GAIN_DIG*/(((object_t9_t *)ptr[inst].common.mem)->blen & 0xF));
}

#ifdef OBJECT_T9_ORIENT
//Note this resolution transform only could room in, if you want larger resolution, should modify in the qtouch lib 
void transfer_pos(t9_data_t *ptr, t9_range_t *ppos)
{
	object_t9_t *mem = (object_t9_t *) ptr->common.mem;
	const qsurface_config_t *surdef = (qsurface_config_t *)ptr->surdef;
	const u16 resol_max = surdef->resolution_max;
	t9_range_t point;
	u16 xrange, yrange, tmp;
	
	xrange = mem->xrange ? mem->xrange : resol_max;
	yrange =  mem->yrange ? mem->yrange : resol_max;
	
	// Re-adjust axis
	if (xrange < resol_max)
		point.x = (u16) (((u32)ppos->x * (xrange + 1)) >> surdef->resolution_bit);
	else
		point.x = ppos->x;
	
	if (yrange < resol_max) {
		if (yrange == xrange) {
			point.y = point.x;
		}else
			point.y = (u16) (((u32)ppos->y * (yrange + 1)) >> surdef->resolution_bit);
	}else
		point.y = ppos->y;
	
	// Switch orientation
	if (mem->orient) {
		// As protocol , invert first, then switch
		if (mem->orient & MXT_T9_ORIENT_INVERTX) {
			point.x = xrange - point.x;
		}
		
		if (mem->orient & MXT_T9_ORIENT_INVERTY) {
			point.y = yrange - point.y;
		}
		
		if (mem->orient & MXT_T9_ORIENT_SWITCH) {
			tmp = point.x;
			point.x = point.y;
			point.y = tmp;
		}
	}
	
	ppos->x = point.x;
	ppos->y = point.y;
}
#endif

ssint object_api_t9_set_pointer_location(u8 inst, /* Slot id */u8 id, u8 status, u16 x, u16 y)
{
	t9_data_t *ptr;
	object_t9_t *mem;
	t9_point_status_t point, *pt;

	if (inst > MXT_TOUCH_MULTI_T9_INST)
		return -2;

	if (id > MXT_TOUCH_MULTI_T9_RIDS)
		return -3;
	
	ptr =  &t9s_data_status[inst];
	mem = (object_t9_t *) ptr->common.mem;
		
	if (mem->ctrl & MXT_T9_CTRL_ENABLE) {
		pt = &ptr->points[id];
		if (pt->status != status || pt->pos.x != x || pt->pos.y != y) {
			pt->status = status;
			pt->pos.x = x;
			pt->pos.y = y;

			memcpy(&point, pt, sizeof(point));
		#ifdef OBJECT_T9_ORIENT
			transfer_pos(ptr, &point.pos);
		#endif
			if (mem->ctrl & MXT_T9_CTRL_RPTEN)
				t9_report_status(ptr->common.rid + id, &point, ptr->common.cb);	//Each Touch finger has own ID
		}
	}
	
	return 0;
}

#endif