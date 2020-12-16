/*
 * t111.c
 *
 * Created: 6/12/2019 2:16:35 PM
 *  Author: A41450
 */ 
#ifdef OBJECT_T111

#include "arch/tslapi.h"
#include "txx.h"

t111_data_t t111s_data_status[MXT_SPT_SELFCAPCONFIG_T111_INST];
ssint object_t111_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
#ifndef OBJECT_T111_DUMMY
	t111_data_t *ptr = &t111s_data_status[0];
	qtouch_config_t *qdef = (qtouch_config_t *)def;
	u8 i, tid;

	for (i = 0; i < MXT_SPT_SELFCAPCONFIG_T111_INST; i++) {
		object_txx_init(&ptr[i].common, 0, def, (object_t111_t *)mem + i, cb);
		if (i < qdef->num_surfaces_slider) {
			tid = qdef->num_surfaces ? i + qdef->num_slider: i;
			ptr[i].ns = qdef->surface_sliders[tid].nodes;
		} else {
			ptr[i].ns = qdef->matrix_nodes;
		}
	}
#endif
	return 0;
}

void t111_set_unsupport_area(object_t111_t *mem)
{
#ifndef OBJECT_WRITEBACK
	mem->ctrl = 0;
	mem->dbgctrl = 0;
	mem->idlesyncsperl = 0;
	mem->dccalrecstr = 0;
	mem->dcdrift = 0;
	mem->dcfilter = 0;
	mem->dccalrecstr = 0;
	mem->dccalerrratio = 0;
	mem->dcgainsf = 0;
#endif
}

void t111_data_sync(const t111_data_t *ptr, u8 rw)
{
#ifndef MPTT_MATRIX_NODES
	const nodes_desc_t *ns = (nodes_desc_t *)ptr->ns;
#endif
	const qtouch_params_t *param = (qtouch_params_t *)&QTOUCH_CONFIG_VAL(ptr->common.def, params);
	object_t111_t *mem = (object_t111_t *)ptr->common.mem;
	
	const u8 inrush_y = (mem->inrushcfg >> T111_INRUSHCFG_Y_RESISTOR_SHIFT) & T111_INRUSHCFG_RESISTOR_MASK;
	const u8 inrush_x = (mem->inrushcfg >> T111_INRUSHCFG_X_RESISTOR_SHIFT) & T111_INRUSHCFG_RESISTOR_MASK;
	
	nibble_t resprsc_y = {
		.lo =  mem->inttime > param->max_prsc_div ? param->max_prsc_div : mem->inttime,	//Low nibble for Inttime
		.hi =  inrush_y > param->max_resl ? param->max_resl : inrush_y	//Hi nibble for resistor
	};
	
	nibble_t resprsc_x = {
		.lo =  mem->altinttimex ? (mem->altinttimex > param->max_prsc_div ? param->max_prsc_div : mem->altinttimex) : resprsc_y.lo,
		.hi =  inrush_x > param->max_resl ? param->max_resl : inrush_x
	};
	
	u8 delay_y = mem->delaytime;
	u8 delay_x = mem->altdelaytimex ? mem->altdelaytimex : delay_y;
	
	txx_cb_param_t yparams[] = {
		{ API_NODE_PARAMS_RESISTOR_PRESCALER, &resprsc_y.value, sizeof(resprsc_y.value)},
		{ API_NODE_PARAMS_CSD, &delay_y, sizeof(delay_y)},
		{ API_NODE_PARAMS_ADC_OVERSAMPLING, &mem->actvsyncsperl, sizeof(mem->actvsyncsperl)},
	};

#ifndef MPTT_MATRIX_NODES	
	txx_cb_param_t xparams[] = {
		{ API_NODE_PARAMS_RESISTOR_PRESCALER, &resprsc_x.value, sizeof(resprsc_x.value)},
		{ API_NODE_PARAMS_CSD, &delay_x, sizeof(delay_x)},
		{ API_NODE_PARAMS_ADC_OVERSAMPLING, &mem->actvsyncsperl, sizeof(mem->actvsyncsperl)},
	};
#endif

	const txx_cb_param_t params[] = {
		{ API_DEF_TCH_DRIFT_RATE, &mem->drift, sizeof(mem->drift)},
		{ API_DEF_DRIFT_HOLD_TIME, &mem->driftst, sizeof(mem->driftst) }
	};
	
	u8 i;

#ifdef MPTT_MATRIX_NODES
	for (i = 0; i < QTOUCH_CONFIG_VAL(ptr->common.def, maxtrix_channel_count); i++) {
		object_txx_op(&ptr->common, yparams, ARRAY_SIZE(yparams), i, rw);
			if (rw == OP_READ)
				break;
	}
#else
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
#endif
	// Common parameters
	object_txx_op(&ptr->common, params, ARRAY_SIZE(params), 0, rw);
	
	if (rw == OP_READ) {
		// Update memory
		mem->inttime = resprsc_y.lo;
		mem->altinttimex = (resprsc_x.lo == resprsc_y.lo) ? 0 : resprsc_x.lo;
		mem->inrushcfg = (resprsc_y.hi << T111_INRUSHCFG_Y_RESISTOR_SHIFT) | (resprsc_x.hi << T111_INRUSHCFG_X_RESISTOR_SHIFT);
		mem->delaytime = delay_y;
		mem->altdelaytimex = (delay_x == delay_y) ? 0 : delay_x;
	}
	
	t111_set_unsupport_area(mem);
}

void object_t111_start(u8 loaded)
{
#ifndef OBJECT_T111_DUMMY
	t111_data_t *ptr = &t111s_data_status[0];
	u8 i;
		
	for (i = 0; i < MXT_SPT_SELFCAPCONFIG_T111_INST; i++) {
		t111_data_sync(ptr + i, loaded ? OP_WRITE : OP_READ);
	}
#endif
}

void object_t111_data_sync(u8 rw)
{
#ifndef OBJECT_T111_DUMMY
	t111_data_t *ptr = &t111s_data_status[0];
	u8 i;

	for (i = 0; i < MXT_SPT_SELFCAPCONFIG_T111_INST; i++) {
		t111_data_sync(ptr + i, rw);
	}
#endif
}

#endif