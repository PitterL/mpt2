/*
 * t9.c
 *
 * Created: 6/9/2019 9:38:09 AM
 *  Author: A41450
 */ 
#ifdef OBJECT_T9

#include <string.h>
#include "arch/tslapi.h"
#include "txx.h"

t9_data_t t9s_data_status[MXT_TOUCH_MULTI_T9_INST];

ssint object_t9_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	t9_data_t *ptr = &t9s_data_status[0];
	qtouch_config_t *qdef = (qtouch_config_t *)def;
	u8 i;

	for (i = 0; i < MXT_TOUCH_MULTI_T9_INST; i++) {
		object_txx_init(&ptr[i].common, rid, def, (object_t9_t *)mem + i, cb);
		if (i < qdef->num_surfaces_slider)
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

	if (!surdef)
		return;

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
#ifndef OBJECT_T15
	const qsurface_config_t *surdef = (qsurface_config_t *)ptr->surdef;
#endif
	object_t9_t *mem = (object_t9_t *) ptr->common.mem;
	/*
	u16 xorigin = mem->xorigin, yorigin = mem->yorigin + ptr->surdef->nodes[NODE_X].origin + ptr->surdef->nodes[NODE_X].size;
	
	txx_cb_param_t params_channel[] = {
		// v for x, h for y
		{ API_SURFACE_CS_START_KEY_V, &xorigin, sizeof(xorigin) },
		{ API_SURFACE_CS_START_KEY_H, &yorigin, sizeof(yorigin) },
		{ API_SURFACE_CS_NUM_KEYS_V, &mem->xsize, sizeof(mem->xsize) },
		{ API_SURFACE_CS_NUM_KEYS_H, &mem->ysize, sizeof(mem->ysize) },
	};
	*/
#ifndef OBJECT_T15
	u8 i;
#ifdef MPTT_MATRIX_NODES
	u8 j, count;	/*	Max nodes should less than 255, or the count may overrun */
#endif
	txx_cb_param_t params_sensor[] = {
		{ API_KEY_PARAMS_THRESHOLD, &mem->tchthr, sizeof(mem->tchthr) },
		{ API_KEY_PARAMS_HYSTERESIS, &mem->tchhyst, sizeof(mem->tchhyst) }
	};

	txx_cb_param_t params_channel[] = {
		{ API_NODE_PARAMS_GAIN, &mem->blen, sizeof(mem->blen) },
	};
	uint8_t channel_group, channel_group_last;
#endif

	nibble_t  movfilter = { .value = 0 };
	u16 amplitude = 0;
	txx_cb_param_t params_touch[] = {
#ifndef OBJECT_T15
		{ API_DEF_TOUCH_DET_INT, &mem->tchdi, sizeof(mem->tchdi) },
		{ API_DEF_ANTI_TCH_DET_INT, &mem->nexttchdi, sizeof(mem->nexttchdi) },
#endif
		{ API_SURFACE_CS_POS_HYST, &mem->movhysti, sizeof(mem->movhysti) },
		//{ API_SURFACE_CS_POS_HYST, &mem->movhystn, sizeof(mem->movhystn) },
		//* Bits 1:0 = IIR (0% / 25% / 50% / 75%), Bit 4 = Enable Median Filter (3-point) */
		{ API_SURFACE_CS_FILT_CFG, &movfilter, sizeof(movfilter) },
		{ API_SURFACE_CS_MIN_CONTACT, &amplitude, sizeof(amplitude) },
	};

	if (rw == OP_WRITE) {	//write	
		movfilter.hi = !(mem->movfilter.hi & 0x8);
		movfilter.lo = (mem->movfilter.hi & 0x7);
		amplitude = mem->amphyst;
	}
	
#ifndef OBJECT_T15
	if ((mem->ctrl & MXT_T9_CTRL_ENABLE) || (rw == OP_READ)) {
		if (surdef) {
	#ifdef MPTT_MATRIX_NODES
			// Note: for read operation, there first XY channel value be valid
	
			channel_group_last = 0xff;
			for (count = 0, i = surdef->nodes[NODE_X].origin; i < surdef->nodes[NODE_X].origin + surdef->nodes[NODE_X].size; i++) {
				for (j = surdef->nodes[NODE_Y].origin; j < surdef->nodes[NODE_Y].origin + surdef->nodes[NODE_Y].size; j++, count++) {
					object_txx_op(&ptr->common, params_sensor, ARRAY_SIZE(params_sensor), count, rw);
					
					channel_group = QTOUCH_MAP_CALL(ptr->common.def, to_channel)(i, true);
					if (channel_group_last != channel_group) {
						object_txx_op(&ptr->common, params_channel, ARRAY_SIZE(params_channel), channel_group, rw);
						channel_group_last = channel_group;
					}
					
					if (rw == OP_READ)
						break;
				}
		
				if (rw == OP_READ)
					break;
			}
	#else
			// Note: for read operation, there first Y channel value be valid

			// Sensor channel parameter for X channel 
			if (rw == OP_WRITE) {
				channel_group_last = 0xff;
				for (i = surdef->nodes[NODE_X].origin; i < surdef->nodes[NODE_X].origin + surdef->nodes[NODE_X].size; i++) {
					object_txx_op(&ptr->common, params_sensor, ARRAY_SIZE(params_sensor), i, rw);
					
					channel_group = QTOUCH_MAP_CALL(ptr->common.def, to_channel)(i, true);
					if (channel_group_last != channel_group) {
						object_txx_op(&ptr->common, params_channel, ARRAY_SIZE(params_channel), channel_group, rw);
						channel_group_last = channel_group;
					}		
				}
			}
	
			//  Sensor channel parameter for Y channel
			channel_group_last = 0xff;
			for (i = surdef->nodes[NODE_Y].origin; i < surdef->nodes[NODE_Y].origin + surdef->nodes[NODE_Y].size; i++) {
				object_txx_op(&ptr->common, params_sensor, ARRAY_SIZE(params_sensor), i, rw);
				
				channel_group = QTOUCH_MAP_CALL(ptr->common.def, to_channel)(i, true);
				if (channel_group_last != channel_group) {
					object_txx_op(&ptr->common, params_channel, ARRAY_SIZE(params_channel), channel_group, rw);
					channel_group_last = channel_group;
				}
				
				if (rw == OP_READ)
					break;
			}
	#endif
		}
	}
#endif
	// Touch parameters
	object_txx_op(&ptr->common, params_touch, ARRAY_SIZE(params_touch), 0, rw);
	
	if (rw == OP_READ) {	// read
		mem->movfilter.hi = (movfilter.lo & 0x7) | (movfilter.hi ? 0 : 0x8);
		mem->movfilter.lo = 0;
		mem->amphyst = amplitude & 0xff;
	}
	
	t9_set_unsupport_area(ptr);
}

