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

#ifdef TOUCH_API_KEYS_H
#ifdef OBJECT_T15
#define TOUCH_API_BUTTON
#endif
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

/* Node status, signal, calibration values */
extern qtm_acq_node_data_t ptc_qtlib_node_stat1[];

/* Container */
extern qtm_touch_key_control_t qtlib_key_set1;
#endif

/* scroller Configurations */
#ifdef TOUCH_API_SCROLLER_H
#ifdef OBJECT_T9
#define TOUCH_API_SCROLLER
#endif
extern qtm_scroller_config_t qtm_scroller_config1[];

/* Group Configuration */
extern qtm_scroller_group_config_t qtm_scroller_group_config1;

/* Individual and Group Data */
extern qtm_scroller_data_t qtm_scroller_data1[];

/* Container */
extern qtm_scroller_control_t qtm_scroller_control1;
#endif

/* Surface Config */
#if (defined(TOUCH_API_SURFACE_CS2T_H) || defined(TOUCH_API_SURFACE_CS_H))
#ifdef OBJECT_T9
#define TOUCH_API_SURFACE
#endif
extern qtm_surface_cs_config_t qtm_surface_cs_config1;

/* Surface Data */
extern qtm_surface_contact_data_t qtm_surface_cs_data1;

/* Container */
extern qtm_surface_cs_control_t qtm_surface_cs_control1;
#endif

/* Acquisition timer schedule interval */
#ifdef OBJECT_T7
extern uint8_t qtlib_time_elapsed_since_update;
#endif

#ifdef OBJECT_T15
#ifndef TOUCH_API_BUTTON
#error "Defined OBJECT T15 But no Button"
#endif
#endif

#ifdef OBJECT_T9
#if !(defined(TOUCH_API_SURFACE) || defined(TOUCH_API_SCROLLER))
#error "Defined OBJECT T9 But no surface"
#endif
#endif

enum {
	COMMON_RW,
	ARRAY_MEM_RW,
	//BIT_RW,
	NUM_RW_OP
};

typedef struct tch_config_callback {
	u8 type;
	void *buf;
	size_t size;	//For common_write, this is data size, for array_member_write, this is struct size
	u8 trunk_size;
} tch_config_callback_t;

tch_config_callback_t touch_config_list[] ={
#ifdef OBJECT_T7
	/* { DEF_TOUCH_MEASUREMENT_PERIOD_MS } Not support untill put it into a global 'varible qtlib_time_elapsed_since_update' */
	{DEF_TOUCH_MEASUREMENT_PERIOD_MS, &qtlib_time_elapsed_since_update, sizeof(qtlib_time_elapsed_since_update), 0 },
#endif
	//TBD
	{DEF_SEL_FREQ_INIT, &ptc_qtlib_acq_gen1.freq_option_select, sizeof(ptc_qtlib_acq_gen1.freq_option_select), 0 },
	
	//qtm_init_sensor_key() + qtm_calibrate_sensor_node()
	{DEF_SENSOR_TYPE, &ptc_qtlib_acq_gen1.acq_sensor_type, sizeof(ptc_qtlib_acq_gen1.acq_sensor_type), 0 },
	
	//qtm_init_sensor_key() + qtm_calibrate_sensor_node()
	{NODE_PARAMS_CSD, &ptc_seq_node_cfg1[0].node_csd, sizeof(ptc_seq_node_cfg1[0].node_csd), sizeof(ptc_seq_node_cfg1[0]) },
	{NODE_PARAMS_RESISTOR_PRESCALER, &ptc_seq_node_cfg1[0].node_rsel_prsc, sizeof(ptc_seq_node_cfg1[0].node_rsel_prsc), sizeof(ptc_seq_node_cfg1[0]) },
	{NODE_PARAMS_GAIN, &ptc_seq_node_cfg1[0].node_gain, sizeof(ptc_seq_node_cfg1[0].node_gain), sizeof(ptc_seq_node_cfg1[0]) },
	{NODE_PARAMS_ADC_OVERSAMPLING, &ptc_seq_node_cfg1[0].node_oversampling, sizeof(ptc_seq_node_cfg1[0].node_oversampling), sizeof(ptc_seq_node_cfg1[0]) },
	
	//qtm_init_sensor_key()
	{KEY_PARAMS_THRESHOLD, &qtlib_key_configs_set1[0].channel_threshold, sizeof(qtlib_key_configs_set1[0].channel_threshold), sizeof(qtlib_key_configs_set1[0]) },
	{KEY_PARAMS_HYSTERESIS, &qtlib_key_configs_set1[0].channel_hysteresis, sizeof(qtlib_key_configs_set1[0].channel_hysteresis), sizeof(qtlib_key_configs_set1[0]) },
	{KEY_PARAMS_AKS_GROUP, &qtlib_key_configs_set1[0].channel_aks_group, sizeof(qtlib_key_configs_set1[0].channel_aks_group), sizeof(qtlib_key_configs_set1[0]) },
	
	//qtm_init_sensor_key()
	{DEF_TOUCH_DET_INT, &qtlib_key_grp_config_set1.sensor_touch_di, sizeof(qtlib_key_grp_config_set1.sensor_touch_di), 0 },
	{DEF_MAX_ON_DURATION, &qtlib_key_grp_config_set1.sensor_max_on_time, sizeof(qtlib_key_grp_config_set1.sensor_max_on_time), 0 },
	{DEF_ANTI_TCH_DET_INT, &qtlib_key_grp_config_set1.sensor_anti_touch_di, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_di), 0 },
	{DEF_ANTI_TCH_RECAL_THRSHLD, &qtlib_key_grp_config_set1.sensor_anti_touch_recal_thr, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_recal_thr), 0 },
	{DEF_TCH_DRIFT_RATE, &qtlib_key_grp_config_set1.sensor_touch_drift_rate, sizeof(qtlib_key_grp_config_set1.sensor_touch_drift_rate), 0 },
	{DEF_ANTI_TCH_DRIFT_RATE, &qtlib_key_grp_config_set1.sensor_anti_touch_drift_rate, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_drift_rate), 0 },
	{DEF_DRIFT_HOLD_TIME, &qtlib_key_grp_config_set1.sensor_drift_hold_time, sizeof(qtlib_key_grp_config_set1.sensor_drift_hold_time), 0 },		

