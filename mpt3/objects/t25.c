/*
 * t25.c
 *
 * Created: 6/9/2019 10:28:17 PM
 *  Author: A41450
 */ 
#ifdef OBJECT_T25

#include <string.h>
#include "txx.h"

t25_data_t t25_data_status;

DECLARE_STATIC_GLOBAL_FLAG(testop, AVDD);
DECLARE_STATIC_GLOBAL_FLAG(testop, PINFAULT);
DECLARE_STATIC_GLOBAL_FLAG(testop, T9_SIGNAL_LIMIT);
DECLARE_STATIC_GLOBAL_FLAG(testop, T15_SIGNAL_LIMIT);
DECLARE_STATIC_GLOBAL_FLAG(testop, T8_SWITCH_MODE);

/**
 * \Brief the current test progress code
 * @mask: the check items mask
 * @Return: return current test operations with the mask, Zero means inactive
 */
u8 t25_test_inprogress(u8 mask)
{
	u8 result = 0;

	if (TEST(testop, AVDD)) {
		SET_BIT(result, TEST_AVDD);
	}

	if (TEST(testop, PINFAULT)) {
		SET_BIT(result, TEST_PINFAULT);
	}

	if (TEST(testop, T9_SIGNAL_LIMIT) || TEST(testop, T15_SIGNAL_LIMIT)) {
		SET_BIT(result, TEST_SIGNAL_LIMIT);
	}

	return result & mask;
}

ssint object_t25_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	t25_data_t *t25_ptr = &t25_data_status;

	object_txx_init(&t25_ptr->common, rid, def, mem, cb);
	
	return 0;
}

void object_t25_start(u8 loaded)
{
	object_t25_data_sync(loaded ? OP_WRITE : OP_READ);
}

void t25_set_report_status(t25_data_t *ptr, u8 result)
{
	object_t25_result_t *rslt = &ptr->result;

	//memset(rslt, 0 ,sizeof(*rslt));

	rslt->status = result;
}

void t25_report_status(t25_data_t *ptr)
{
	object_txx_report_msg(&ptr->common, &ptr->result, sizeof(ptr->result));
}

void object_t25_report_status(u8 force)
{
	t25_data_t *ptr = &t25_data_status;

	if (force || !t25_test_inprogress(ALL_TESTS_MASK)) {
		t25_report_status(ptr);
	}
}

/**
 * \brief T25 memory sychronization
	fill the code to result buffer if fault detected
 * @rw: read or write flag
 * @Return: None
 */
ssint t25_set_command(t25_data_t *ptr, u8 cmd);
void object_t25_data_sync(u8 rw)
{
	t25_data_t *ptr = &t25_data_status;
	object_t25_t *mem = (object_t25_t *)ptr->common.mem;
	ssint result;	

	if (!(mem->ctrl & MXT_T25_CTRL_ENABLE))
		return;

	if (rw != OP_WRITE)
		return;

	// Set test command
	result = t25_set_command(ptr, mem->cmd);
	if (result) {
		// Unsupported
		t25_set_report_status(ptr, MXT_T25_INFO_RESULT_INVALID);
		t25_report_status(ptr);
	}

	// Cycling test parameters
	if (mem->ctrl & MXT_T25_CTRL_PERSTMODE_MASK) {
		// Reset cycling test counter first
		ptr->current_ticks = 0;
		if (!mem->pertstinterval) {
			ptr->cycling_interval = 200;	// Default (2)
		} else {
			ptr->cycling_interval = mem->pertstinterval * 100;
		}
	}

	mem->cmd = MXT_T25_CMD_NONE;
}

typedef enum REPORT_MODE_enum {
	REPORT_NORMAL,
	REPORT_CYCLING,
	REPORT_FAILED,
} REPORT_MODE_t;

/**
 * @brief handle test result when test done
 * 
 * @param ptr: t25 data handle pointer
 * @param result: the test result
 * @param mode: the report mode
 */