void object_t9_start(u8 loaded)
{
	t9_data_t *ptr = &t9s_data_status[0];
	u8 i;
		
	for (i = 0; i < MXT_TOUCH_MULTI_T9_INST; i++) {
		t9_data_sync(ptr + i, loaded ? OP_WRITE : OP_READ);
	}
}

void object_t9_data_sync(u8 rw)
{
	t9_data_t *ptr = &t9s_data_status[0];
	u8 i;
	
	for (i = 0; i < MXT_TOUCH_MULTI_T9_INST; i++) {
		t9_data_sync(ptr + i, rw);
	}
}

void t9_report_status(u8 rid, const t9_point_status_t *pt, u8 res_bit, const mpt_api_callback_t *cb)
{
#ifdef OBJECT_T5
	object_t5_t message;
	
	memset(&message, 0, sizeof(message));
		
	message.reportid = rid;
	message.data[0] = pt->status;
	
	if (res_bit <= 10) {
		message.data[1] = (pt->pos.x >> 2);
		message.data[2] = (pt->pos.y >> 2);
		message.data[3] = ((pt->pos.x & 0x3F) << 2) | (pt->pos.y & 0x3F);
	} else {
		message.data[1] = (pt->pos.x >> 4);
		message.data[2] = (pt->pos.y >> 4);
		message.data[3] = ((pt->pos.x & 0xF) << 4) | (pt->pos.y & 0xF);
	}
	
	message.data[4] = 1;
	message.data[5] = 1;
	
	MPT_API_CALLBACK(cb, write_message)(&message);
#endif
}

