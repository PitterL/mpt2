/*
 * t9.c
 *
 * Created: 6/9/2019 9:38:09 AM
 *  Author: A41450
 */ 

#include <string.h>
#include "../tslapi.h"
#include "txx.h"

t9_data_t t9_data_status;

ssint object_t9_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	t9_data_t *t9_ptr = &t9_data_status;

	object_txx_init(&t9_ptr->common, rid, def, mem, cb);

	return 0;
}

void t9_set_unsupport_area(t9_data_t *ptr)
{
	object_t9_t *mem = (object_t9_t *) ptr->common.mem;
	
	mem->ctrl = 0xff;
	mem->akscfg = 0;

	//self cap
	//mem->blen = 0;
	//mem->tchthr = 0;
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
	const qtouch_config_t *def = ptr->common.def;
	object_t9_t *mem = (object_t9_t *) ptr->common.mem;
	u16 xorigin = mem->xorigin, yorigin = mem->yorigin + def->matrix_xsize;
	u8 end;
	
	txx_cb_param_t params_channel[] = {
		// v for x, h for y
		{ SURFACE_CS_START_KEY_V, &xorigin, sizeof(xorigin) },
		{ SURFACE_CS_START_KEY_H, &yorigin, sizeof(yorigin) },
		{ SURFACE_CS_NUM_KEYS_V, &mem->xsize, sizeof(mem->xsize) },
		{ SURFACE_CS_NUM_KEYS_H, &mem->ysize, sizeof(mem->ysize) },
	};
	
	txx_cb_param_t params_sensor[] = {
		{ NODE_PARAMS_GAIN, &mem->blen, sizeof(mem->blen) },
		{ KEY_PARAMS_THRESHOLD, &mem->tchthr, sizeof(mem->tchthr) },
		{ KEY_PARAMS_HYSTERESIS, &mem->tchhyst, sizeof(mem->tchhyst) }
	};
	
	nibble_t  movfilter;
	txx_cb_param_t params[] = {
		{ DEF_TOUCH_DET_INT, &mem->tchdi, sizeof(mem->tchdi) },
		{ DEF_ANTI_TCH_DET_INT, &mem->nexttchdi, sizeof(mem->nexttchdi) },
		{ SURFACE_CS_POS_HYST, &mem->movhysti, sizeof(mem->movhysti) },
		//{ SURFACE_CS_POS_HYST, &mem->movhystn, sizeof(mem->movhystn) },
		{ SURFACE_CS_FILT_CFG, &movfilter, sizeof(movfilter) },
	};
	
	int i;
	
	if (!rw) {	//write
		movfilter.lo = (mem->movfilter >> 4);
	}
	
	// Channel using
	object_txx_op(&ptr->common, params_channel, ARRAY_SIZE(params_channel), 0, rw);
	
	// Mutual only
	// Sensor channel parameters
	end = rw ? (u8)xorigin + 1 : (u8)xorigin + mem->xsize;
	for ( i = (u8)xorigin; i < end; i++ ) {
		object_txx_op(&ptr->common, params_sensor, ARRAY_SIZE(params_sensor), i, rw);
	}
	
	if (!rw) {
		for ( i = (u8)yorigin; i < (u8)yorigin + mem->ysize; i++ ) {
			object_txx_op(&ptr->common, params_sensor, ARRAY_SIZE(params_sensor), i, rw);
		}
	}
	
	// TOuch parameters
	object_txx_op(&ptr->common, params, ARRAY_SIZE(params), 0, rw);
	
	if (rw) {	// read
		mem->movfilter = (movfilter.value << 4);
		mem->xorigin = (u8)xorigin;
		mem->yorigin = (u8)yorigin - def->matrix_xsize;
	}
	
	t9_set_unsupport_area(ptr);
}

void object_t9_start(u8 loaded)
{
	t9_data_t *ptr = &t9_data_status;
	
	if (loaded)
		return;
	
	t9_data_sync(ptr, 1);
}

void object_t9_process(void)
{
	t9_data_t *ptr = &t9_data_status;
	
	t9_data_sync(ptr, 0);
}

void t9_report_status(u8 rid, const t9_point_status_t *pt)
{
	t9_data_t *ptr = &t9_data_status;
	
	object_t5_t message;
	
	memset(&message, 0, sizeof(message));
		
	message.reportid = rid;
	message.data[0] = pt->status;
	message.data[1] = (pt->pos.x >> 4);
	message.data[2] = (pt->pos.y >> 4);
	message.data[3] = ((pt->pos.x & 0xF) << 4) | (pt->pos.y & 0xF);
	message.data[4] = 1;
	message.data[5] = 1;
	
	MPT_API_CALLBACK(ptr->common.cb, cb_write_message)(&message);
}

void object_t9_report_status(void)
{
	t9_data_t *ptr = &t9_data_status;
	u8 i;
	
	for (i = 0; i < MXT_TOUCH_MULTI_T9_RIDS; i++) {
		t9_report_status(ptr->common.rid + i, &ptr->points[i]);
	}
}

void transfer_pos(t9_data_t *ptr, t9_range_t *ppos)
{
	const qtouch_config_t *def = ptr->common.def;
	object_t9_t *mem = (object_t9_t *) ptr->common.mem;
	const u16 resol_max = (1 << def->resolution_bit) - 1;
	t9_range_t point;
	u16 xrange, yrange, tmp;
	
	xrange = mem->xrange ? mem->xrange : resol_max;
	yrange =  mem->yrange ? mem->yrange : resol_max;
	
	// Re-adjust axis
	if (xrange < resol_max)
		point.x = (u16) (((u32)ppos->x * (xrange + 1) >> def->resolution_bit) - 1);
	else
		point.x = ppos->x;
	
	if (yrange < resol_max) {
		if (yrange == xrange) {
			point.y = point.x;
		}else
			point.y = (u16) (((u32)ppos->y * (yrange + 1) >> def->resolution_bit)) - 1;
	}else
		point.y = ppos->y;
	
	// Switch orientation
	if (mem->orient) {
		// As protocol , invert first, then switch
		if (mem->orient & MXT_T9_ORIENT_INVERTX) {
			point.x = xrange - point.x;
		}
		
		if (mem->orient & MXT_T9_ORIENT_INVERTY) {
			point.y = xrange - point.y;
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

ssint object_t9_set_pointer_location(/* Slot id */u8 id, u8 status, u16 x, u16 y)
{
	t9_data_t *ptr = &t9_data_status;
	t9_point_status_t point, *pt;

	if (id > MXT_TOUCH_MULTI_T9_RIDS)
		return -2;
		
	pt = &ptr->points[id];
	if (pt->status != status || pt->pos.x != x || pt->pos.y != y) {
		pt->status = status;
		pt->pos.x = x;
		pt->pos.y = y;
		
		memcpy(&point, pt, sizeof(point));
		transfer_pos(ptr, &point.pos);
		t9_report_status(ptr->common.rid + id, &point);
	}
	
	return 0;
}