#ifdef TOUCH_API_SCROLLER
	{SLIDER_START_KEY, &qtm_scroller_config1[0].start_key, sizeof(qtm_scroller_config1[0].start_key), sizeof(qtm_scroller_config1[0]) },
	{SLIDER_NUM_KEYS, &qtm_scroller_config1[0].number_of_keys, sizeof(qtm_scroller_config1[0].number_of_keys), sizeof(qtm_scroller_config1[0]) },
	{SLIDER_POS_HYST, &qtm_scroller_config1[0].position_hysteresis, sizeof(qtm_scroller_config1[0].position_hysteresis), sizeof(qtm_scroller_config1[0]) },
	{SLIDER_MIN_CONTACT, &qtm_scroller_config1[0].contact_min_threshold, sizeof(qtm_scroller_config1[0].contact_min_threshold), sizeof(qtm_scroller_config1[0]) },
#endif

#ifdef TOUCH_API_SURFACE
	//qtm_init_surface_cs()
	{SURFACE_CS_START_KEY_V, &qtm_surface_cs_config1.start_key_v, sizeof(qtm_surface_cs_config1.start_key_v), 0 },
	{SURFACE_CS_START_KEY_H, &qtm_surface_cs_config1.start_key_h, sizeof(qtm_surface_cs_config1.start_key_h), 0 },
	{SURFACE_CS_NUM_KEYS_V, &qtm_surface_cs_config1.number_of_keys_v, sizeof(qtm_surface_cs_config1.number_of_keys_v), 0 },
	{SURFACE_CS_NUM_KEYS_H, &qtm_surface_cs_config1.number_of_keys_h, sizeof(qtm_surface_cs_config1.number_of_keys_h), 0 },
	{SURFACE_CS_POS_HYST, &qtm_surface_cs_config1.position_hysteresis, sizeof(qtm_surface_cs_config1.position_hysteresis), 0 },
	{SURFACE_CS_FILT_CFG, &qtm_surface_cs_config1.position_filter, sizeof(qtm_surface_cs_config1.position_filter), 0 },
	//{SURFACE_CS_MIN_CONTACT, }
#endif
};

#define TCH_CONFIG_WRITEBACK_NUM ARRAY_SIZE(touch_config_list)

