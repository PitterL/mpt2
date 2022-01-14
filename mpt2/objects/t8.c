/*
 * t8.c
 *
 * Created: 6/9/2019 10:32:50 AM
 *  Author: A41450
 */ 
#ifdef OBJECT_T8

#include "arch/tslapi.h"
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
/*
	mem->measallow = MXT_T8_MEASALLOW_ALLOWED;
	mem->meastestdef = 0;
	mem->measactvdef = MXT_T8_MEASALLOW_SELFTCH;
	mem->refmode = 0;
*/
	mem->cfg = 0;
}

#ifdef OBJECT_T8_RSD_SWITCH
u8 object_api_t8_ref_mode(void)
{
	t8_data_t *ptr = &t8_data_status;
	object_t8_t *mem = (object_t8_t *)ptr->mem;
		
	return mem->refmode;
}
#endif

void t8_data_sync(const txx_data_t *ptr, u8 rw)
{
	const txx_data_t * cm = ptr;
	object_t8_t *mem = (object_t8_t *)ptr->mem;
    u8 sensortype;

	txx_cb_param_t params_channel[] = {
		{ API_NODE_PARAMS_CSD, &mem->chrgtime, sizeof(mem->chrgtime)},	//Compared to T111 Intdelay
	};
	u8 i;
	uint8_t lumped_channel, lumped_channel_last;
	
	txx_cb_param_t params[] = {
        { API_DEF_SENSOR_TYPE, &sensortype, sizeof(sensortype) },
		{ API_DEF_TCH_DRIFT_RATE, &mem->tchdrift, sizeof(mem->tchdrift) },
		{ API_DEF_ANTI_TCH_DRIFT_RATE, &mem->atchdrift, sizeof(mem->atchdrift) },	//Use rsv for anti touch drift
		{ API_DEF_DRIFT_HOLD_TIME, &mem->driftst, sizeof(mem->driftst) },
		{ API_DEF_MAX_ON_DURATION, &mem->tchautocal, sizeof(mem->tchautocal) },
		{ API_DEF_ANTI_TCH_RECAL_THRSHLD, &mem->atchcalsthr, sizeof(mem->atchcalsthr) },
	};
	
	if (rw == OP_WRITE) {
		sensortype = tsapi_t8_sensing_mode_translate(mem->measallow, rw);
	}

	//T8 always write all channels
	lumped_channel_last = 0xff;
	for (i = 0; i < QTOUCH_CONFIG_VAL(cm->def, sensor_count); i++) {
		lumped_channel = QTOUCH_MAP_CALL(cm->def, to_channel)(i, true);
		if (lumped_channel_last != lumped_channel) {
			object_txx_op(cm, params_channel, ARRAY_SIZE(params_channel), lumped_channel, rw);
			lumped_channel_last = lumped_channel;
		}
		if (rw == OP_READ)
			break;
	}
	
	object_txx_op(ptr, params, ARRAY_SIZE(params), 0, rw);
	
    if (rw == OP_READ) {
        mem->measallow = tsapi_t8_sensing_mode_translate(sensortype, rw);
	}

	t8_set_unsupport_area(mem);
}

void object_t8_start(u8 loaded)
{
	t8_data_t *ptr = &t8_data_status;

	t8_data_sync(ptr, loaded ? OP_WRITE : OP_READ);
}

void object_t8_data_sync(u8 rw)
{
	t8_data_t *ptr = &t8_data_status;
	
	t8_data_sync(ptr, rw);
}

ssint object_t8_switch_measure_mode(u8 test)
{
	t8_data_t *ptr = &t8_data_status;
	object_t8_t *mem = (object_t8_t *)ptr->mem;
	u8 measallow = test ? mem->meastestdef : mem->measactvdef;

	if (measallow && measallow != mem->measallow) {
		mem->measallow = measallow;
		t8_data_sync(ptr, OP_WRITE);
		
		return 0;
	}
	
	return -1;
}

#endif