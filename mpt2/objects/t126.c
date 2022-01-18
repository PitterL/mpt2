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
	
	txx_cb_param_t params_sensor[] = {
		{ API_DEF_TOUCH_DRIFT_PERIOD_MS, &mem->driftcoef, sizeof(mem->driftcoef) },
		{ API_DEF_QTM_AUTOSCAN_THRESHOLD, &mem->threshold, sizeof(mem->threshold) },
		{ API_DEF_QTM_AUTOSCAN_NODE, &mem->node, sizeof(mem->node) },
	};
	
	if ((mem->ctrl & MXT_T126_CTRL_ENABLE) || rw == OP_READ) {
		object_txx_op(&ptr->common, params_sensor, ARRAY_SIZE(params_sensor), 0, rw);
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

void object_t126_lowpower_status_change(wakeup_type_t type, int16_t val)
{
	t126_data_t *ptr =  &t126s_data_status;
	object_t126_t *mem = (object_t126_t *) ptr->common.mem;
	
	if (!(mem->ctrl & MXT_T126_CTRL_ENABLE)) {
		return;
	}
	
	if (type < NUM_WK_TYPES) {
		ptr->button.status = type;
		ptr->button.delta = val;
		
		if (mem->ctrl & MXT_T126_CTRL_RPTEN) {
			object_t126_report_status(false);
		}
	} else {
		ptr->button.status = MXT_T126_STATUS_IDLE;
		ptr->button.delta = 0;
	}
}

void object_api_t126_force_waked(int16_t val)
{
	object_t126_lowpower_status_change(WK_FORCE, val);
}

void object_api_t126_breach_waked(int16_t val)
{
	wakeup_type_t type = val > 0 ? WK_POS_BREACH : WK_RSV_NEG_BREACH;
	
	object_t126_lowpower_status_change(type, val);
}

void object_api_t126_breach_sleep(void)
{
	object_t126_lowpower_status_change(NUM_WK_TYPES, 0);
}

static bool is_lo_node(u8 node)
{
	t126_data_t *ptr =  &t126s_data_status;
	object_t126_t *mem = (object_t126_t *) ptr->common.mem;
	
#ifdef DEF_TOUCH_LOWPOWER_SOFT
	return (BIT(node) & mem->node);
#else
	return node == mem->node;
#endif
}

bool object_api_t126_node_skipped(u8 node)
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
		if (is_lo_node(node)) {
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
		if (is_lo_node(node)) {	
			if (!(mem->ctrl & MXT_T126_CTRL_DBGEN))
				ret = true;
		}
	}
	
	return ret;
}

ssint object_api_t126_is_low_power_node(u8 node)
{
	t126_data_t *ptr =  &t126s_data_status;
	object_t126_t *mem = (object_t126_t *) ptr->common.mem;
	ssint ret = -1;
	
	if (!(mem->ctrl & MXT_T126_CTRL_ENABLE)) {
		return ret;
	}

	return is_lo_node(node);
}

bool object_api_t126_get_low_power_status(void)
{
	t126_data_t *ptr =  &t126s_data_status;
	
	return !!(ptr->button.status & MXT_T126_STATUS_IDLE);
}
#endif