void force_init_sensor_key(u8 sensor_node, u8 cal)
{
	qtm_init_sensor_key(&qtlib_key_set1, sensor_node, &ptc_qtlib_node_stat1[sensor_node]);
	if (cal)
		qtm_calibrate_sensor_node(&qtlib_acq_set1, sensor_node);
}

void force_init_all_sensor_key(void) {
	const qtm_touch_key_group_config_t *qttkg = &qtlib_key_grp_config_set1;
	u8 i;
	
	for ( i = 0; i < qttkg->num_key_sensors; i++) {
		force_init_sensor_key(i, 1);
	}
}

#ifdef TOUCH_API_SCROLLER
void force_init_slider(void)
{
	qtm_init_scroller_module(&qtm_scroller_control1);
}
#endif

#ifdef TOUCH_API_SURFACE
void force_init_surface(void)
{
	qtm_init_surface_cs(&qtm_surface_cs_control1);
}
#endif

void force_parameters(u8 type, u8 index)
{		
	switch (type)
	{
		case DEF_SENSOR_TYPE:
			force_init_all_sensor_key();
			break;
		case NODE_PARAMS_CSD:
		case NODE_PARAMS_RESISTOR_PRESCALER:
		case NODE_PARAMS_GAIN:
		case NODE_PARAMS_ADC_OVERSAMPLING:
			force_init_sensor_key(index, 1);
			break;
		
		case KEY_PARAMS_THRESHOLD:
		case KEY_PARAMS_HYSTERESIS:
		case KEY_PARAMS_AKS_GROUP:
		
		case DEF_TOUCH_DET_INT:
		case DEF_MAX_ON_DURATION:
		case DEF_ANTI_TCH_DET_INT:
		case DEF_ANTI_TCH_RECAL_THRSHLD:
		case DEF_TCH_DRIFT_RATE:
		case DEF_ANTI_TCH_DRIFT_RATE:
		case DEF_DRIFT_HOLD_TIME:
			force_init_sensor_key(index, 0);
			break;
#ifdef TOUCH_API_SCROLLER
		case SLIDER_START_KEY:
		case SLIDER_NUM_KEYS:
		case SLIDER_RESOL_DB:
		//case SLIDER_FILT_CFG:
		case SLIDER_POS_HYST:
		case SLIDER_MIN_CONTACT:
			force_init_slider();
			break;
#endif
#ifdef TOUCH_API_SURFACE
		case SURFACE_CS_START_KEY_H:
		case SURFACE_CS_NUM_KEYS_H:
		case SURFACE_CS_START_KEY_V:
		case SURFACE_CS_NUM_KEYS_V:
		case SURFACE_CS_RESOL_DB:
		case SURFACE_CS_FILT_CFG:
		case SURFACE_CS_POS_HYST:
		case SURFACE_CS_MIN_CONTACT:
			force_init_surface();
			break;
#endif
		default:
			;
	}
}

ssint common_read_write(void *dst, const void *src, size_t size)
{
	if (memcmp(dst, src, size)) {
		memcpy(dst, src, size);
		return 0;
	}
	
	return -2;
}

static inline ssint tch_config_rw(const tch_config_callback_t *param, void *buf, size_t size, u8 index, u8 rw)
{
	void *dst, *src;
	size_t trunk_size = 0;
	ssint result;
	
	if (size != param->size)
		return -2;
	
	trunk_size = param->trunk_size ? param->trunk_size * index : 0;
		
	if (rw == OP_READ) {	//read: 1, write: 0
		dst = buf;
		src = param->buf + trunk_size;
	}else {
		dst = param->buf + trunk_size;
		src = buf;
	}
	
	result = common_read_write(dst, src, param->size);
	if (result == 0) {
		if (rw == OP_WRITE) {
			force_parameters(param->type, index);
		}
	}
	
	return 0;
}

ssint tch_config_op(u8 type, void *buf, size_t size, u8 index, u8 rw)
{
	qtm_acq_node_group_config_t *qtacq = &ptc_qtlib_acq_gen1;
	const tch_config_callback_t *tccs = &touch_config_list[0];
	u8 i;
	ssint result = -1;
		
	if (index >= (u8)qtacq->num_sensor_nodes)
		return result;
	
	for (i = 0; i < TCH_CONFIG_WRITEBACK_NUM; i++) {
		if (tccs[i].type == type) {
			result = tch_config_rw(&tccs[i], buf, size, index, rw);
			break;
		}
	}
		
	return result;
}

