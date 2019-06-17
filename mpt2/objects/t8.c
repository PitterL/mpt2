/*
 * t8.c
 *
 * Created: 6/9/2019 10:32:50 AM
 *  Author: A41450
 */ 

#include "../tslapi.h"
#include "txx.h"

t8_data_t t8_data_status;
ssint object_t8_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	return object_txx_init(&t8_data_status, rid, def, mem, cb);
}

void t8_set_unsupport_area(object_t8_t *mem)
{
	mem->chrgtime = 0;
	mem->sync = 0;
	mem->atchcalst = 0;
	mem->atchfrccalthr = 0;
	mem->atchfrccalratio = 0;
#ifdef OBJECT_T8_MC_SC
	mem->measallow = 0xf;
	mem->measidledef = 0x2;
	mem->measactvdef = 0x2;
	mem->refmode = 0x1;
	mem->cfg = 0;
#endif
}

void t8_data_sync(const txx_data_t *ptr, u8 rw)
{
	object_t8_t *mem = (object_t8_t *)ptr->mem;
	
	txx_cb_param_t params[] = {
		{ DEF_TCH_DRIFT_RATE, &mem->tchdrift, sizeof(mem->tchdrift) },
		{ DEF_ANTI_TCH_DRIFT_RATE, &mem->tchdrift, sizeof(mem->tchdrift) },
		{ DEF_DRIFT_HOLD_TIME, &mem->driftst, sizeof(mem->driftst) },
		{ DEF_MAX_ON_DURATION, &mem->tchautocal, sizeof(mem->tchautocal) },
		{ DEF_ANTI_TCH_RECAL_THRSHLD, &mem->atchcalsthr, sizeof(mem->atchcalsthr) },
	};
	
	object_txx_op(ptr, params, ARRAY_SIZE(params), 0, rw);
	
	t8_set_unsupport_area(mem);
}

void object_t8_start(u8 loaded)
{
	t8_data_t *ptr = &t8_data_status;
	
	if (loaded)
	return;
	
	t8_data_sync(ptr, 1);
}

void object_t8_process(void)
{
	t8_data_t *ptr = &t8_data_status;
	
	t8_data_sync(ptr, 0);
}