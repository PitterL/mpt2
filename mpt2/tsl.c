/*
 * trans.c
 *
 * Created: 6/14/2019 10:14:51 PM
 *  Author: A41450
 */ 

#include <string.h>
#include <touch_api_ptc.h>
#include "mptt.h"
#include "tsl.h"

//extern qtm_surface_cs_control_t qtm_surface_cs_control1;
/* Node status, signal, calibration values */
//extern qtm_acq_node_data_t ptc_qtlib_node_stat1[DEF_NUM_CHANNELS];

/* Node configure parameters */
extern qtm_acq_t161x_node_config_t ptc_seq_node_cfg1[];

/* Key Configurations */
extern qtm_touch_key_config_t qtlib_key_configs_set1[];

/* Key data */
extern qtm_touch_key_data_t qtlib_key_data_set1[];

/* Keys set 1 - General settings */
extern qtm_touch_key_group_config_t qtlib_key_grp_config_set1;

/* Container structure for sensor group */
extern qtm_acquisition_control_t qtlib_acq_set1;

/* Acquisition set 1 - General settings */
extern qtm_acq_node_group_config_t ptc_qtlib_acq_gen1;

/* Surface Config */
extern qtm_surface_cs_config_t qtm_surface_cs_config1;

/* Surface Data */
extern qtm_surface_contact_data_t qtm_surface_cs_data1;

/* Acquisition timer schedule interval */
#ifdef OBJECT_T7
extern uint8_t qtlib_time_elapsed_since_update;
#endif

enum {
	COMMON_RW,
	BIT_RW,
	ARRAY_MEM_RW,
	NUM_RW_OP
};

typedef struct tch_config_callback {
	u8 type;
	void *buf;
	size_t size;	//For common_write, this is data size, for array_member_write, this is struct size
	u8 trunk_size;
	u8 mask;
	u8 wbflag;
} tch_config_callback_t;