void tch_calibrate(void)
{
	qtm_acq_node_group_config_t *qtacq = &ptc_qtlib_acq_gen1;
	u8 i;
	
	for (i = 0; i < (u8)qtacq->num_sensor_nodes; i++) {
		/* Calibrate Node */
		
		//calibrate_node(i);
		qtm_calibrate_sensor_node(&qtlib_acq_set1, i);
	}
}

#ifdef TOUCH_API_BUTTON
qbutton_config_t buttons_config[MXT_TOUCH_KEYARRAY_T15_INST] = {
#ifdef EVK_QT1
// Note QT1 need modify circle to support QT1
{ .node = {	.origin = 0, .size = 2 } },	// Button
{ .node = {	.origin = 2, .size = 4 } },	// Surface slider
#endif
#ifdef EVK_QT7	
	{ .node = {	.origin = 0, .size = 2 } },	// Button
	{ .node = {	.origin = 2, .size = 3 } },	// Surface slider
#endif
#ifdef EVK_QT8	
	{ .node = {	.origin = 0, .size = 5 } },	// Button
	{ .node = {	.origin = 5, .size = 5 } },	
#endif
};
#endif

#if defined(TOUCH_API_SURFACE) || defined(TOUCH_API_SCROLLER)
// For simpling the algorithm, we set v for x, h for y, but must care, v should start first, h follow up
qsurface_config_t surfaces_sliders_config[MXT_TOUCH_MULTI_T9_INST] = {
	//{ { .origin = 5, .size = 4 }, { .origin = 9, .size = 13 } },
};
#endif

qtouch_config_t tsl_qtouch_def = {
#ifdef EVK_QT1
.matrix_nodes = {{.origin = 0, .size = 2}, {.origin =  2, .size = 4}},
#endif
#ifdef EVK_QT7
	.matrix_nodes = {{.origin = 0, .size = 2}, {.origin =  2, .size = 3}},
#endif

#ifdef TOUCH_API_BUTTON
	//If define num_button, should filled the buttons_config
	.buttons = &buttons_config[0],
	.num_button = ARRAY_SIZE(buttons_config),
	/*.num_buttons_channel_count,*/
#endif

#if defined(TOUCH_API_SURFACE) || defined(TOUCH_API_SCROLLER)
	//If define num_surfaces_slider, should filled the surfaces_sliders_config
	.surface_sliders = &surfaces_sliders_config[0],
	.num_surfaces_slider = ARRAY_SIZE(surfaces_sliders_config),
	/*.num_surfaces,*/
	/*.num_slider,*/
	/*.num_surfaces_slider_channel_count,*/
#endif
};

qtouch_api_callback_t tsl_api_info =
{
	//.read = tch_config_read,
	//.write = tch_config_write,
	.sync = tch_config_op,
#ifdef OBJECT_T6
	.calibrate = tch_calibrate,
#endif
};

tsl_interface_info_t interface_tsl = 
{
	.api = &tsl_api_info,
	.qtdef = &tsl_qtouch_def,
};

void tch_assert_irq(void)
{
	//const tsl_interface_info_t *tsl = &interface_tsl;

	mpt_api_request_irq();
}

void init_maxtrix_node(qtouch_config_t *qdef)
{
	qtm_acq_node_group_config_t *qtacq = &ptc_qtlib_acq_gen1;

	if (!(qdef->matrix_nodes[NODE_X].size || qdef->matrix_nodes[NODE_Y].size)) {
		qdef->matrix_nodes[NODE_X].origin = 0;
		qdef->matrix_nodes[NODE_X].size = (qtacq->num_sensor_nodes + 1) >> 1;
		qdef->matrix_nodes[NODE_Y].origin = qdef->matrix_nodes[NODE_X].size;
		qdef->matrix_nodes[NODE_Y].size = qtacq->num_sensor_nodes - qdef->matrix_nodes[NODE_X].size;
	}
	
	qdef->maxtrix_channel_count = (u8)qtacq->num_sensor_nodes;
}

