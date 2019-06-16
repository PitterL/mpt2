/*
 * t111.c
 *
 * Created: 6/12/2019 2:16:35 PM
 *  Author: A41450
 */ 

#include "../tslapi.h"
#include "../mptt.h"
#include "t111.h"

t111_data_t t111_data_status;
ssint object_t111_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	return object_txx_init(&t111_data_status, rid, def, mem, cb);
}

void t111_set_unsupport_area(object_t111_t *mem)
{
	mem->ctrl = 0;
	mem->dbgctrl = 0;
	mem->idlesyncsperl = 0;
	mem->dccalrecstr = 0;
	mem->dcdrift = 0;
	mem->dcfilter = 0;
	mem->dccalrecstr = 0;
	mem->dccalerrratio = 0;
	mem->dcgainsf = 0;
}

void object_t111_process(void)
{
	t111_data_t *ptr = &t111_data_status;
	object_t111_t *mem = (object_t111_t *)ptr->mem;
	nibble_t resprsc_y = {
		.lo =  mem->inttime & 0xF,
		.hi =  (mem->inrushcfg >> T111_INRUSHCFG_Y_RESISTOR_SHIFT) & T111_INRUSHCFG_RESISTOR_MASK
	};
	
	nibble_t resprsc_x = {
		.lo =  mem->altinttimex ? (mem->altinttimex & 0xF) : (mem->inttime & 0xF),
		.hi =  (mem->inrushcfg >> T111_INRUSHCFG_X_RESISTOR_SHIFT) & T111_INRUSHCFG_RESISTOR_MASK
	};
	
	u8 delay_y = mem->delaytime;
	u8 delay_x = mem->altdelaytimex ? mem->altdelaytimex : mem->delaytime;
	
	txx_cb_param_t yparams[] = {
		{ NODE_PARAMS_RESISTOR_PRESCALER, &resprsc_y.value, sizeof(resprsc_y.value)},
		{ NODE_PARAMS_CSD, &delay_y, sizeof(delay_y)},
		{ NODE_PARAMS_ADC_OVERSAMPLING, &mem->actvsyncsperl, sizeof(mem->actvsyncsperl)},	
	};
	
	txx_cb_param_t xparams[] = {
		{ NODE_PARAMS_RESISTOR_PRESCALER, &resprsc_x.value, sizeof(resprsc_x.value)},
		{ NODE_PARAMS_CSD, &delay_x, sizeof(delay_x)},
		{ NODE_PARAMS_ADC_OVERSAMPLING, &mem->actvsyncsperl, sizeof(mem->actvsyncsperl)},
	};
	
	const txx_cb_param_t params[] = {
		{ DEF_TCH_DRIFT_RATE, &mem->drift, sizeof(mem->drift)},
		{ DEF_DRIFT_HOLD_TIME, &mem->driftst, sizeof(mem->driftst) }
	};
	
	u8 i;
		
	for (i = 0; i < QTOUCH_CONFIG_VAL(ptr->def, matrix_xsize); i++) {
		object_txx_process(ptr, xparams, ARRAY_SIZE(xparams), i);
	}
	
	for (; i < QTOUCH_CONFIG_VAL(ptr->def, matrix_xsize) + QTOUCH_CONFIG_VAL(ptr->def, matrix_ysize); i++) {
		object_txx_process(ptr, yparams, ARRAY_SIZE(yparams), i);
	}
	
	object_txx_process(ptr, params, ARRAY_SIZE(params), 0);

	t111_set_unsupport_area(mem);
}