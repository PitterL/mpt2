/*
 * t104.c
 *
 * Created: 6/12/2019 2:16:55 PM
 *  Author: A41450
 */ 

#include "../mptt.h"
#include "../tslapi.h"
#include "t104.h"

t104_data_t t104_data_status;
ssint object_t104_init(u8 rid,  const /*sensor_config_t*/void *cfg, void *mem, void *cb)
{
	return object_txx_init(&t104_data_status, rid, cfg, mem, cb);
}

void t104_set_unsupport_area(object_t104_t *mem)
{
	// always enabled
	mem->ctrl |= MXT_T104_CTRL_ENABLE;
	mem->xintthr = 0;
	mem->xtchhyst = 0;
	mem->yintthr = 0;
	mem->ytchhyst = 0;	
}

void object_t104_process(void)
{
	t104_data_t *ptr = &t104_data_status;
	object_t104_t *mem = (object_t104_t *)ptr->mem;
	u8 i;
	
	txx_cb_param_t xparams[] = {
		{ NODE_PARAMS_GAIN, &mem->xgain, sizeof(mem->xgain) },
		{ KEY_PARAMS_THRESHOLD, &mem->xtchthr, sizeof(mem->xtchthr) },
		{ KEY_PARAMS_HYSTERESIS, &mem->xtchhyst, sizeof(mem->xtchhyst)}
	};
	
	txx_cb_param_t yparams[] = {
		{ NODE_PARAMS_GAIN, &mem->ygain, sizeof(mem->ygain) },
		{ KEY_PARAMS_THRESHOLD, &mem->ytchthr, sizeof(mem->ytchthr) },
		{ KEY_PARAMS_HYSTERESIS, &mem->ytchhyst, sizeof(mem->ytchhyst) },
	};
	
	for (i = 0; i < ptr->matrix_xsize; i++) {
		object_txx_process(ptr, xparams, ARRAY_SIZE(xparams), i);
	}
	
	for (; i < ptr->matrix_xsize + ptr->matrix_ysize; i++) {
		object_txx_process(ptr, yparams, ARRAY_SIZE(yparams), i);
	}
	
	t104_set_unsupport_area(mem);
}