#ifdef TOUCH_API_BUTTON
void init_button_nodes(qtouch_config_t *qdef)
{
	qtm_acq_node_group_config_t *qtacq = &ptc_qtlib_acq_gen1;
	qbutton_config_t *btn = &qdef->buttons[0];
	u8 i;
	
	for (i = 0; i < qdef->num_button; i++) {
		if (!btn[i].node.size) {
			if (i == 0) {	//All nodes assigned to button 0
				btn[i].node.origin = 0;
				btn[i].node.size = qtacq->num_sensor_nodes;
			} else {
				break;
			}
		}
		qdef->num_button_channel_count += btn[i].node.size;
	}
	
	qdef->num_button = i;
}
#endif

#ifdef TOUCH_API_SCROLLER
void init_slider_nodes(qtouch_config_t *qdef)
{
	qtm_scroller_group_config_t *qsgcfg = &qtm_scroller_group_config1;
	qtm_scroller_config_t *qtcfg = &qtm_scroller_config1[0];
	qsurface_config_t *sursld = &qdef->surface_sliders[0];
	u8 i;
			
	for ( i = 0; i < qsgcfg->num_scrollers && i < qdef->num_surfaces_slider; i++) {
		if (!(sursld[i].nodes[NODE_X].size || sursld[i].nodes[NODE_Y].size)) {
			sursld[i].nodes[NODE_X].origin = 0;
			sursld[i].nodes[NODE_X].size = 0;
			sursld[i].nodes[NODE_Y].origin = qtcfg[i].start_key;
			sursld[i].nodes[NODE_Y].size = qtcfg[i].number_of_keys;
	
			sursld[i].resolution_bit = ((qtcfg->resol_deadband >> 4) - RESOL_2_BIT);
			sursld[i].resolution_max = (1 << sursld->resolution_bit) - 1;
		
			//position_hysteresis
			//contact_min_threshold
		}
		qdef->num_surfaces_slider_channel_count += qtcfg[i].number_of_keys;
	}
	
	qdef->num_slider = i;
}
#endif

#ifdef TOUCH_API_SURFACE
void init_surface_node(qtouch_config_t *qdef)
{
	const qtm_surface_cs_config_t *qtcfg = &qtm_surface_cs_config1;
	qsurface_config_t *sursld = &qdef->surface_sliders[qdef->num_slider];

	if (sursld->nodes[NODE_X].size && sursld->nodes[NODE_Y].size)
		return;

	// For simpling the algorithm, we set v for x, h for y, but must care, v should start first, h follow up
	// Only one surface in atmel start code, so there without loop
	sursld->nodes[NODE_X].origin = qtcfg->start_key_v;	//should start first
	sursld->nodes[NODE_X].size = qtcfg->number_of_keys_v;
	sursld->nodes[NODE_Y].origin = qtcfg->start_key_h;
	sursld->nodes[NODE_Y].size = qtcfg->number_of_keys_h;
		
	// Resolution
	sursld->resolution_bit = ((qtcfg->resol_deadband >> 4) - RESOL_2_BIT);
	sursld->resolution_max = (1 << sursld->resolution_bit) - 1;
	// Deadband percentage
	// sursld->deadband = qtm_surface_cs_config1.resol_deadband & 0xf;
	
	qdef->num_surfaces_slider_channel_count += qtcfg->number_of_keys_v + qtcfg->number_of_keys_h;
	qdef->num_surfaces = 1;
}
#endif

void inlitialize_button_slider_surface_nodes(qtouch_config_t *qdef)
{
	init_maxtrix_node(qdef);

#ifdef TOUCH_API_BUTTON
	init_button_nodes(qdef);
#endif
	
#ifdef TOUCH_API_SCROLLER
	init_slider_nodes(qdef);
#endif

#ifdef TOUCH_API_SURFACE
	init_surface_node(qdef);
#endif
}

void tsl_init(const hal_interface_info_t *hal)
{
	tsl_interface_info_t *tsl = &interface_tsl;
	qtouch_config_t *qdef = (qtouch_config_t *)tsl->qtdef;
	
	// Save HAL interface
	tsl->hal = hal;
	
	inlitialize_button_slider_surface_nodes(qdef);
	
	mpt_chip_init(tsl);
}

void tsl_start(void)
{
	mpt_chip_start();
}

