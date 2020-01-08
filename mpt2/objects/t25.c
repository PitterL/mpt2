/*
 * t25.c
 *
 * Created: 6/9/2019 10:28:17 PM
 *  Author: A41450
 */ 
#ifdef OBJECT_T25

#include <string.h>
#include "arch/tslapi.h"
#include "arch/pinconf.h"
#include "txx.h"

t25_data_t t25_data_status;
ssint object_t25_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	t25_data_t *t25_ptr = &t25_data_status;

	object_txx_init(&t25_ptr->common, rid, def, mem, cb);
	
	return 0;
}

void object_t25_start(u8 unused)
{

}

void t25_report_status(t25_data_t *ptr, uint8_t report)
{
	object_t25_t *mem = (object_t25_t *)ptr->common.mem;
	
	if (!ptr->cache.data.status)
		return;

	mem->cmd = MXT_T25_CMD_COMPLETED;

	if (report)
		object_txx_report_msg(&ptr->common, &ptr->cache.data, sizeof(ptr->cache.data));

	memset(&ptr->cache, 0 ,sizeof(ptr->cache));
}

void object_t25_report_status(u8 force)
{
	t25_data_t *ptr = &t25_data_status;
	
	if (force || ptr->cache.data.status) {
		object_txx_report_msg(&ptr->common, &ptr->cache.data, sizeof(ptr->cache.data));
	}
}

void object_t25_data_sync(u8 rw)
{
	t25_data_t *ptr = &t25_data_status;
	object_t25_t *mem = (object_t25_t *)ptr->common.mem;
	u8 testop = 0;	

	if (!(mem->ctrl & MXT_T25_CTRL_ENABLE))
		return;

	if (rw != OP_WRITE)
		return;
		
	switch(mem->cmd) {
		case MXT_T25_CMD_TEST_AVDD:
			testop = BIT_MASK(TEST_AVDD);
		break;
		case MXT_T25_CMD_TEST_PIN_FAULT:
			testop = BIT_MASK(TEST_PINFAULT);
		break;
		case MXT_T25_CMD_TEST_SIGNAL_LIMIT:
			testop = BIT_MASK(TEST_SIGNAL_LIMIT);
		break;
		case MXT_T25_CMD_TEST_ALL:
			testop = TEST_ALL;
		break;
		case MXT_T25_CMD_COMPLETED:
			;
		break;
		default:
			ptr->cache.data.status = MXT_T25_INFO_RESULT_INVALID;
	}

	ptr->cache.testop = testop;
	
	t25_report_status(ptr, 1);
}

#ifdef OBJECT_T15
ssint inspect_button_data(t25_data_t *ptr, u8 channel, u16 reference, u16 cap, object_t25_result_t *rslt)
{
	const qtouch_config_t *qdef = (qtouch_config_t *)ptr->common.def;
	const qbutton_config_t *qbtn = &qdef->buttons[0];
	object_t25_t *mem = (object_t25_t *) ptr->common.mem;
	u8 i, id, st, end;
	u16 baseref;
	
	for ( i = 0; i < qdef->num_button; i++) {
		st = qbtn[i].node.origin;
		end = qbtn[i].node.origin + qbtn[i].node.size;
		if (channel >=  st && channel < end) {
			id = MXT_TOUCH_MULTI_T9_INST + i;
			baseref = object_t15_get_button_base_ref(i);	//If Zero, this will failed
			if (baseref && (mem->siglim[id].lo || mem->siglim[id].up || mem->sigrangelim[id])) {
				if (cap < mem->siglim[id].lo || cap > mem->siglim[id].up ||
					reference < baseref - mem->sigrangelim[id] || reference > baseref + mem->sigrangelim[id]) {
					rslt->data.status = MXT_T25_INFO_RESULT_SIGNAL_LIMIT;
					rslt->data.info[0] = MXT_TOUCH_KEYARRAY_T15;
					rslt->data.info[1] = i;
	#ifdef OBJECT_T25_EXTENSION
					rslt->data.info[2] = channel;
					rslt->data.info[3] = cap & 0xff;	//LSB
					rslt->data.info[4] = (cap >> 8);
					rslt->data.info[5] = reference & 0xff;
					rslt->data.info[6] = (reference >> 8);
	#endif
				}
			}
			rslt->counter[INSPECT_BUTTON]++;
			break;
		}
	}
	
	if ((rslt->counter[INSPECT_BUTTON] >= qdef->num_button_channel_count))
		return 0;
	else
		return -2;
}
#endif