tch_config_callback_t touch_config_list[] ={
#ifdef OBJECT_T7
	/* { DEF_TOUCH_MEASUREMENT_PERIOD_MS } Not support untill put it into a global 'varible qtlib_time_elapsed_since_update' */
	{DEF_TOUCH_MEASUREMENT_PERIOD_MS, &qtlib_time_elapsed_since_update, sizeof(qtlib_time_elapsed_since_update), 0, 0, COMMON_RW },
#endif
	{DEF_SEL_FREQ_INIT, &ptc_qtlib_acq_gen1.freq_option_select, sizeof(ptc_qtlib_acq_gen1.freq_option_select), 0, 0, COMMON_RW },
	{DEF_SENSOR_TYPE, &ptc_qtlib_acq_gen1.acq_sensor_type, sizeof(ptc_qtlib_acq_gen1.acq_sensor_type), 0, 0, COMMON_RW },
	
	{NODE_PARAMS_CSD, &ptc_seq_node_cfg1[0].node_csd, sizeof(ptc_seq_node_cfg1[0].node_csd), sizeof(ptc_seq_node_cfg1[0]), 0, ARRAY_MEM_RW },
	{NODE_PARAMS_RESISTOR_PRESCALER, &ptc_seq_node_cfg1[0].node_rsel_prsc, sizeof(ptc_seq_node_cfg1[0].node_rsel_prsc), sizeof(ptc_seq_node_cfg1[0]), 0, ARRAY_MEM_RW },
	{NODE_PARAMS_GAIN, &ptc_seq_node_cfg1[0].node_gain, sizeof(ptc_seq_node_cfg1[0].node_gain), sizeof(ptc_seq_node_cfg1[0]), 0, ARRAY_MEM_RW },
	{NODE_PARAMS_ADC_OVERSAMPLING, &ptc_seq_node_cfg1[0].node_oversampling, sizeof(ptc_seq_node_cfg1[0].node_oversampling), sizeof(ptc_seq_node_cfg1[0]), 0, ARRAY_MEM_RW },
	
	{KEY_PARAMS_THRESHOLD, &qtlib_key_configs_set1[0].channel_threshold, sizeof(qtlib_key_configs_set1[0].channel_threshold), sizeof(qtlib_key_configs_set1[0]), 0, ARRAY_MEM_RW},
	{KEY_PARAMS_HYSTERESIS, &qtlib_key_configs_set1[0].channel_hysteresis, sizeof(qtlib_key_configs_set1[0].channel_hysteresis), sizeof(qtlib_key_configs_set1[0]), 0, ARRAY_MEM_RW},
	{KEY_PARAMS_AKS_GROUP, &qtlib_key_configs_set1[0].channel_aks_group, sizeof(qtlib_key_configs_set1[0].channel_aks_group), sizeof(qtlib_key_configs_set1[0]), 0, ARRAY_MEM_RW},
	
	{DEF_TOUCH_DET_INT, &qtlib_key_grp_config_set1.sensor_touch_di, sizeof(qtlib_key_grp_config_set1.sensor_touch_di), 0, 0, COMMON_RW},
	{DEF_MAX_ON_DURATION, &qtlib_key_grp_config_set1.sensor_max_on_time, sizeof(qtlib_key_grp_config_set1.sensor_max_on_time), 0, 0, COMMON_RW},
	{DEF_ANTI_TCH_DET_INT, &qtlib_key_grp_config_set1.sensor_anti_touch_di, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_di), 0, 0, COMMON_RW},
	{DEF_ANTI_TCH_RECAL_THRSHLD, &qtlib_key_grp_config_set1.sensor_anti_touch_recal_thr, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_recal_thr), 0, 0, COMMON_RW},
	{DEF_TCH_DRIFT_RATE, &qtlib_key_grp_config_set1.sensor_touch_drift_rate, sizeof(qtlib_key_grp_config_set1.sensor_touch_drift_rate), 0, 0, COMMON_RW},
	{DEF_ANTI_TCH_DRIFT_RATE, &qtlib_key_grp_config_set1.sensor_anti_touch_drift_rate, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_drift_rate), 0, 0, COMMON_RW},
	{DEF_DRIFT_HOLD_TIME, &qtlib_key_grp_config_set1.sensor_drift_hold_time, sizeof(qtlib_key_grp_config_set1.sensor_drift_hold_time), 0, 0, COMMON_RW},		

	{SURFACE_CS_START_KEY_V, &qtm_surface_cs_config1.start_key_v, sizeof(qtm_surface_cs_config1.start_key_v), 0, 0, COMMON_RW},
	{SURFACE_CS_START_KEY_H, &qtm_surface_cs_config1.start_key_h, sizeof(qtm_surface_cs_config1.start_key_h), 0, 0, COMMON_RW},
	{SURFACE_CS_NUM_KEYS_V, &qtm_surface_cs_config1.number_of_keys_v, sizeof(qtm_surface_cs_config1.number_of_keys_v), 0, 0, COMMON_RW},
	{SURFACE_CS_NUM_KEYS_H, &qtm_surface_cs_config1.number_of_keys_h, sizeof(qtm_surface_cs_config1.number_of_keys_h), 0, 0, COMMON_RW},
	{SURFACE_CS_POS_HYST, &qtm_surface_cs_config1.position_hysteresis, sizeof(qtm_surface_cs_config1.position_hysteresis), 0, 0, COMMON_RW},
	{SURFACE_CS_FILT_CFG, &qtm_surface_cs_config1.position_filter, sizeof(qtm_surface_cs_config1.position_filter), 0, 0, COMMON_RW},
};

#define TCH_CONFIG_WRITEBACK_NUM ARRAY_SIZE(touch_config_list)

void common_write(void *dst, const void *src, size_t size)
{
	memcpy(dst, src, size);
}