void tch_update_chip_state(void)
{
	#ifdef OBJECT_T6
	const qtm_touch_key_data_t *qtkds = &qtlib_key_data_set1[0];
	const qtm_acq_node_data_t * qtns= &ptc_qtlib_node_stat1[0];
	const qtm_touch_key_group_config_t *qttkg = &qtlib_key_grp_config_set1;
	u8 state, cal;
	u8 i;

	for (i = 0; i < qttkg->num_key_sensors; i++) {
		// FIXME, Why node_acq_status may different with sensor_state?
		cal = qtns[i].node_acq_status & NODE_CAL_MASK;
		if (cal) {
			return mpt_api_set_chip_status(MXT_T6_STATUS_CAL, 1);
		}
		
		state = qtkds[i].sensor_state & ~KEY_TOUCHED_MASK;
		switch (state) {
			case QTM_KEY_STATE_DISABLE:
			mpt_api_set_chip_status(MXT_T6_STATUS_RESET, 1);
			return;
			case QTM_KEY_STATE_CAL:
			mpt_api_set_chip_status(MXT_T6_STATUS_CAL, 1);
			return;
			case QTM_KEY_STATE_CAL_ERR:
			mpt_api_set_chip_status(MXT_T6_STATUS_SIGERR, 1);
			return;
			default:
			;
		}
	}
	
	mpt_api_set_chip_status(MXT_T6_STATUS_RESET|MXT_T6_STATUS_CAL|MXT_T6_STATUS_SIGERR, 0);
	#endif
}

void tsl_pre_process(void)
{
	tch_update_chip_state();
}

void tch_ref_signal_update(void)
{
#if (defined(OBJECT_T37) || defined(OBJECT_T25)) 
	const qtm_touch_key_group_config_t *qttkg = &qtlib_key_grp_config_set1;
	const qtm_touch_key_data_t *qtkds = &qtlib_key_data_set1[0];
	u8 i;

	for (i = 0; i < qttkg->num_key_sensors; i++) {
		mpt_api_set_sensor_data(i, qtkds[i].sensor_state, qtkds[i].channel_reference, qtkds[i].node_data_struct_ptr->node_acq_signals, CALCULATE_CAP(qtkds[i].node_data_struct_ptr->node_comp_caps));
	}
#endif
}

#ifdef TOUCH_API_BUTTON
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
#endif

#ifdef TOUCH_API_SCROLLER
void tch_slider_location_report(const qtouch_config_t *qdef)
{
	const qtm_scroller_data_t * qtsd = &qtm_scroller_data1[0];
	u8 t9_status = 0;
	u8 i;
	
	for ( i = 0; i < qdef->num_slider; i++ ) {
		if (qtsd->scroller_status & TOUCH_ACTIVE)
		t9_status = MXT_T9_DETECT;
		
		if(qtsd->scroller_status & POSITION_CHANGE)
		t9_status |= MXT_T9_MOVE;
		else
		t9_status |= MXT_T9_PRESS;

		#define SLIDER_FINGER_ID 0
		mpt_api_set_pointer_location(i, SLIDER_FINGER_ID, t9_status,  0, qtsd->position/*, qtsd->contact_size*/);
	}
}
#endif

#ifdef TOUCH_API_SURFACE
void tch_surface_location_report(void)
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
	mpt_api_set_pointer_location(SURFACE_INST_ID, SURFACE_FINGER_ID, t9_status,  qtsf->h_position, qtsf->v_position/*, qtsf->contact_size*/);
}
#endif

void tsl_process(void)
{	
#ifdef TOUCH_API_SCROLLER
	tsl_interface_info_t *tsl = &interface_tsl;
	qtouch_config_t *qdef = (qtouch_config_t *)tsl->qtdef;
#endif

	tch_ref_signal_update();

#ifdef TOUCH_API_SCROLLER
	tch_slider_location_report(qdef);
#endif
#ifdef TOUCH_API_SURFACE
	tch_surface_location_report();
#endif
#ifdef TOUCH_API_BUTTON
	tch_button_press_report();
#endif
	
#ifdef OBJECT_WRITEBACK
	//mpt_api_process();
#endif

	//mpt_api_report_status();

	tch_assert_irq();
}

ssint tsl_mem_read(u16 baseaddr, u16 offset, u8 *out_ptr)
{
	return mpt_mem_read(baseaddr, offset, out_ptr);
}

ssint tsl_mem_write(u16 baseaddr, u16 offset, u8 val)
{
	return mpt_mem_write(baseaddr, offset, val);
}

void tsl_end(void)
{

	mpt_api_writeback();
	
	tch_assert_irq();
}