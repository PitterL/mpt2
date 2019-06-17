/*
 * t104.c
 *
 * Created: 6/12/2019 2:16:55 PM
 *  Author: A41450
 */ 

#include "../tslapi.h"
#include "txx.h"

t104_data_t t104_data_status;
ssint object_t104_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	return object_txx_init(&t104_data_status, rid, def, mem, cb);
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

void t104_data_sync(const txx_data_t *ptr, u8 rw)
{
	object_t104_t *mem = (object_t104_t *)ptr->mem;
	u8 i, end;
	
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
	
	
	end = rw ? 1 : QTOUCH_CONFIG_VAL(ptr->def, matrix_xsize);
	for (i = 0; i < end; i++) {
		object_txx_op(ptr, xparams, ARRAY_SIZE(xparams), i, rw);
	}
	
	end = rw ? QTOUCH_CONFIG_VAL(ptr->def, matrix_xsize) + 1 : QTOUCH_CONFIG_VAL(ptr->def, matrix_xsize) + QTOUCH_CONFIG_VAL(ptr->def, matrix_ysize);
	for (i = QTOUCH_CONFIG_VAL(ptr->def, matrix_xsize); i < end; i++) {
		object_txx_op(ptr, yparams, ARRAY_SIZE(yparams), i, rw);
	}
	
	t104_set_unsupport_area(mem);
}

void object_t104_start(u8 loaded)
{
	t104_data_t *ptr = &t104_data_status;
	
	if (loaded)
		return;
	
	t104_data_sync(ptr, 1);
}

void object_t104_process(void)
{
	t104_data_t *ptr = &t104_data_status;
	
	t104_data_sync(ptr, 0);
}