#ifdef OBJECT_T9
ssint inspect_surface_slider_data(t25_data_t *ptr, u8 channel, u16 reference, u16 cap, object_t25_result_t *rslt)
{
	const qtouch_config_t *qdef = (qtouch_config_t *)ptr->common.def;
	object_t25_t *mem = (object_t25_t *) ptr->common.mem;
	u8 i;
	u16 baseref;
	
	for ( i = 0; i < qdef->num_surfaces_slider; i++) {
		//Note: Since there is only 1 Gain in surface, the first X gain will be decided as base gain.
		baseref = object_t9_get_surface_slider_base_ref(i, channel);	//FIXME: here need add T104 support for baseref get
															//If Zero, this will skip	
		if (baseref) {
			if (mem->siglim[i].lo || mem->siglim[i].up || mem->sigrangelim[i]) {
				if (cap < mem->siglim[i].lo || cap > mem->siglim[i].up ||
					reference < baseref - mem->sigrangelim[i] || reference > baseref + mem->sigrangelim[i]) {
					rslt->data.status = MXT_T25_INFO_RESULT_SIGNAL_LIMIT;
					rslt->data.info[0] = MXT_TOUCH_MULTI_T9;
					rslt->data.info[1] = i;
	#ifdef OBJECT_T25_EXTENSION
					rslt->data.info[2] = channel;
					rslt->data.info[3] = cap & 0xff;	//LSB
					rslt->data.info[4] = (cap >> 8);
					rslt->data.info[5] = reference & 0xff;
					rslt->data.info[6] = (reference >> 8);
	#endif
				}
			}
			rslt->counter[INSPECT_SURFACE_SLIDER]++;
			break;
		}
	}
	
	if ((rslt->counter[INSPECT_SURFACE_SLIDER] >= qdef->num_surfaces_slider_channel_count)) {
		return 0;
	} else {
		return -2;
	}
}
#endif

ssint t25_inspect_sensor_data(t25_data_t *ptr, u8 channel, u16 reference, u16 cap)
{
	object_t25_result_t *tdat = &ptr->cache;
	ssint result = -1;
	
#ifdef OBJECT_T15
	if(!tdat->data.status)
		result = inspect_button_data(ptr, channel, reference, cap, tdat);
#endif

#ifdef OBJECT_T9
	if(!tdat->data.status)
		result |= inspect_surface_slider_data(ptr, channel, reference, cap, tdat);
#endif
	
	return result;
}

ssint t25_inspect_avdd(t25_data_t *ptr)
{
	object_t25_result_t *tdat = &ptr->cache;
	uint8_t result;

	if (tdat->data.status)
		return -2;
	
	result = avdd_test();
	if (result) {
		tdat->data.status = MXT_T25_INFO_RESULT_NO_AVDD;
		tdat->data.info[0] = result;
	}

	return 0;
}

ssint t25_inspect_pinfault(t25_data_t *ptr, u8 pindwellus, u8 pinthr)
{
	object_t25_result_t *tdat = &ptr->cache;
	uint8_t seq, pin, val;

	if (tdat->data.status)
		return -2;

	pindwellus = pindwellus ? pindwellus : 200;
	pinthr = pinthr ? pinthr : 225;

	seq = pinfault_test(pindwellus, pinthr, &pin, &val);
	if (seq) {
		tdat->data.status = MXT_T25_INFO_RESULT_PIN_FAULT;
		tdat->data.info[0] = seq;
		tdat->data.info[1] = pin + 1;
		tdat->data.info[2] = val;
	}

	return 0;
}

void t25_inspect_success(t25_data_t *ptr)
{
	object_t25_result_t *tdat = &ptr->cache;

	if (!tdat->data.status)
		tdat->data.status = MXT_T25_INFO_RESULT_PASS;
}

void object_api_t25_set_sensor_data(u8 channel, u16 reference, u16 signal, u16 cap)
{
	t25_data_t *ptr = &t25_data_status;
	object_t25_result_t *tdat = &ptr->cache;
	object_t25_t *mem = (object_t25_t *) ptr->common.mem;
	ssint result = -1;

	if (!(mem->ctrl & MXT_T25_CTRL_ENABLE))
		return;

	//Test AVDD
	if (TEST_BIT(ptr->cache.testop, TEST_AVDD)) {
		result = t25_inspect_avdd(ptr);
		if (result == 0) {
			CLR_BIT(tdat->testop, TEST_AVDD);
		}
	}

	//Test Pin Fault
	if (TEST_BIT(ptr->cache.testop, TEST_PINFAULT)) {
		result = t25_inspect_pinfault(ptr, mem->pindwellus, mem->pinthr);
		if (result == 0) {
			CLR_BIT(tdat->testop, TEST_PINFAULT);
		}
	}

	// Test signal, randomly start tested sensor
	if (TEST_BIT(ptr->cache.testop, TEST_SIGNAL_LIMIT)) {
		result = t25_inspect_sensor_data(ptr, channel, reference, cap);
		if (result == 0) {
			CLR_BIT(tdat->testop, TEST_SIGNAL_LIMIT);
		}
	}

	if (result == 0) {
		t25_inspect_success(ptr);
	}

	t25_report_status(ptr, mem->ctrl & MXT_T25_CTRL_RPTEN);
}

ssint object_api_t25_pinfault_test(void)
{	
	t25_data_t *ptr = &t25_data_status;
	object_t25_t *mem = (object_t25_t *) ptr->common.mem;

	t25_inspect_pinfault(ptr, mem->pindwellus, mem->pinthr);
	
	//Pin Fault Test Failed
	if (ptr->cache.data.status) {
		t25_report_status(ptr, 1);
		return -1;
	}

	return 0;
}

#endif