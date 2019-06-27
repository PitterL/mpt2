/*
 * t9.c
 *
 * Created: 6/9/2019 9:38:09 AM
 *  Author: A41450
 */ 

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
	const qsurface_config_t *surdef = (qsurface_config_t *)ptr->surdef;
	object_t9_t *mem = (object_t9_t *) ptr->common.mem;
	
	mem->xorigin = /*surdef->xnode.origin*/0;
	mem->xsize = surdef->xnode.size;
	mem->yorigin = /*surdef->ynode.origin*/0;
	mem->ysize = surdef->ynode.size;

	if (mem->xsize && mem->ysize)
		mem->ctrl = 0xff;
	else
		mem->ctrl = 0;
	mem->akscfg = 0;
	
	//self cap
	if (object_api_t8_measuring_self()) {
		mem->blen = 0;
		mem->tchthr = 0;
		mem->tchhyst = 0;	
	}
	
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
}

void t9_data_sync(t9_data_t *ptr, u8 rw)
{
	object_t9_t *mem = (object_t9_t *) ptr->common.mem;
	u8 i, count;
	/*
	u16 xorigin = mem->xorigin, yorigin = mem->yorigin + ptr->surdef->xnode.origin + ptr->surdef->xnode.size;
	
	txx_cb_param_t params_channel[] = {
		// v for x, h for y
		{ SURFACE_CS_START_KEY_V, &xorigin, sizeof(xorigin) },
		{ SURFACE_CS_START_KEY_H, &yorigin, sizeof(yorigin) },
		{ SURFACE_CS_NUM_KEYS_V, &mem->xsize, sizeof(mem->xsize) },
		{ SURFACE_CS_NUM_KEYS_H, &mem->ysize, sizeof(mem->ysize) },
	};
	*/
	
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

	if (!rw) {	//write		
		// Channel using
		t9_set_unsupport_area(ptr);
		movfilter.lo = (mem->movfilter >> 4);
	}
	
	// Mutual only
	if (object_api_t8_measuring_mutual()) {	//Test Mutual cap
		// Sensor channel parameter
		count = rw ? 1 :  mem->xsize;
		for (i = mem->xorigin; i < mem->xorigin + count; i++) {
			object_txx_op(&ptr->common, params_sensor, ARRAY_SIZE(params_sensor), i, rw);
		}
	
		if (!rw) {	//write
			for (i = mem->yorigin; i < mem->yorigin + mem->ysize; i++) {
				object_txx_op(&ptr->common, params_sensor, ARRAY_SIZE(params_sensor), i, rw);
			}
		}
	}
	
	// Touch parameters
	object_txx_op(&ptr->common, params_touch, ARRAY_SIZE(params_touch), 0, rw);
	
	if (rw) {	// read
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
		t9_data_sync(ptr + i, 1);
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
}

void object_t9_report_status(void)
{
	t9_data_t *ptr = &t9s_data_status[0];
	u8 i, j;
	
	for (i = 0; i < MXT_TOUCH_MULTI_T9_INST; i++) {
		for (j = 0; j < MXT_TOUCH_MULTI_T9_RIDS; j++) {
			t9_report_status(ptr[i].common.rid + j, &ptr[i].points[j], ptr[i].common.cb);
		}
	}
}

void transfer_pos(t9_data_t *ptr, t9_range_t *ppos)
{
	object_t9_t *mem = (object_t9_t *) ptr->common.mem;
	const qsurface_config_t *surdef = (qsurface_config_t *)ptr->surdef;
	const u16 resol_max = (1 << surdef->resolution_bit) - 1;
	t9_range_t point;
	u16 xrange, yrange, tmp;
	
	xrange = mem->xrange ? mem->xrange : resol_max;
	yrange =  mem->yrange ? mem->yrange : resol_max;
	
	// Re-adjust axis
	if (xrange < resol_max)
		point.x = (u16) (((u32)ppos->x * (xrange + 1) >> surdef->resolution_bit) - 1);
	else
		point.x = ppos->x;
	
	if (yrange < resol_max) {
		if (yrange == xrange) {
			point.y = point.x;
		}else
			point.y = (u16) (((u32)ppos->y * (yrange + 1) >> surdef->resolution_bit)) - 1;
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

ssint object_t9_set_pointer_location(u8 inst, /* Slot id */u8 id, u8 status, u16 x, u16 y)
{
	t9_data_t *ptr;
	t9_point_status_t point, *pt;

	if (inst > MXT_TOUCH_MULTI_T9_INST)
		return -2;

	if (id > MXT_TOUCH_MULTI_T9_RIDS)
		return -3;
	
	ptr =  &t9s_data_status[inst];
	
	pt = &ptr->points[id];
	if (pt->status != status || pt->pos.x != x || pt->pos.y != y) {
		pt->status = status;
		pt->pos.x = x;
		pt->pos.y = y;
		
		memcpy(&point, pt, sizeof(point));
		transfer_pos(ptr, &point.pos);
		t9_report_status(ptr->common.rid + id, &point, ptr->common.cb);	//Each Touch finger has own ID
	}
	
	return 0;
}