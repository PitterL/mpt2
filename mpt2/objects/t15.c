/*
 * t15.c
 *
 * Created: 6/9/2019 10:39:14 PM
 *  Author: A41450
 */ 

#include "../tslapi.h"
#include "txx.h"

t15_data_t t15_data_status;
ssint object_t15_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	return object_txx_init(&t15_data_status, rid, def, mem, cb);
}
/*
void t15_report_status(const t15_data_t *ptr)
{
	object_t5_t message;
	
	memset(&message, 0, sizeof(message));
	
	message.reportid = ptr->rid;
	message.data[0] = ptr->status;
	message.data[1] = (ptr->pos.x >> 4);
	message.data[2] = (ptr->pos.y >> 4);
	message.data[3] = ((ptr->pos.x & 0xF) << 4) | (ptr->pos.y & 0xF);
	message.data[4] = 1;
	message.data[5] = 1;
	
	mpt_write_message(&message);
}

void object_t15_report_status(void)
{
	t15_data_t *ptr = &t15_status_data[0];
	u8 i;
	
	for (i = 0; i < MXT_TOUCH_MULTI_T9_RIDS; i++) {
		t15_report_status(&ptr[i]);
	}
}*/

void t15_set_unsupport_area(object_t15_t *mem)
{
	
}

void object_t15_process(void)
{
	t15_data_t *ptr = &t15_data_status;
	object_t15_t *mem = (object_t15_t *)ptr->mem;
	int i;
	
	txx_cb_param_t params[] = {
		{ KEY_PARAMS_AKS_GROUP, &mem->akscfg, sizeof(mem->akscfg) },
		{ NODE_PARAMS_GAIN, &mem->blen, sizeof(mem->blen) },	
		{ KEY_PARAMS_THRESHOLD, &mem->tchthr, sizeof(mem->tchthr) },
		{ DEF_TOUCH_DET_INT, &mem->tchdi, sizeof(mem->tchdi) },
		{ DEF_ANTI_TCH_DET_INT, &mem->tchdi, sizeof(mem->tchdi) },
		{ KEY_PARAMS_HYSTERESIS, &mem->tchhyst, sizeof(mem->tchhyst)}
	};
	
	//Fixme: not finished here, expamle for self cap only
	//Need verify xstart/xsize
	for ( i = 0; i < mem->xsize; i++ )
		object_txx_process(ptr, params, ARRAY_SIZE(params), mem->xorigin + i);
	
	
	for ( i = 0; i < mem->ysize; i++ )
		object_txx_process(ptr, params, ARRAY_SIZE(params), mem->yorigin + i);
	
	t15_set_unsupport_area(mem);
}