void bit_write(void *dst, const void *src, u8 mask)
{
	*(u8 *)dst = ((*(u8 *)dst) & ~mask) | ((*(u8 *)src) & mask);
}

void array_member_write(void *dst, const void *src, size_t size)
{
	memcpy(dst, src, size);
}

static inline ssint tch_config_rw(const tch_config_callback_t *param, void *buf, size_t size, u8 index, u8 rw)
{
	void *dst, *src;
	size_t trunk_size = 0;
	
	if (size != param->size)
		return -2;
	
	if (param->wbflag == ARRAY_MEM_RW)	//Array offset
		trunk_size = param->trunk_size ? param->trunk_size * index : 0;
		
	if (rw) {	//read: 1, write: 0
		dst = buf;
		src = param->buf + trunk_size;
	}else {
		dst = param->buf + trunk_size;
		src = buf;
	}
	
	switch (param->wbflag) {
		case COMMON_RW:
		case ARRAY_MEM_RW:
			common_write(dst, src, param->size);
		break;
		case BIT_RW:
			bit_write(dst, src, param->mask);
		break;
		default:
		;
	}
	
	return 0;
}

ssint tch_config_op(u8 type, void *buf, size_t size, u8 index, u8 rw)
{
	const tch_config_callback_t *tccs = &touch_config_list[0];
	u8 i;
	ssint result = -1;
	
	for (i = 0; i < TCH_CONFIG_WRITEBACK_NUM; i++) {
		if (tccs[i].type == type) {
			result = tch_config_rw(&tccs[i], buf, size, index, rw);
			break;
		}
	}
	
	return result;
}

ssint tch_config_write(u8 type, void *src, size_t size, u8 index) 
{
	return tch_config_op(type, src, size, index, 0);
}

ssint tch_config_read(u8 type, void *dst, size_t size, u8 index) 
{
	return tch_config_op(type, dst, size, index, 1);
}

void tch_calibrate(void)
{
	qtm_acq_node_group_config_t *qtacq = &ptc_qtlib_acq_gen1;
	u8 i;
	
	for (i = 0; i < (u8)qtacq->num_sensor_nodes; i++) {
		calibrate_node(i);
	}
}

void tch_ref_signal_update(void)
{
	const qtm_touch_key_data_t *qtkds = &qtlib_key_data_set1[0];
	const qtm_touch_key_group_config_t *qttkg = &qtlib_key_grp_config_set1;
	u8 i;
	
	for (i = 0; i < qttkg->num_key_sensors; i++) {
		mpt_api_set_sensor_data(i, qtkds->sensor_state, qtkds[i].channel_reference, qtkds[i].node_data_struct_ptr->node_acq_signals, qtkds[i].node_data_struct_ptr->node_comp_caps);
	}
}

void tch_button_press_report(void)
{
	const qtm_touch_key_data_t *qtkds = &qtlib_key_data_set1[0];
	const qtm_touch_key_group_config_t *qttkg = &qtlib_key_grp_config_set1;
	u8 i, status;
	
	for (i = 0; i < qttkg->num_key_sensors; i++) {
		if (qtkds[i].sensor_state & KEY_TOUCHED_MASK) {
			status = 1;
		}else {
			status = 0;
		}
		mpt_api_set_button_status(i, status);
	}
}

void tch_touch_location_report(void)
{
	const qtm_surface_contact_data_t *qtsf = &qtm_surface_cs_data1;
	u8 t9_status = 0;
	
	if (qtsf->qt_surface_status & TOUCH_ACTIVE)
		t9_status = MXT_T9_DETECT;
	
	if(qtsf->qt_surface_status & POSITION_CHANGE)
		t9_status |= MXT_T9_MOVE;
	else
		t9_status |= MXT_T9_PRESS;

#define SURFACE_INST_ID 0
#define SURFACE_FINGER_ID 0
	mpt_api_set_pointer_location(SURFACE_INST_ID, SURFACE_FINGER_ID, t9_status,  qtsf->h_position, qtsf->v_position);	
}

