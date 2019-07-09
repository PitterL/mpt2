/*
 * t104.c
 *
 * Created: 6/12/2019 2:16:55 PM
 *  Author: A41450
 */ 
#ifdef OBJECT_T104

#include "../tslapi.h"
#include "txx.h"

t104_data_t t104s_data_status[MXT_SPT_AUXTOUCHCONFIG_T104_INST];
ssint object_t104_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	t104_data_t *ptr = &t104s_data_status[0];
	qtouch_config_t *qdef = (qtouch_config_t *)def;
	u8 i;

	for (i = 0; i < MXT_SPT_AUXTOUCHCONFIG_T104_INST; i++) {
		object_txx_init(&ptr[i].common, 0, def, (object_t104_t *)mem + i, cb);

		if (i < qdef->num_surfaces_slider) {
			ptr->ns = qdef->surface_sliders[/*qdef->num_slider + */i].nodes;
		}else {
			ptr->ns = qdef->matrix_nodes;
		}
	}
	
	return 0;
}

void t104_set_unsupport_area(object_t104_t *mem)
{
	// always enabled
	mem->ctrl |= MXT_T104_CTRL_ENABLE;
#ifdef OBJECT_WRITEBACK
	mem->xintthr = 0;
	mem->xinthyst = 0;
	mem->yintthr = 0;
	mem->yinthyst = 0;
#endif
}

void t104_data_sync(const t104_data_t *ptr, u8 rw)
{
	const nodes_desc_t *ns = (nodes_desc_t *)ptr->ns;
	object_t104_t *mem = (object_t104_t *)ptr->common.mem;

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
	u8 i;
	
	// Sensor channel parameter for X channel
	for (i = ns[NODE_X].origin; i < ns[NODE_X].origin + ns[NODE_X].size; i++) {
		object_txx_op(&ptr->common, xparams, ARRAY_SIZE(xparams), i, rw);
		if (rw == OP_READ)
			break;
	}
	
	//  Sensor channel parameter for Y channel
	for (i = ns[NODE_Y].origin; i < ns[NODE_Y].origin + ns[NODE_Y].size; i++) {
		object_txx_op(&ptr->common, yparams, ARRAY_SIZE(yparams), i, rw);
		if (rw == OP_READ)
			break;
	}
	
	t104_set_unsupport_area(mem);
}

void object_t104_start(u8 loaded)
{
	t104_data_t *ptr = &t104s_data_status[0];
	u8 i;
	
	if (loaded)
		return;
	
	for (i = 0; i < MXT_SPT_AUXTOUCHCONFIG_T104_INST; i++) {
		t104_data_sync(&ptr[i], OP_READ);
	}
}

void object_t104_process(u8 rw)
{
	t104_data_t *ptr = &t104s_data_status[0];
	u8 i;
	
	for (i = 0; i < MXT_SPT_AUXTOUCHCONFIG_T104_INST; i++) {
		t104_data_sync(&ptr[i], rw);
	}
}

#endif