void object_t9_report_status(u8 force)
{
	t9_data_t *ptr = &t9s_data_status[0];
	const qsurface_config_t *surdef;
	u8 i, j;
	
	if (!force)
		return;
	
	for (i = 0; i < MXT_TOUCH_MULTI_T9_INST; i++) {
		for (j = 0; j < MXT_TOUCH_MULTI_T9_RIDS; j++) {
			surdef = ((const qsurface_config_t *)ptr[i].surdef);
			if (surdef)
				t9_report_status(ptr[i].common.rid + j, &ptr[i].points[j], surdef->resolution_bit, ptr[i].common.cb);
		}
	}
}

//Note: Since there is only 1 Gain in surface, the first X gain will be decided as base gain. If different Gain set in touch.h, need extra code to detect
u16 object_t9_get_surface_slider_base_ref(u8 inst, u8 channel)
{
	t9_data_t *ptr =  &t9s_data_status[0];
	object_t9_t *mem;
	const qsurface_config_t *surdef = (qsurface_config_t *)ptr->surdef;
	if (inst >= MXT_TOUCH_MULTI_T9_INST)
		return 0;

	if (!surdef)
		return 0;
	
	mem = (object_t9_t *)ptr[inst].common.mem;
	if (!(mem->ctrl & MXT_T9_CTRL_ENABLE))
		return 0;

#ifdef MPTT_MATRIX_NODES
	//FIXME: to decide whether itself?
#else
	if (!((channel >=  surdef->nodes[NODE_X].origin && channel <surdef->nodes[NODE_X].origin + surdef->nodes[NODE_X].size) ||
		(channel >=  surdef->nodes[NODE_Y].origin && channel < surdef->nodes[NODE_Y].origin + surdef->nodes[NODE_Y].size)))
		return 0;
#endif
	return (tsapi_t6_get_sensor_base_ref() << /*NODE_GAIN_DIG*/(((object_t9_t *)ptr[inst].common.mem)->blen & 0xF));
}

#ifdef OBJECT_T9_ORIENT
//Note this resolution transform only could room in, if you want larger resolution, should modify in the qtouch lib 
void transfer_pos(t9_data_t *ptr, t9_range_t *ppos)
{
	object_t9_t *mem = (object_t9_t *) ptr->common.mem;
	const qsurface_config_t *surdef = (qsurface_config_t *)ptr->surdef;
	u16 resol_max;
	t9_range_t point;
	u16 xrange, yrange, tmp;
	
	if (!surdef)
		return;
	
	resol_max = surdef->resolution_max;
	
	xrange = mem->xrange ? mem->xrange : resol_max;
	yrange =  mem->yrange ? mem->yrange : resol_max;
	
    point.x = ppos->x;
    point.y = ppos->y;

#ifdef OBJECT_T9_ORIENT_RESOLUTION
	// Re-adjust axis
    if (xrange < resol_max) {
		point.x = (u16) (((u32)ppos->x * (xrange + 1)) >> surdef->resolution_bit);
    }

	if (yrange < resol_max) {
		point.y = (u16) (((u32)ppos->y * (yrange + 1)) >> surdef->resolution_bit);
    }
#endif
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
	const qsurface_config_t *surdef;
	object_t9_t *mem;
	t9_point_status_t point, *pt;

	if (inst > MXT_TOUCH_MULTI_T9_INST)
		return -2;

	if (id > MXT_TOUCH_MULTI_T9_RIDS)
		return -3;
	
	ptr =  &t9s_data_status[inst];
	mem = (object_t9_t *) ptr->common.mem;
	surdef = (const qsurface_config_t *)ptr->surdef;
	
	if (!surdef)
		return -4;
		
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
			if (mem->ctrl & MXT_T9_CTRL_RPTEN) {
				t9_report_status(ptr->common.rid + id, &point, surdef->resolution_bit, ptr->common.cb);	//Each Touch finger has own ID
#ifdef OBJECT_T9_USE_STATE_CB
				if (surdef->set_touch_state) {
					surdef->set_touch_state(inst, id, status, x, y, surdef->resolution_max);
				}
#endif
			}
		}
	}
	
	return 0;
}

#endif