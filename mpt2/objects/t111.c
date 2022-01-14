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
	u8 i;
#ifdef MPTT_MATRIX_NODES
	u8 j;
#endif

	for (i = 0; i < MXT_SPT_SELFCAPCONFIG_T111_INST; i++) {
		object_txx_init(&ptr[i].common, 0, def, (object_t111_t *)mem + i, cb);

		/* Placement buttons first in order:
			Buttons <0, 1, ...>
			Slider/Surfaces <0, 1, ...>
		*/
		if (i < qdef->num_button) {
			ptr[i].ns = &qdef->buttons[i].node;
			ptr[i].num_ns = 1;
		} 
#ifdef MPTT_MATRIX_NODES		
		else {
			j = i - qdef->num_button;
			if (j < qdef->num_surfaces_slider) {
				ptr[i].ns = qdef->surface_sliders[j].nodes;
				ptr[i].num_ns = NUM_NODE_2D;
			}
		}
#endif	
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
	const nodes_desc_t * const ns = (nodes_desc_t *)ptr->ns;
	u8 num_ns = ptr->num_ns;
	
	const qtouch_params_t *param = (qtouch_params_t *)&QTOUCH_CONFIG_VAL(ptr->common.def, params);
	object_t111_t *mem = (object_t111_t *)ptr->common.mem;

	u8 delay_y = mem->delaytime;	
	const u8 inrush_y = (mem->inrushcfg >> T111_INRUSHCFG_Y_RESISTOR_SHIFT) & T111_INRUSHCFG_RESISTOR_MASK;	
	nibble_t resprsc_y = {
		.lo =  mem->inttime > param->max_prsc_div ? param->max_prsc_div : mem->inttime,	//Low nibble for Inttime
		.hi =  inrush_y > param->max_resl ? param->max_resl : inrush_y	//Hi nibble for resistor
	};
	txx_cb_param_t yparams_channel[] = {
		{ API_NODE_PARAMS_RESISTOR_PRESCALER, &resprsc_y.value, sizeof(resprsc_y.value)},
		{ API_NODE_PARAMS_CSD, &delay_y, sizeof(delay_y)},
		{ API_NODE_PARAMS_ADC_OVERSAMPLING, &mem->actvsyncsperl, sizeof(mem->actvsyncsperl)},
	};
		
#ifdef MPTT_MATRIX_NODES
	u8 delay_x = mem->altdelaytimex ? mem->altdelaytimex : delay_y;
	const u8 inrush_x = (mem->inrushcfg >> T111_INRUSHCFG_X_RESISTOR_SHIFT) & T111_INRUSHCFG_RESISTOR_MASK;
	nibble_t resprsc_x = {
		.lo =  mem->altinttimex ? (mem->altinttimex > param->max_prsc_div ? param->max_prsc_div : mem->altinttimex) : resprsc_y.lo,
		.hi =  inrush_x > param->max_resl ? param->max_resl : inrush_x
	};
	txx_cb_param_t xparams_channel[] = {
		{ API_NODE_PARAMS_RESISTOR_PRESCALER, &resprsc_x.value, sizeof(resprsc_x.value)},
		{ API_NODE_PARAMS_CSD, &delay_x, sizeof(delay_x)},
		{ API_NODE_PARAMS_ADC_OVERSAMPLING, &mem->actvsyncsperl, sizeof(mem->actvsyncsperl)},
	};
#endif

#ifndef OBJECT_T15
	const txx_cb_param_t params[] = {
		{ API_DEF_TCH_DRIFT_RATE, &mem->drift, sizeof(mem->drift)},
		{ API_DEF_DRIFT_HOLD_TIME, &mem->driftst, sizeof(mem->driftst) }
	};
#endif
	u8 i, yid;
	uint8_t lumped_channel, lumped_channel_last;
	
	if (num_ns == NUM_NODE_2D) {
		yid = NODE_Y;
	} else {
		yid = 0;
	}

	//  Sensor channel parameter for Y channel
	lumped_channel_last = 0xff;
	for (i = ns[yid].origin; i < ns[yid].origin + ns[yid].size; i++) {	
		lumped_channel = QTOUCH_MAP_CALL(ptr->common.def, to_channel)(i, true);
		if (lumped_channel_last != lumped_channel) {
			object_txx_op(&ptr->common, yparams_channel, ARRAY_SIZE(yparams_channel), lumped_channel, rw);
			lumped_channel_last = lumped_channel;
		}
		
		if (rw == OP_READ)
			break;
	}

#ifdef MPTT_MATRIX_NODES
	// Sensor channel parameter for X channel
	if (num_ns == NUM_NODE_2D) {
		lumped_channel_last = 0xff;
		for (i = ns[NODE_X].origin; i < ns[NODE_X].origin + ns[NODE_X].size; i++) {
			lumped_channel = QTOUCH_MAP_CALL(ptr->common.def, to_channel)(i, true);
			if (lumped_channel_last != lumped_channel) {
				object_txx_op(&ptr->common, xparams_channel, ARRAY_SIZE(xparams_channel), lumped_channel, rw);
				lumped_channel_last = lumped_channel;
			}
			if (rw == OP_READ)
			break;
		}
	}
#endif
	// Common parameters
#ifndef OBJECT_T15
	object_txx_op(&ptr->common, params, ARRAY_SIZE(params), 0, rw);
#endif
	if (rw == OP_READ) {
		// Update memory
		mem->inttime = resprsc_y.lo;
		mem->delaytime = delay_y;
		mem->inrushcfg = (resprsc_y.hi << T111_INRUSHCFG_Y_RESISTOR_SHIFT);
#ifdef MPTT_MATRIX_NODES
		if (num_ns == NUM_NODE_2D) {
			mem->altinttimex = (resprsc_x.lo == resprsc_y.lo) ? 0 : resprsc_x.lo;
			mem->altdelaytimex = (delay_x == delay_y) ? 0 : delay_x;
			mem->inrushcfg |= (resprsc_x.hi << T111_INRUSHCFG_X_RESISTOR_SHIFT);
		}
#endif
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