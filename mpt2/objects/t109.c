/*
 * t109.c
 *
 * Created: 6/16/2019 2:48:07 AM
 *  Author: A41450
 */ 
#ifdef OBJECT_T109

#include <string.h>
#include "arch/tslapi.h"
#include "txx.h"

t109_data_t t109_data_status;
ssint object_t109_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	t109_data_t *t109_ptr = &t109_data_status;

	object_txx_init(&t109_ptr->common, rid, def, mem, cb);
	
	return 0;
}

void t109_report_status(t109_data_t *ptr)
{
	object_t109_t *mem = (object_t109_t *)ptr->common.mem;
		
	if (!(mem->ctrl & MXT_T109_CMD_RPTEN))
		return;
	
	if (!ptr->status.result.cmd)
		return;
	
	object_txx_report_msg(&ptr->common, &ptr->status.result, sizeof(ptr->status.result));
	
	memset(&ptr->status.result, 0, sizeof(ptr->status.result));
}

void t109_data_sync(t109_data_t *ptr, u8 rw)
{
	u16 compcap;
	u8 i;
	
	txx_cb_param_t params_sensor[] = {
		{ API_NODE_COMPCAP_VALUE, &compcap, sizeof(compcap) },
	};
		
	// Sensor channel parameter
	// Fix me: if mutual cap node by node mode, the node_comp_caps may not enough?
	for (i = 0; i < QTOUCH_CONFIG_VAL(ptr->common.def, sensor_count) && i < ARRAY_SIZE(ptr->status.node_comp_caps); i++) {
		compcap = ptr->status.node_comp_caps[i];
		object_txx_op(&ptr->common, params_sensor, ARRAY_SIZE(params_sensor), i, rw);
		if (rw == OP_READ) {
			ptr->status.node_comp_caps[i] = compcap;
		}
	}
}

void object_t109_start(u8 loaded)
{
	t109_data_t *ptr = &t109_data_status;
	
	t109_data_sync(ptr, loaded? OP_WRITE : OP_READ);
}

void object_t109_report_status(u8 force)
{
	t109_data_t *ptr = &t109_data_status;
	
	if (force || ptr->status.result.cmd) {
		object_txx_report_msg(&ptr->common, &ptr->status.result, sizeof(ptr->status.result));
	}
}

void object_t109_param_sync(void)
{
	t109_data_t *ptr = &t109_data_status;
	
	if (object_t109_single_end_mode())
		t109_data_sync(ptr, OP_WRITE);
}

void object_t109_data_sync(u8 rw)
{
	t109_data_t *ptr = &t109_data_status;
	object_t109_t *mem = (object_t109_t *)ptr->common.mem;
	
	switch(mem->cmd) {
		case CMD_NONE:
			break;
#ifndef MPTT_MATRIX_NODES
		case CMD_TUNE:
			/* performance calibration */
			MPT_API_CALLBACK(ptr->common.cb, calibrate)();
			
			memset(&ptr->status, 0, sizeof(ptr->status));
#endif
		default:
			ptr->status.result.cmd = mem->cmd;
			mem->cmd = CMD_NONE;
	}
	
	//t109_report_status(ptr);
}

u8 object_t109_single_end_mode(void)
{
	t109_data_t *ptr = &t109_data_status;
	object_t109_t *mem = (object_t109_t *)ptr->common.mem;
	
	if (ptr->status.counter == QTOUCH_CONFIG_VAL(ptr->common.def, sensor_count))
		return (mem->ctrl & MXT_T109_CMD_SNGLENDEN);
	
	return 0;
}

void t109_set_sensor_data(t109_data_t *ptr, u8 channel, u16 compcap)
{
#ifdef OBJECT_T6
	if (object_api_t6_get_status()) {
		memset(ptr->status.node_comp_caps, 0, sizeof(ptr->status.node_comp_caps));
		ptr->status.counter = 0;
		return;
	}
#endif
		
	if (channel < ARRAY_SIZE(ptr->status.node_comp_caps)) {
		ptr->status.node_comp_caps[channel] = compcap;
		ptr->status.counter++;
	}
	
	if (ptr->status.counter == QTOUCH_CONFIG_VAL(ptr->common.def, sensor_count)) {
		ptr->status.result.errorcode = MSG_ERRCODE_SUCCESSFUL;
		t109_report_status(ptr);
	}
}

ssint object_api_t109_set_sensor_data(u8 channel, /*const cap_sample_value_t * const*/ const void * cv)
{
	t109_data_t *ptr = &t109_data_status;
	const cap_sample_value_t * const cval = (const cap_sample_value_t *)cv;
	
	if (!ptr->status.result.cmd)
		return -2;
	
	if (cval) {
		t109_set_sensor_data(ptr, channel, cval->cccap);
	}
	
	return 0;
}

#endif