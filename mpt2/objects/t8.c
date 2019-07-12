/*
 * t8.c
 *
 * Created: 6/9/2019 10:32:50 AM
 *  Author: A41450
 */ 
#ifdef OBJECT_T8

#include "../tslapi.h"
#include "../include/touch_api_ptc.h"
#include "txx.h"

t8_data_t t8_data_status;
ssint object_t8_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	return object_txx_init(&t8_data_status, rid, def, mem, cb);
}

void t8_set_unsupport_area(object_t8_t *mem)
{
	//mem->chrgtime = 0;
	mem->sync = 0;
	mem->atchcalst = 0;
	mem->atchfrccalthr = 0;
	mem->atchfrccalratio = 0;
	mem->measallow = MXT_T8_MEASALLOW_ALLOWED;
	mem->measidledef = 0;
	//mem->measactvdef = MXT_T8_MEASALLOW_SELFTCH;
	//mem->refmode = 0;
	mem->cfg = 0;
}

void t8_readback_sensing_mode(const txx_data_t *ptr)
{
	object_t8_t *mem = (object_t8_t *)ptr->mem;
	u8 sensortype = 0;
	txx_cb_param_t param = { DEF_SENSOR_TYPE, &sensortype, sizeof(sensortype) };
	
	object_txx_op(ptr, &param, 1, 0, 1);
	
	switch (sensortype) {
		case NODE_SELFCAP:
			mem->measactvdef = MXT_T8_MEASALLOW_SELFPROX;
		case NODE_SELFCAP_SHIELD:
			mem->measactvdef = MXT_T8_MEASALLOW_SELFTCH;
		break;
		case NODE_MUTUAL:
			mem->measactvdef = MXT_T8_MEASALLOW_MUTUALTCH;
			break;
		case NODE_MUTUAL_4P:
			mem->measactvdef = MXT_T8_MEASALLOW_MUTUALTCH_4P;
			break;
		case NODE_MUTUAL_8P:
			mem->measactvdef = MXT_T8_MEASALLOW_MUTUALTCH_8P;
		default:
			;
	}
}

void t8_writeback_sensing_mode(const txx_data_t *ptr)
{
	object_t8_t *mem = (object_t8_t *)ptr->mem;
	u8 sensortype = NODE_MUTUAL;
	txx_cb_param_t param = { DEF_SENSOR_TYPE, &sensortype, sizeof(sensortype) };
	
	if (mem->measactvdef & MXT_T8_MEASALLOW_SELFTCH) {
		sensortype = NODE_SELFCAP_SHIELD;
	} else if(mem->measactvdef & MXT_T8_MEASALLOW_MUTUALTCH) {
		if (mem->measactvdef == MXT_T8_MEASALLOW_MUTUALTCH_8P) {
			sensortype = NODE_MUTUAL_8P;
		}else if (mem->measactvdef == MXT_T8_MEASALLOW_MUTUALTCH_4P) {
			sensortype = NODE_MUTUAL_4P;
		}else {
			sensortype = NODE_MUTUAL;
		}
	} else if(mem->measactvdef & MXT_T8_MEASALLOW_SELFPROX) {
		sensortype = NODE_SELFCAP;
	}
	
	object_txx_op(ptr, &param, 1, 0, 0);
}

/* Don't call this function before T8 started */
/*
	Mutual cap: return 0
	Selfcap: return 1
*/
u8 object_t8_get_measure_mode(u8 *meas) 
{
	t8_data_t *ptr = &t8_data_status;
	object_t8_t *mem = (object_t8_t *)ptr->mem;
	
	if (meas) {
		*meas = mem->measactvdef;
	}
	
	if(mem->measactvdef & MXT_T8_MEASALLOW_MUTUALTCH)
		return 0;
	else
		return 1;
}

u8 object_api_t8_measuring_self(void) 
{
	return object_t8_get_measure_mode(NULL);
}

u8 object_api_t8_measuring_mutual(void) 
{
	return !object_t8_get_measure_mode(NULL);
}

void t8_data_sync(const txx_data_t *ptr, u8 rw)
{
	object_t8_t *mem = (object_t8_t *)ptr->mem;

	txx_cb_param_t ct_params[] = {
		{ NODE_PARAMS_CSD, &mem->chrgtime, sizeof(mem->chrgtime)},	//Compared to T111 Intdelay
		{ NODE_PARAMS_RESISTOR_PRESCALER, &mem->refmode, sizeof(mem->refmode)},	//Compared to T111 Inttime and Resistor
	};
	u8 i;
	
	txx_cb_param_t params[] = {
		{ DEF_TCH_DRIFT_RATE, &mem->tchdrift, sizeof(mem->tchdrift) },
		{ DEF_ANTI_TCH_DRIFT_RATE, &mem->atchdrift, sizeof(mem->atchdrift) },	//Use rsv for anti touch drift
		{ DEF_DRIFT_HOLD_TIME, &mem->driftst, sizeof(mem->driftst) },
		{ DEF_MAX_ON_DURATION, &mem->tchautocal, sizeof(mem->tchautocal) },
		{ DEF_ANTI_TCH_RECAL_THRSHLD, &mem->atchcalsthr, sizeof(mem->atchcalsthr) },
	};
	
	for (i = 0; i < QTOUCH_CONFIG_VAL(ptr->def, maxtrix_channel_count); i++) {
		object_txx_op(ptr, ct_params, ARRAY_SIZE(ct_params), i, rw);
		if (rw == OP_READ)
			break;
		else
			t8_set_unsupport_area(mem);
	}
	
	object_txx_op(ptr, params, ARRAY_SIZE(params), 0, rw);
	
	t8_set_unsupport_area(mem);
}

void object_t8_start(u8 loaded)
{
	t8_data_t *ptr = &t8_data_status;
	
	if (loaded)
		return;
	
	t8_readback_sensing_mode(ptr);
	t8_data_sync(ptr, OP_READ);
}

void object_t8_data_sync(u8 rw)
{
	t8_data_t *ptr = &t8_data_status;
	
	t8_writeback_sensing_mode(ptr);
	t8_data_sync(ptr, rw);
}

#endif