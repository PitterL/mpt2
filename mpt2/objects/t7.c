/*
 * t7.c
 *
 * Created: 6/9/2019 10:30:07 AM
 *  Author: A41450
 */ 

#include "../tslapi.h"
#include "txx.h"

t7_data_t t7_data_status;
ssint object_t7_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	return object_txx_init(&t7_data_status, rid, def, mem, cb);
}

void t7_set_unsupport_area(object_t7_t *mem)
{
	mem->idleacqint = 0;
	mem->actv2idleto = 0;
}

void t7_data_sync(const txx_data_t *ptr, u8 rw)
{
	object_t7_t *mem = (object_t7_t *)ptr->mem;
	
	txx_cb_param_t params[] = {
		{ DEF_TOUCH_MEASUREMENT_PERIOD_MS, &mem->actvaqint, sizeof(mem->actvaqint) },
	};
	
	object_txx_op(ptr, params, ARRAY_SIZE(params), 0, rw);
	
	t7_set_unsupport_area(mem);
}

void object_t7_start(u8 loaded)
{
	t7_data_t *ptr = &t7_data_status;
	
	if (loaded)
		return;
	
	t7_data_sync(ptr, 1);
}

void object_t7_process(void)
{
	t7_data_t *ptr = &t7_data_status;
	
	t7_data_sync(ptr, 0);
}