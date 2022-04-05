/*
 * t126.c
 *
 * Created: 4/19/2021 03:41:00 PM
 *  Author: A41450
 */ 
#ifdef OBJECT_T126

#include <string.h>
#include "arch/tslapi.h"
#include "txx.h"

t126_data_t t126s_data_status;
ssint object_t126_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	t126_data_t *ptr = &t126s_data_status;

	object_txx_init(&ptr->common, rid, def, (object_t126_t *)mem, cb);

	return 0;
}

void t126_set_unsupport_area(t126_data_t *ptr)
{
	/* object_t126_t *mem = (object_t126_t *) ptr->common.mem; */
}

void t126_data_sync(t126_data_t *ptr, u8 rw)
{
	object_t126_t *mem = (object_t126_t *) ptr->common.mem;
#ifndef DEF_TOUCH_LOWPOWER_SOFT
	u16 tunparam;
#endif
	txx_cb_param_t params_sensor[] = {
		{ API_DEF_TOUCH_DRIFT_PERIOD_MS, &mem->driftcoef, sizeof(mem->driftcoef) },
		{ API_DEF_QTM_AUTOSCAN_THRESHOLD, &mem->threshold, sizeof(mem->threshold) },
        { API_DEF_QTM_AUTOSCAN_DRIFT_TARGET_NODE, &mem->driftnode, sizeof(mem->driftnode) },
#ifdef DEF_TOUCH_LOWPOWER_SOFT
		{ API_DEF_QTM_AUTOSCAN_NODE, &mem->node, sizeof(mem->node) },
#else
        { API_DEF_QTM_AUTOSCAN_TUNING_PARAM, &tunparam, sizeof(tunparam) },
        { API_DEF_QTM_AUTOSCAN_GAIN, &mem->gain, sizeof(mem->gain)},
    	{ API_DEF_QTM_AUTOSCAN_RESISTOR_PRESCALER, &mem->respres, sizeof(mem->respres)},
    	{ API_DEF_QTM_AUTOSCAN_CSD, &mem->csd, sizeof(mem->csd)},
    	{ API_DEF_QTM_AUTOSCAN_ADC_OVERSAMPLING, &mem->syncsperl, sizeof(mem->syncsperl)},
#endif
    };
	
    if (rw == OP_WRITE) {
#ifndef DEF_TOUCH_LOWPOWER_SOFT
        tunparam = mem->tunparam[0] | (mem->tunparam[1] << 8);
#endif
    }

	if ((mem->ctrl & MXT_T126_CTRL_ENABLE) || rw == OP_READ) {
		object_txx_op(&ptr->common, params_sensor, ARRAY_SIZE(params_sensor), 0, rw);
	}

    if (rw == OP_READ) {
#ifndef DEF_TOUCH_LOWPOWER_SOFT
        mem->tunparam[0] = tunparam & 0xFF;
        mem->tunparam[1] = (tunparam >> 8) & 0xFF;
#endif
    }
	
	t126_set_unsupport_area(ptr);
}

void object_t126_start(u8 loaded)
{
	object_t126_data_sync(loaded ? OP_WRITE : OP_READ);
}

void object_t126_data_sync(u8 rw)
{
	t126_data_sync(&t126s_data_status, rw);	
}

void object_t126_report_status(u8 force)
{
	t126_data_t *ptr = &t126s_data_status;
	
	if (force) {
		ptr->button.status &= ~WK_TYPE_MASK;
		ptr->button.status = WK_REPORT_ALL;
	}
	
	object_txx_report_msg(&ptr->common, &ptr->button, sizeof(ptr->button));
}

bool object_api_t126_lowpower_mode_enabled(void)
{
	t126_data_t *ptr =  &t126s_data_status;
	object_t126_t *mem = (object_t126_t *) ptr->common.mem;
	
	return (mem->ctrl & MXT_T126_CTRL_ENABLE);
}

void object_t126_lowpower_status_change(wakeup_type_t type, u16 signal)
{
	t126_data_t *ptr =  &t126s_data_status;
	object_t126_t *mem = (object_t126_t *) ptr->common.mem;
	
	if (!(mem->ctrl & MXT_T126_CTRL_ENABLE)) {
		return;
	}
	
    memset(&ptr->button.info, 0, sizeof(ptr->button.info));

	if (type < NUM_WK_TYPES) {
		ptr->button.status = type;
        ptr->button.info.signal = signal;

		if (mem->ctrl & MXT_T126_CTRL_RPTEN) {
			object_t126_report_status(false);
		}
	} else {
		ptr->button.status = MXT_T126_STATUS_IDLE;
	}
}

void object_api_t126_force_waked(void)
{
	object_t126_lowpower_status_change(WK_FORCE, 0);
}

void object_api_t126_breach_waked(u16 signal)
{	
    wakeup_type_t type = WK_FORCE;

	object_t126_lowpower_status_change(type, signal);
}

void object_api_t126_breach_sleep(void)
{
	object_t126_lowpower_status_change(NUM_WK_TYPES, 0);
}

static bool is_lo_node(u8 sensor_node)
{
#ifdef DEF_TOUCH_LOWPOWER_SOFT
	t126_data_t *ptr =  &t126s_data_status;
	object_t126_t *mem = (object_t126_t *) ptr->common.mem;
	
	return (BIT(sensor_node) & mem->node);
#else
    return false;
#endif
}

bool object_api_t126_node_skipped(u8 sensor_node)
{
	t126_data_t *ptr =  &t126s_data_status;
	object_t126_t *mem = (object_t126_t *) ptr->common.mem;
	bool ret = false;
	
	// Function not enabled
	if (!(mem->ctrl & MXT_T126_CTRL_ENABLE)) {
		return ret;
	}

	// In idle mode
	if (ptr->button.status & MXT_T126_STATUS_IDLE) {
		if (is_lo_node(sensor_node)) {
			if (!(mem->ctrl & MXT_T126_CTRL_RPTTCHEN)) {
				ret = true;
			}
		} else {
			if (!(mem->ctrl & MXT_T126_CTRL_RPTAUTOEN)) {
				ret = true;	
			}
		}
	// In active mode
	} else {
		if (is_lo_node(sensor_node)) {	
			if (!(mem->ctrl & MXT_T126_CTRL_DBGEN))
				ret = true;
		}
	}
	
	return ret;
}

ssint object_api_t126_is_low_power_node(u8 sensor_node)
{
	t126_data_t *ptr =  &t126s_data_status;
	object_t126_t *mem = (object_t126_t *) ptr->common.mem;
	ssint ret = -1;
	
	if (!(mem->ctrl & MXT_T126_CTRL_ENABLE)) {
		return ret;
	}

	return is_lo_node(sensor_node);
}

bool object_api_t126_get_low_power_status(void)
{
	t126_data_t *ptr =  &t126s_data_status;
	
	return !!(ptr->button.status & MXT_T126_STATUS_IDLE);
}
#endif