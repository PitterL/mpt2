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

void object_t9_start(void)
{

}

void t9_set_unsupport_area(t9_data_t *ptr)
{
	object_t9_t *mem = (object_t9_t *) ptr->common.mem;
	
	mem->ctrl = 0xff;
	mem->akscfg = 0;
	mem->blen = 0;
	mem->tchthr = 0;
}

void t9_set_resolution(t9_data_t *ptr)
{
	
	
}

void object_t9_process(void)
{
	t9_data_t *ptr = &t9_data_status;
	object_t9_t *mem = (object_t9_t *)ptr->common.mem;
	u8 i;
	
	txx_cb_param_t params[] = {
		{ DEF_TOUCH_DET_INT, &mem->tchdi, sizeof(mem->tchdi) },
	};
	
	for ( i = mem->xorigin; i < mem->xorigin + mem->xsize; i++ ) {
		object_txx_process(&ptr->common, params, ARRAY_SIZE(params), i);	
	}
	
	for ( i = mem->yorigin; i < mem->yorigin + mem->ysize; i++ ) {
		object_txx_process(&ptr->common, params, ARRAY_SIZE(params), i);
	}
	
	object_txx_process(&ptr->common, params, ARRAY_SIZE(params), 0);
	
	t9_set_unsupport_area(ptr);
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

ssint object_t9_set_pointer_location(/* Slot id */u8 id, u8 status, u16 x, u16 y)
{
	t9_data_t *ptr = &t9_data_status;
	t9_point_status_t *pt;

	if (id > MXT_TOUCH_MULTI_T9_RIDS)
		return -2;
	
	pt = &ptr->points[id];
	
	if (pt->status != status || pt->pos.x != x || pt->pos.y != y) {	
		pt->status = status;
		pt->pos.x = x;
		pt->pos.y = y;
		
		t9_report_status(ptr->common.rid + id, pt);
	}
	
	return 0;
}