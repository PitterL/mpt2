/*
 * t7.c
 *
 * Created: 6/9/2019 10:30:07 AM
 *  Author: A41450
 */ 

#include "../mptt.h"
#include "../tslapi.h"
#include "t7.h"

t7_data_t t7_data_status;
ssint object_t7_init(u8 rid,  const /*sensor_config_t*/void *cfg, void *mem, void *cb)
{
	return object_txx_init(&t7_data_status, rid, cfg, mem, cb);
}

void t7_set_unsupport_area(object_t7_t *mem)
{
	mem->idleacqint = 0;
	mem->actv2idleto = 0;
}

void object_t7_process(void)
{
	t7_data_t *ptr = &t7_data_status;
	object_t7_t *mem = (object_t7_t *)ptr->mem;
	
	txx_cb_param_t params[] = {
		{ DEF_TOUCH_MEASUREMENT_PERIOD_MS, &mem->actvaqint, sizeof(mem->actvaqint) },
	};
	
	object_txx_process(ptr, params, ARRAY_SIZE(params), 0);
	
	t7_set_unsupport_area(mem);
}