qbutton_config_t buttons_config[MXT_TOUCH_KEYARRAY_T15_INST] = {
	{ .node = {	.origin = 0, .size = 7 } },
	{ .node = {	.origin = 7, .size = 1 } },
	{ .node = {	.origin = 8, .size = 2 } },
};

qsurface_config_t surfaces_sliders_config[MXT_TOUCH_MULTI_T9_INST] = {

};

qtouch_config_t tsl_qtouch_def = {
	.matrix_xsize = 5,
	.matrix_ysize = 5,
	
	//If define num_surfaces_slider, should filled the surfaces_sliders_config
	.surface_sliders = &surfaces_sliders_config[0],
	.num_surfaces_slider = ARRAY_SIZE(surfaces_sliders_config),
	
	//If define num_buttons, should filled the buttons_config
	.buttons = &buttons_config[0],
	.num_buttons = ARRAY_SIZE(buttons_config),
};

qtouch_api_callback_t tsl_api_info =
{
	.write = tch_config_write,
	.read = tch_config_read,
	.sync = tch_config_op,
	.calibrate = tch_calibrate,
};

tsl_interface_info_t interface_tsl = 
{
	.api = &tsl_api_info,
	.qtdef = &tsl_qtouch_def,
};

void tch_assert_irq(void)
{
	const tsl_interface_info_t *tsl = &interface_tsl;

	mpt_api_request_irq(tsl->hal->fn_set_chg);
}

void tsl_init(const hal_interface_info_t *hal)
{
	const qtm_surface_cs_config_t *qtcfg = &qtm_surface_cs_config1;
	//qtm_acq_node_group_config_t *qtacq = &ptc_qtlib_acq_gen1;
	tsl_interface_info_t *tsl = &interface_tsl;
	qtouch_config_t *qdef = (qtouch_config_t *)tsl->qtdef;
	
	// Save HAL interface
	tsl->hal = hal;
	
	qdef->num_sensor_params = ptc_qtlib_acq_gen1.num_sensor_nodes;
	if (!qdef->buttons[0].node.size) {
		qdef->num_buttons = ARRAY_SIZE(buttons_config);
		qdef->buttons[0].node.origin = 0;
		qdef->buttons[0].node.size = qdef->num_sensor_params; 
	}
	
	// For simpling the algorithm, we set v for x, h for y, but must care, v should start first, h follow up
	if (!qdef->surface_sliders[0].xnode.size) {
		// Maxtrix
		qdef->surface_sliders[0].xnode.origin = qtcfg->start_key_v;
		qdef->surface_sliders[0].xnode.size = qtcfg->number_of_keys_v;
		qdef->surface_sliders[0].ynode.origin = qtcfg->start_key_h;
		qdef->surface_sliders[0].ynode.size = qtcfg->number_of_keys_h;
		
		// Resolution
		qdef->surface_sliders[0].resolution_bit = (qtm_surface_cs_config1.resol_deadband >> 4);
		
		// Deadband percentage
		// qdef->surface_sliders[0].deadband = qtm_surface_cs_config1.resol_deadband & 0xf;
	}
	
	mpt_chip_init(tsl);
}

void tsl_start(void)
{
	mpt_chip_start();
}

void tsl_process(void)
{	
	tch_ref_signal_update();
	tch_touch_location_report();
	tch_button_press_report();

	tch_assert_irq();
	
	mpt_api_process();
}

ssint tsl_write_message(const void *msg)
{
	return mpt_write_message(msg);
}

ssint tsl_read_message(void *msg)
{
	return mpt_read_message(msg);
}

ssint tsl_mem_read(u16 baseaddr, u16 offset, u8 *out_ptr)
{
	return mpt_mem_read(baseaddr, offset, out_ptr);
}

ssint tsl_mem_write(u16 baseaddr, u16 offset, u8 val)
{
	return mpt_mem_write(baseaddr, offset, val);
}