void t25_test_done(t25_data_t *ptr, s8 result, REPORT_MODE_t mode) 
{
	object_t25_t *mem = (object_t25_t *)ptr->common.mem;
	bool report = false;

	if (result < 0) {
		switch(mode) {
			case REPORT_FAILED:
				report = true;
				break;
			case REPORT_CYCLING:
				report = (mem->ctrl & MXT_T25_CTRL_RPTEN) && (mem->ctrl & MXT_T25_PERST_MODE_FAILURE);
				break;
			case REPORT_NORMAL:
				report = (mem->ctrl & MXT_T25_CTRL_RPTEN);
			default:
				;
		}
	} else if (result == 0) {
		if (!t25_test_inprogress(ALL_TESTS_MASK)) {
			t25_set_report_status(ptr, MXT_T25_INFO_RESULT_PASS);

			switch(mode) {
				case REPORT_CYCLING:
					report = (mem->ctrl & MXT_T25_CTRL_RPTEN) && (mem->ctrl & MXT_T25_PERST_MODE_SUCCESS);
					break;
				case REPORT_NORMAL:
					report = (mem->ctrl & MXT_T25_CTRL_RPTEN);
					break;
				case REPORT_FAILED:
				default:
					;
			}
		}
	} else if (result > 0) {
		// not completed
	}

	if (report) {
		t25_report_status(ptr);
	}
}

#ifdef OBJECT_T15
/**
 * \brief T25 Signal Limit test by T15, 
	fill the code to result buffer if fault detected
 * @ptr: t25 data handle pointer
 * @channel: node channel to be inspected
 * @reference: node reference
 * @cap: node cap value 
 * @Return: Zero means successful, Positive means the test is not completed, Negative means test is failed
 */
ssint t25_inspect_t15_sensor_data(t25_data_t *ptr, u8 channel, ref_t reference, cap_t cap)
{
	const qtouch_config_t *qdef = (qtouch_config_t *)ptr->common.def;
	const qbutton_config_t *qbtn = &qdef->buttons[0];
	object_t25_t *mem = (object_t25_t *) ptr->common.mem;
	object_t25_result_t *rslt = (object_t25_result_t *)&ptr->result;
	u8 i, id, st, end;
	ssint result = 0;
	ref_t baseref;
	
	for ( i = 0; i < qdef->num_button; i++) {
		// Check whatever the instance is enabled
		st = qbtn[i].node.origin;
		end = qbtn[i].node.origin + qbtn[i].node.size;
		if (channel >=  st && channel < end) {
#ifdef OBJECT_T9
			id = MXT_TOUCH_MULTI_T9_INST + i;
#else
			id = i;
#endif
			result = object_read16(&ptr->common, API_CHANNEL_BASE_REF, (u16 *)&baseref, channel);
			if (result ||
						(mem->siglim[id].lo && cap < mem->siglim[id].lo) || 
						(mem->siglim[id].up && cap > mem->siglim[id].up) ||
						(mem->sigrangelim[id] && 
							(reference < baseref - mem->sigrangelim[id] || reference > baseref + mem->sigrangelim[id]))) {
				rslt->status = MXT_T25_INFO_RESULT_SIGNAL_LIMIT;
				rslt->info[0] = MXT_TOUCH_KEYARRAY_T15;
				rslt->info[1] = i;
#ifdef OBJECT_T25_EXTENSION
				rslt->info[2] = channel;
				rslt->info[3] = (u8)(cap & 0xff);	//LSB
				rslt->info[4] = (u8)(cap >> 8);
				rslt->info[5] = (u8)(reference & 0xff);
				rslt->info[6] = (u8)(reference >> 8);
#endif
				result = -2;
			} else {
				if (!DEC(testop, T15_SIGNAL_LIMIT)) {
					// Test completed
					result = 0;
				}
			}
			break;
		}
	}

	return result;
}
#endif

#ifdef OBJECT_T9
/**
 * \brief T25 Signal Limit test by T9, 
	fill the code to result buffer if fault detected
 * @ptr: t25 data handle pointer
 * @channel: node channel to be inspected
 * @reference: node reference
 * @cap: node cap value 
 * @Return: Zero means successful, Positive means the test is not completed, Negative means test is failed
 */
ssint t25_inspect_t9_sensor_data(t25_data_t *ptr, u8 channel, ref_t reference, cap_t cap)
{
	const qtouch_config_t *qdef = (qtouch_config_t *)ptr->common.def;
	const qsurface_config_t *qsur = &qdef->surface_sliders[0];
	object_t25_t *mem = (object_t25_t *) ptr->common.mem;
	object_t25_result_t *rslt = &ptr->result;
	u8 i, j, id, st, end;
	bool done = false;
	cap_t baseref;
	ssint result = 1;
	
	for ( i = 0; i < qdef->num_surfaces_slider && !done; i++) {
		for (j = 0; j < NUM_NODE_2D; j++) {
			st = qsur[i].nodes[j].origin;
			end = qsur[i].nodes[j].origin + qsur[i].nodes[j].size;
			if (channel >=  st && channel < end) {
				result = object_read16(&ptr->common, API_CHANNEL_BASE_REF, &baseref, channel);
				id = i;
				if (result ||
							(mem->siglim[id].lo && cap < mem->siglim[id].lo) || 
							(mem->siglim[id].up && cap > mem->siglim[id].up) ||
							(mem->sigrangelim[id] && 
								(reference < baseref - mem->sigrangelim[id] || reference > baseref + mem->sigrangelim[id]))) {
					rslt->status = MXT_T25_INFO_RESULT_SIGNAL_LIMIT;
					rslt->info[0] = MXT_TOUCH_MULTI_T9;
					rslt->info[1] = i;
	#ifdef OBJECT_T25_EXTENSION
					rslt->info[2] = channel;
					rslt->info[3] = cap & 0xff;	//LSB
					rslt->info[4] = (cap >> 8);
					rslt->info[5] = reference & 0xff;
					rslt->info[6] = (reference >> 8);
	#endif
					result = -2;
				} else {
					if (DEC(testop, T9_SIGNAL_LIMIT)) {
						result = 0;
						// Test complete
					}
				}

				done = true;
				break;
			}
		}
	}
	
	return result;
}
#endif

