/*
 * t104.c
 *
 * Created: 6/12/2019 2:16:55 PM
 *  Author: A41450
 */ 
#ifdef OBJECT_T104

#include "txx.h"

t104_data_t t104_data_status;
ssint object_t104_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	return object_txx_init(&t104_data_status, 0, def, mem, cb);
}

void t104_set_unsupport_area(object_t104_t *mem)
{
#ifndef OBJECT_WRITEBACK
	mem->ctrl |= MXT_T104_CTRL_ENABLE;
#endif

	mem->xintthr = 0;
	mem->xinthyst = 0;
	mem->yintthr = 0;
	mem->yinthyst = 0;
}

void t104_data_sync(const t104_data_t *ptr, u8 rw)
{
	object_t104_t *mem = (object_t104_t *)ptr->mem;
	const nodes_desc_t *ns = (nodes_desc_t *)QTOUCH_CONFIG_VAL(ptr->def, matrix_nodes);

	txx_cb_param_t xparams_sensor[] = {
		{ API_KEY_PARAMS_THRESHOLD, &mem->xtchthr, sizeof(mem->xtchthr) },
		{ API_KEY_PARAMS_HYSTERESIS, &mem->xtchhyst, sizeof(mem->xtchhyst)}
	};
	
	txx_cb_param_t xparams_channel[] = {
		{ API_NODE_PARAMS_GAIN, &mem->xgain, sizeof(mem->xgain) },
	};

	txx_cb_param_t yparams_sensor[] = {
		{ API_KEY_PARAMS_THRESHOLD, &mem->ytchthr, sizeof(mem->ytchthr) },
		{ API_KEY_PARAMS_HYSTERESIS, &mem->ytchhyst, sizeof(mem->ytchhyst) },
	};
	txx_cb_param_t yparams_channel[] = {
		{ API_NODE_PARAMS_GAIN, &mem->ygain, sizeof(mem->ygain) },
	};	
	uint8_t channel_group, channel_group_last, lumped_channel_last;
	u8 i;

	if (!(mem->ctrl & MXT_T104_CTRL_ENABLE))	// Not enabled is readonly mode
		if  (rw != OP_READ)
			return;

	// Sensor channel parameter for X channel
	channel_group_last = 0xff;
	for (i = ns[NODE_X].origin; i < ns[NODE_X].origin + ns[NODE_X].size; i++) {
		channel_group = QTOUCH_MAP_CALL(ptr->def, to_channel)(i, true);
		if (channel_group_last != channel_group) {
			object_txx_op(ptr, xparams_channel, ARRAY_SIZE(xparams_channel), channel_group, rw);
			lumped_channel_last = channel_group;
		}	
		if (rw == OP_READ) {
			object_txx_op(ptr, xparams_sensor, ARRAY_SIZE(xparams_sensor), i, rw);
			break;
		}
	}
	
	//  Sensor channel parameter for Y channel
	channel_group_last = 0xff;
	for (i = ns[NODE_Y].origin; i < ns[NODE_Y].origin + ns[NODE_Y].size; i++) {
		channel_group = QTOUCH_MAP_CALL(ptr->def, to_channel)(i, true);
		if (lumped_channel_last != channel_group) {
			object_txx_op(ptr, yparams_channel, ARRAY_SIZE(yparams_channel), channel_group, rw);
			lumped_channel_last = channel_group;
		}
		if (rw == OP_READ) {
			object_txx_op(ptr, yparams_sensor, ARRAY_SIZE(yparams_sensor), i, rw);
			break;
		}
	}

	t104_set_unsupport_area(mem);
}

void object_t104_start(u8 loaded)
{
	t104_data_t *ptr = &t104_data_status;

	t104_data_sync(ptr, loaded ? OP_WRITE : OP_READ);
}

void object_t104_data_sync(u8 rw)
{
	t104_data_t *ptr = &t104_data_status;

	t104_data_sync(ptr, rw);
}

#endif