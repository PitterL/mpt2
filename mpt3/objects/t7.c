/*
 * t7.c
 *
 * Created: 6/9/2019 10:30:07 AM
 *  Author: A41450
 */ 
#ifdef OBJECT_T7

#include "txx.h"

t7_data_t t7_data_status;
ssint object_t7_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	return object_txx_init(&t7_data_status, rid, def, mem, cb);
}

void t7_set_unsupport_area(object_t7_t *mem)
{
	mem->cfg &= 0x3C;	// active to idle MSB is not supported
}

void t7_data_sync(const txx_data_t *ptr, u8 rw)
{
	object_t7_t *mem = (object_t7_t *)ptr->mem;
	
	txx_cb_param_t params[] = {
#ifdef API_DEF_TOUCH_MEASUREMENT_PERIOD_MS
		{ API_DEF_TOUCH_MEASUREMENT_PERIOD_MS, &mem->actvaqint, sizeof(mem->actvaqint) },
#endif
#ifdef API_DEF_TOUCH_MEASUREMENT_IDLE_PERIOD_MS
		{ API_DEF_TOUCH_MEASUREMENT_IDLE_PERIOD_MS, &mem->idleacqint, sizeof(mem->idleacqint) },
#endif
#ifdef API_DEF_TOUCH_ACTIVE_IDLE_TIMEOUT
		{ API_DEF_TOUCH_ACTIVE_IDLE_TIMEOUT, &mem->actv2idleto, sizeof(mem->actv2idleto) },
#endif
	};
	
	object_txx_op(ptr, params, ARRAY_SIZE(params), 0, rw);
	
	t7_set_unsupport_area(mem);
}

void object_t7_start(u8 loaded)
{
	t7_data_t *ptr = &t7_data_status;
	
	t7_data_sync(ptr, loaded ? OP_WRITE : OP_READ);
}

void object_t7_data_sync(u8 rw)
{
	t7_data_t *ptr = &t7_data_status;
	
	t7_data_sync(ptr, rw);
}

ssint object_t7_report_overflow(void)
{
	t7_data_t *ptr = &t7_data_status;
	object_t7_t *mem = (object_t7_t *)ptr->mem;
	
	return !(mem->cfg & MXT_T7_CFG_OVFRPTSUP);
}

#endif