/**
 * \brief T25 Avdd test, 
 * @ptr: the object handle
 * @Return: Zero means successful, other value means test is failed
 */
ssint t25_inspect_avdd(t25_data_t *ptr)
{
	object_t25_result_t *rst = &ptr->result;
	u8 val = 0;
	ssint result;

	result = object_cmd(&ptr->common, API_SELFTEST, &val, sizeof(val), AVDD_CMD_RUN);
	if (result) {
		rst->status = MXT_T25_INFO_RESULT_NO_AVDD;
		rst->info[0] = val;
		
		return -2;
	}

	return 0;
}

/**
 * \brief T25 Pin fault test init, 
 * @ptr: the object handle
 * @cycling: whether it's cycling test
 * @Return: None Zero menas initilization is failed
 */
ssint t25_inspect_pinfault(t25_data_t *ptr, u8 cycling)
{
	object_t25_t *mem = (object_t25_t *) ptr->common.mem;
	object_t25_result_t *rst = &ptr->result;
	u8 pindwellus = mem->pindwellus, pinthr = mem->pinthr;
	/*pinfault_test_setting_t */ u8 *buf = &ptr->pinfault_test_setting[0];
	const u8 size = sizeof(ptr->pinfault_test_setting);
	u8 count;
	ssint result = 1;

	// Set pinfault parameters
	if (buf[PF_DONE] || !cycling) {
		pindwellus = pindwellus ? pindwellus : 100;
		pinthr = pinthr ? pinthr : 225;

		if (cycling) {
			count = PF_COUNT_TEST_ONE;	// Test only 1 pin in cycling test
		} else {
			count = PF_COUNT_TEST_ALL;	// Test all pins
		}

		/* set pinfault_test_setting_t structure */
		buf[PF_SEQ] = /* DRIVEN_GROUND */ 1;
		buf[PF_NODE_IDX] = 0;
		buf[PF_THRESHOLD] = pinthr;
		buf[PF_DELAY] = pindwellus;
		buf[PF_COUNT] = count;
		buf[PF_VALUE] = 0;
		buf[PF_DONE] = 0;

		result = object_cmd(&ptr->common, API_SELFTEST, buf, size, PINFAULT_CMD_SET_PARAM);
	}

	// Start pin fault test
	if (result == 0) {
		result = object_cmd(&ptr->common, API_SELFTEST, buf, size, PINFAULT_CMD_RUN);
		if (result) {
			rst->status = MXT_T25_INFO_RESULT_PIN_FAULT;
			rst->info[0] = buf[PF_SEQ];	// seq
			rst->info[1] = buf[PF_NODE_IDX] + 1;	// pin
			rst->info[2] = buf[PF_VALUE];	// val
		} else if (result == 0) {
			if (!buf[PF_DONE]) {	// Not done
				result = 2;
			}
		}
	}

	return result;
}

/**
 * @brief request adc resource
 * 
 * @param ptr :  the object handle
 * @param request : request adc or release the adc
 * @return ssint 0 mean success
 */
ssint t25_inspect_request_resource(t25_data_t *ptr, bool request)
{
	return object_cmd(&ptr->common, API_SELFTEST, (u8 *)&request, sizeof(request), REQUEST_ADC_RESOURCE);
}

/**
 * \Brief the post-stage selftest
 * @channel: the test node channel
 * @cv: the `cap_sample_value_t` structure data pointer
 * @Return: Zero means successful, Positive means the test is not completed, Negative means test is failed
 */
