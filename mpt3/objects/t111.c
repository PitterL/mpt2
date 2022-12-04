/*
 * t111.c
 *
 * Created: 6/12/2019 2:16:35 PM
 *  Author: A41450
 */ 
#ifdef OBJECT_T111
#include "txx.h"

t111_data_t t111_data_status;
ssint object_t111_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	return object_txx_init(&t111_data_status, 0, def, mem, cb);
}

void t111_set_unsupport_area(object_t111_t *mem)
{

}

void t111_data_sync(const t111_data_t *ptr, u8 rw)
{
	const nodes_desc_t *ns = (nodes_desc_t *)QTOUCH_CONFIG_VAL(ptr->def, matrix_nodes);
	object_t111_t *mem = (object_t111_t *)ptr->mem;

#define NUM_CHANNLE_PARAMS 3
	txx_cb_param_t nodes_channel_params[NUM_NODE_2D][NUM_CHANNLE_PARAMS] = {
		{
#ifdef API_NODE_PARAMS_PRECHARGE
			{ API_NODE_PARAMS_PRECHARGE, &mem->delaytime, sizeof(mem->delaytime)},
#endif
#ifdef API_NODE_PARAMS_CSD
			{ API_NODE_PARAMS_CSD, &mem->inttime, sizeof(mem->inttime)},
#endif
#ifdef API_NODE_PARAMS_ADC_OVERSAMPLING
			{ API_NODE_PARAMS_ADC_OVERSAMPLING, &mem->actvsyncsperl, sizeof(mem->actvsyncsperl)},
#endif
		},
		{
#ifdef API_NODE_PARAMS_PRECHARGE
			{ API_NODE_PARAMS_PRECHARGE, &mem->altdelaytimex, sizeof(mem->altdelaytimex)},
#endif
#ifdef API_NODE_PARAMS_CSD
			{ API_NODE_PARAMS_CSD, &mem->altinttimex, sizeof(mem->altinttimex)},
#endif
#ifdef API_NODE_PARAMS_ADC_OVERSAMPLING
			{ API_NODE_PARAMS_ADC_OVERSAMPLING, &mem->idlesyncsperl, sizeof(mem->idlesyncsperl)},
#endif
		}
	};

#ifndef OBJECT_T15
	const txx_cb_param_t params[] = {
		{ API_DEF_TCH_DRIFT_RATE, &mem->drift, sizeof(mem->drift)},
		{ API_DEF_DRIFT_HOLD_TIME, &mem->driftst, sizeof(mem->driftst) }
	};
#endif
	u8 i, j;
	uint8_t channel_group, channel_group_last;
	
	//  Sensor channel parameter for Y channel
	for (i = NODE_X; i < NUM_NODE_2D; i++) {
		channel_group_last = 0xff;
		for (j = ns[i].origin; j < ns[i].origin + ns[i].size; i++) {	
			channel_group = QTOUCH_MAP_CALL(ptr->def, to_channel)(j, true);
			if (channel_group_last != channel_group) {
				object_txx_op(ptr, nodes_channel_params[i], ARRAY_SIZE(nodes_channel_params[i]), channel_group, rw);
				channel_group_last = channel_group;
			}
			
			if (rw == OP_READ)
				break;
		}
	}

	// Common parameters
#ifndef OBJECT_T15
	object_txx_op(ptr, params, ARRAY_SIZE(params), 0, rw);
#endif

	t111_set_unsupport_area(mem);
}

void object_t111_start(u8 loaded)
{
	t111_data_sync(&t111_data_status, loaded ? OP_WRITE : OP_READ);
}

void object_t111_data_sync(u8 rw)
{
	t111_data_sync(&t111_data_status, rw);
}

#endif