ssint object_api_t25_signal_limit_test(u8 channel, /*const cap_sample_value_t **/ const void * cv)
{
	t25_data_t *ptr = &t25_data_status;
	object_t25_t *mem = (object_t25_t *) ptr->common.mem;
	const cap_sample_value_t * const cval = (const cap_sample_value_t *)cv;
	ssint result = 0;	/* 0: success; negative: failed; other value: not finished */
	bool done = false;

	if (!(mem->ctrl & MXT_T25_CTRL_ENABLE)) {
		return 0;
	}
	
	if (t25_test_inprogress(BIT(TEST_AVDD)|BIT(TEST_PINFAULT))) {
		return 1;
	}

	// Skip when calibrating
	if (!(object_api_t6_get_status() & MXT_T6_STATUS_CAL)) {
		return 2;
	}
	
	// Test signal, randomly start tested sensor
#ifdef OBJECT_T9
	if (result == 0 && TEST(testop, T9_SIGNAL_LIMIT)) {
		result = t25_inspect_t9_sensor_data(ptr, channel, cval->reference, cval->cccap);
		if (result <= 0) {
			CLR(testop, T9_SIGNAL_LIMIT);
			done = true;
		}
	}
#endif

#ifdef OBJECT_T15
	if (result == 0 && TEST(testop, T15_SIGNAL_LIMIT)) {
		result = t25_inspect_t15_sensor_data(ptr, channel, cval->reference, cval->cccap);
		if (result <= 0) {
			CLR(testop, T15_SIGNAL_LIMIT);
			done = true;
		}
	}
#endif

	if (done) {
		t25_test_done(ptr, result, REPORT_NORMAL);
	}

	return result;
}

/**
 * \execute sensor pin fault inspection, 
	see t25 protocol for details
 * @ptr: the object handle
 * @cycling: whether it's cycling test
 * @Return: Zero means successful, Positive means the test is not completed, Negative means test is failed
 */
ssint t25_avdd_pinfault_test(t25_data_t *ptr, bool cycling)
{	
#ifdef OBJECT_T25_PIN_FAULT_ENABLE
	ssint result = 0;
	bool done = false;


	if (!(TEST(testop, AVDD) || TEST(testop, PINFAULT))) {
		return 0;
	}

	result = t25_inspect_request_resource(ptr, true);
	if (result) {
		return result;
	}

	// Test AVDD
	if (TEST(testop, AVDD)) {
		result = t25_inspect_avdd(ptr);
		CLR(testop, AVDD);
		done = true;
	}

	// Test Pinfault
	if (TEST(testop, PINFAULT)) {
		if (result == 0) {
			result = t25_inspect_pinfault(ptr, cycling);
			if (result <= 0) {
				// Done or failed
				CLR(testop, PINFAULT);
				done = true;
			}
		} else {
			// Skip the test if AVDD failed
			CLR(testop, PINFAULT);	
		}
	}

	if (done) {
		t25_test_done(ptr, result, cycling ? REPORT_CYCLING : REPORT_FAILED);
	}

	return t25_inspect_request_resource(ptr, false);

#endif

	return 0;
}

/**
 * \Set t25 selftest
 * @ptr: the object handle data
 * @mask: the test items mask
 * @Return: unhandled command mask
 */
u8 set_command(t25_data_t *ptr, u8 mask)
{
	const qtouch_config_t *qdef = (qtouch_config_t *)ptr->common.def;

	if (!(mask & ALL_TESTS_MASK)) {
		return mask;
	}

	// Clear result cache		
	memset(&ptr->result, 0 ,sizeof(ptr->result));

	if (TEST_BIT(mask, TEST_AVDD)) {
		CLR_BIT(mask, TEST_AVDD);
		SET(testop, AVDD);
	}

	if (TEST_BIT(mask, TEST_PINFAULT)) {
		CLR_BIT(mask, TEST_AVDD);
		SET(testop, PINFAULT);
	}

	if (TEST_BIT(mask, TEST_SIGNAL_LIMIT)) {
#ifdef OBJECT_T9
		SETV(testop, T9_SIGNAL_LIMIT, qdef->num_surfaces_slider_channel_count);	
#endif
#ifdef OBJECT_T15
		SETV(testop, T15_SIGNAL_LIMIT, qdef->num_button_channel_count);
#endif
		// T8 switch on only without cycling mode
		if (object_t8_switch_measure_mode(TEST_BIT(mask, FLAG_CYCLING)) == 0) {
			SET(testop, T8_SWITCH_MODE);
		}
	}

	return mask;
}

/**
 * \Set t25 selftest command
 * @ptr: the object handler
 * @cmd: the test opcode by the register
 * @Return: return 0 if successful, otherwisze failed
 */
ssint t25_set_command(t25_data_t *ptr, u8 cmd)
{
	u8 mask = 0;

	switch(cmd) {
		case MXT_T25_CMD_TEST_AVDD:
			SET_BIT(mask, TEST_AVDD);
		break;
		case MXT_T25_CMD_TEST_PIN_FAULT:
			SET_BIT(mask, TEST_PINFAULT);
		break;
		case MXT_T25_CMD_TEST_SIGNAL_LIMIT:
			SET_BIT(mask, TEST_SIGNAL_LIMIT);
		break;
		case MXT_T25_CMD_TEST_ALL:
			mask = ALL_TESTS_MASK;
		case MXT_T25_CMD_NONE:
			;
		break;
		default:
			return -1;
	}

	if (mask) {
		ptr->opmask[CMD_TEST] = mask;
	}

	return 0;
}

/**
 * \Brief the por-stage selftest
 * @Return: Zero means successful, Positive means the test is not completed, Negative means test is failed
 */
ssint object_api_t25_selftest_porstage(void)
{
	t25_data_t *ptr = &t25_data_status;

	set_command(ptr, BIT(TEST_AVDD) | BIT(TEST_PINFAULT));

	return t25_avdd_pinfault_test(ptr, false);
}

/**
 * \Brief the pre-stage selftest
 * @Return: Zero means successful, Positive means the test is not completed, Negative means test is failed
 */

ssint object_api_t25_selftest_prestage(void)
{
	t25_data_t *ptr = &t25_data_status;
	u8 mask = ptr->opmask[CMD_TEST];
	static u8 flag = 0;
	ssint result;

	if (mask) {
		ptr->opmask[CMD_TEST] = 0;

		// Command test will override cycling flag
		flag = 0;
	} else {
		// Cycling test will only run when idle
		if (!t25_test_inprogress(ALL_TESTS_MASK)) {
			mask = ptr->opmask[CYCLING_TEST];
			ptr->opmask[CYCLING_TEST] = 0;
			flag = BIT(FLAG_CYCLING);
		}
	}

	// Load the test command only in pre-process because there maybe the status switch
	if (mask) {
		set_command(ptr, mask | flag);
	}

	result = t25_avdd_pinfault_test(ptr, !!flag);
	if (result <= 0) {
		flag = 0;
	}

	return result;
}

/**
 * \Brief the post-stage selftest
 * @channel: the test node channel
 * @cv: the `cap_sample_value_t` structure data pointer
 * @Return: Zero means successful, Positive means the test is not completed, Negative means test is failed
 */
ssint object_api_t25_selftest_poststage(u8 channel, /*const cap_sample_value_t **/ const void * cv)
{
	ssint result;

	result = object_api_t25_signal_limit_test(channel, cv);

	// Turn of T8 sensing mode switch
	if (result <= 0 && TEST(testop, T8_SWITCH_MODE)) {
		object_t8_switch_measure_mode(CLR(testop, T8_SWITCH_MODE));
	}

	return result;
}

/**
 * \Brief current t25 test code
 * @Return: return current self test operation mask, Zero means inactive
 */
u8 object_api_t25_test_inprogress(void)
{	
	return t25_test_inprogress(ALL_TESTS_MASK);
}

/**
 * \Update T25 ticks for periodic test
 * @ticks: the ticks elapsed (ms)
 * @Return: None
 */
void object_t25_cycling_ticks(uint8_t ticks)
{
	t25_data_t *ptr = &t25_data_status;
	object_t25_t *mem = (object_t25_t *)ptr->common.mem;
	u8 mask = 0;

	if (!(mem->ctrl & MXT_T25_CTRL_ENABLE))
		return;

	if (!(mem->ctrl & MXT_T25_CTRL_PERSTMODE_MASK))
		return;
	
	ptr->current_ticks += ticks;
	if (ptr->cycling_interval <= ptr->current_ticks) {
		ptr->current_ticks = 0;
		
		if (mem->pertstcfg & MXT_T25_PERTSTCFG_PWRTSTEN) {
			SET_BIT(mask, TEST_AVDD);
		}

		if (mem->pertstcfg & MXT_T25_PERTSTCFG_PINFAULTEN) {
			SET_BIT(mask, TEST_PINFAULT);
		}

		if (mem->pertstcfg & MXT_T25_PERTSTCFG_SIGLIMTSTEN) {
			SET_BIT(mask, TEST_SIGNAL_LIMIT);
		}

		if (mask) {
			ptr->opmask[CYCLING_TEST] = mask;	// Latest ops will override former ops in cycling test
		}
	}
}

#endif