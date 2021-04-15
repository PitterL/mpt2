/*
 * trans.c
 *
 * Created: 6/14/2019 10:14:51 PM
 *  Author: A41450
 */ 

#include <string.h>

#include "include/types.h"
#include "arch/cpu.h"
#include "objects/txx.h"
#include "arch/tslapi.h"

/* Put API_DEF_TOUCH_MEASUREMENT_PERIOD_MS into Global variable so that we could dynamic modify it */
#define DEF_TOUCH_MEASUREMENT_PERIOD_MS MTOUCH_SCAN_INTERVAL
uint8_t adclib_time_elapsed_since_update = DEF_TOUCH_MEASUREMENT_PERIOD_MS;

/* Acquisition set 1 - General settings */
qtm_acq_node_group_config_t ptc_qtlib_acq_gen1 = {
	.num_sensor_nodes = MTOUCH_SENSORS, 
	.acq_sensor_type = NODE_SELFCAP
};

/* Keys set 1 - General settings */

qtm_touch_key_group_config_t qtlib_key_grp_config_set1 = {
	.num_key_sensors = MTOUCH_BUTTONS,
	.sensor_touch_di = 0,
	.sensor_max_on_time = (MTOUCH_BUTTON_PRESSTIMEOUT >> PARAM_SAMPLING_LOOP_COUNT_SHIFT),
	.sensor_anti_touch_di = (MTOUCH_BUTTON_NEGATIVEDEVIATION >> PARAM_SAMPLING_LOOP_COUNT_SHIFT),
	.sensor_anti_touch_recal_thr = RECAL_100,   /* artichout threshold percent shift, Not achieved yet */
	.sensor_touch_drift_rate = (MTOUCH_BUTTON_BASELINE_RATE >> PARAM_SAMPLING_LOOP_COUNT_SHIFT),
	.sensor_anti_touch_drift_rate = /* Unused */0,
	.sensor_drift_hold_time = /* Unused */0,
	.sensor_reburst_mode = /* Unused */0,
	.node_gain = (MTOUCH_BUTTON_READING_GAIN << 4) | MTOUCH_BUTTON_BASELINE_GAIN
};

/* scroller Configurations */
#ifdef TOUCH_API_SCROLLER
extern mtouch_slider_t mtouch_slider[];

/* Group Configuration */
qtm_scroller_group_config_t qtm_scroller_group_config1 = {
	.num_scrollers = MTOUCH_SLIDERS
};
#endif

/* Surface Config */
#ifdef TOUCH_API_SURFACE
extern mtouch_surface_t mtouch_surface;
#endif

/* Acquisition timer schedule interval */
#ifdef OBJECT_T7
extern uint8_t qtlib_time_elapsed_since_update;
#endif

#ifdef OBJECT_T25
extern uint8_t qtlib_suspend;
#endif

#ifdef OBJECT_T15
#ifndef TOUCH_API_BUTTON
#error "Defined OBJECT T15 But no Button"
#endif
#endif

#ifdef OBJECT_T9
#if !(defined(TOUCH_API_SURFACE) || defined(TOUCH_API_SCROLLER))
//#error "Defined OBJECT T9 But no surface"
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
	/* { API_DEF_TOUCH_MEASUREMENT_PERIOD_MS } Not support untill put it into a global 'varible qtlib_time_elapsed_since_update' */
	{API_DEF_TOUCH_MEASUREMENT_PERIOD_MS, &qtlib_time_elapsed_since_update, sizeof(qtlib_time_elapsed_since_update), 0 },
#endif
	//TBD
	//{API_DEF_SEL_FREQ_INIT, &ptc_qtlib_acq_gen1.freq_option_select, sizeof(ptc_qtlib_acq_gen1.freq_option_select), 0 },
	
	// Invalid whatever changed
	{API_NUM_CHANNELS, &ptc_qtlib_acq_gen1.num_sensor_nodes, sizeof(ptc_qtlib_acq_gen1.num_sensor_nodes), 0 },
	{API_DEF_SENSOR_TYPE, &ptc_qtlib_acq_gen1.acq_sensor_type, sizeof(ptc_qtlib_acq_gen1.acq_sensor_type), 0 },

	{API_NODE_PARAMS_CSD, &mtouch_sensor[0].acquisition_time, sizeof(mtouch_sensor[0].acquisition_time), sizeof(mtouch_sensor[0]) },
	{API_NODE_PARAMS_RESISTOR_PRESCALER, &mtouch_sensor[0].precharge_time, sizeof(mtouch_sensor[0].precharge_time), sizeof(mtouch_sensor[0]) },
	{API_NODE_PARAMS_ADC_OVERSAMPLING, &mtouch_sensor[0].oversampling, sizeof(mtouch_sensor[0].oversampling), sizeof(mtouch_sensor[0]) },
	{API_NODE_COMPCAP_VALUE, &mtouch_sensor[0].addcap, sizeof(mtouch_sensor[0].addcap), sizeof(mtouch_sensor[0]) },
	
	{API_KEY_PARAMS_THRESHOLD, &mtouch_button[0].threshold, sizeof(mtouch_button[0].threshold), sizeof(mtouch_button[0]) },
	//{API_KEY_PARAMS_HYSTERESIS},
	{API_KEY_PARAMS_AKS_GROUP, &mtouch_button[0].aks_group, sizeof(mtouch_button[0].aks_group), sizeof(mtouch_button[0]) },
	
	{API_NODE_DELTA_VALUE, &mtouch_button[0].deviation, sizeof(mtouch_button[0].deviation), sizeof(mtouch_button[0]) },
	{API_NODE_SIGNAL_VALUE, &mtouch_button[0].reading, sizeof(mtouch_button[0].reading), sizeof(mtouch_button[0]) },
	{API_NODE_DELTA_VALUE, &mtouch_button[0].baseline, sizeof(mtouch_button[0].baseline), sizeof(mtouch_button[0]) },

	{API_NUM_SENSORS, &qtlib_key_grp_config_set1.num_key_sensors, sizeof(qtlib_key_grp_config_set1.num_key_sensors), 0 },
	{API_DEF_TOUCH_DET_INT, &qtlib_key_grp_config_set1.sensor_touch_di, sizeof(qtlib_key_grp_config_set1.sensor_touch_di), 0 },
	{API_DEF_MAX_ON_DURATION, &qtlib_key_grp_config_set1.sensor_max_on_time, sizeof(qtlib_key_grp_config_set1.sensor_max_on_time), 0 },
	{API_DEF_ANTI_TCH_DET_INT, &qtlib_key_grp_config_set1.sensor_anti_touch_di, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_di), 0 },
	{API_DEF_ANTI_TCH_RECAL_THRSHLD, &qtlib_key_grp_config_set1.sensor_anti_touch_recal_thr, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_recal_thr), 0 },
	{API_DEF_TCH_DRIFT_RATE, &qtlib_key_grp_config_set1.sensor_touch_drift_rate, sizeof(qtlib_key_grp_config_set1.sensor_touch_drift_rate), 0 },
	{API_DEF_ANTI_TCH_DRIFT_RATE, &qtlib_key_grp_config_set1.sensor_anti_touch_drift_rate, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_drift_rate), 0 },
	{API_DEF_DRIFT_HOLD_TIME, &qtlib_key_grp_config_set1.sensor_drift_hold_time, sizeof(qtlib_key_grp_config_set1.sensor_drift_hold_time), 0 },	
	{API_NODE_PARAMS_GAIN, &qtlib_key_grp_config_set1.node_gain, sizeof(qtlib_key_grp_config_set1.node_gain), 0 },	
	

#ifdef TOUCH_API_SCROLLER
	{API_NUM_SLIDERS, &qtm_scroller_group_config1.num_scrollers, sizeof(qtm_scroller_group_config1.num_scrollers), 0},
	{API_SLIDER_START_KEY, &mtouch_slider[0].startSegment, sizeof(mtouch_slider[0].startSegment), sizeof(mtouch_slider[0]) },
	{API_SLIDER_NUM_KEYS, &mtouch_slider[0].segmentCount, sizeof(mtouch_slider[0].segmentCount), sizeof(mtouch_slider[0]) },
	{API_SLIDER_RESOL_DB, &mtouch_slider[0].resolDeadband, sizeof(mtouch_slider[0].resolDeadband), sizeof(mtouch_slider[0]) },

	{API_SLIDER_POS_HYST, &mtouch_slider[0].positionHysteresis, sizeof(mtouch_slider[0].positionHysteresis), sizeof(mtouch_slider[0]) },
	{API_SLIDER_MIN_CONTACT, &mtouch_slider[0].contactThreshold, sizeof(mtouch_slider[0].contactThreshold), sizeof(mtouch_slider[0]) },
#endif

#ifdef TOUCH_API_SURFACE
	{API_SURFACE_CS_START_KEY_V, &mtouch_surface.start_segment_v, sizeof(mtouch_surface.start_segment_v), 0 },
	{API_SURFACE_CS_START_KEY_H, &mtouch_surface.start_segment_h, sizeof(mtouch_surface.start_segment_h), 0 },
	{API_SURFACE_CS_NUM_KEYS_V, &mtouch_surface.number_of_segments_v, sizeof(mtouch_surface.number_of_segments_v), 0 },
	{API_SURFACE_CS_NUM_KEYS_H, &mtouch_surface.number_of_segments_h, sizeof(mtouch_surface.number_of_segments_h), 0 },
	
	{API_SURFACE_CS_RESOL_DB, &mtouch_surface.resol_deadband, sizeof(mtouch_surface.resol_deadband), 0 },
	{API_SURFACE_CS_POS_HYST, &mtouch_surface.position_hysteresis, sizeof(mtouch_surface.position_hysteresis), 0 },
	{API_SURFACE_CS_FILT_CFG, &mtouch_surface.position_filter, sizeof(mtouch_surface.position_filter), 0 },
	{API_SURFACE_CS_MIN_CONTACT, &mtouch_surface.contact_min_threshold, sizeof(mtouch_surface.contact_min_threshold), 0 }
#endif
};

#define TCH_CONFIG_WRITEBACK_NUM ARRAY_SIZE(touch_config_list)

void force_init_sensor_key(u8 sensor_node)
{
	MTOUCH_Sensor_Initialize(sensor_node);
}

void force_init_all_sensor_key(void) {
	const qtm_touch_key_group_config_t * const qttkg = &qtlib_key_grp_config_set1;
	u8 i;
	
	for ( i = 0; i < qttkg->num_key_sensors; i++) {
		force_init_sensor_key(i);
	}
}

#ifdef TOUCH_API_SCROLLER
void force_init_slider(void)
{
	MTOUCH_Slider_InitializeAll();
}
#endif

#ifdef TOUCH_API_SURFACE
void force_init_surface(void)
{
	MTOUCH_Surface_InitializeAll();
}
#endif

void force_parameters(u8 type, u8 index)
{		
	switch (type)
	{
		case API_NODE_PARAMS_CSD:
		case API_NODE_PARAMS_RESISTOR_PRESCALER:
		case API_NODE_PARAMS_GAIN:
		case API_NODE_PARAMS_ADC_OVERSAMPLING:
			force_init_sensor_key(index);
			break;
		
		case API_KEY_PARAMS_THRESHOLD:
		case API_KEY_PARAMS_HYSTERESIS:
		case API_KEY_PARAMS_AKS_GROUP:
		
		case API_DEF_TOUCH_DET_INT:
		case API_DEF_MAX_ON_DURATION:
		case API_DEF_ANTI_TCH_DET_INT:
		case API_DEF_ANTI_TCH_RECAL_THRSHLD:
		case API_DEF_TCH_DRIFT_RATE:
		case API_DEF_ANTI_TCH_DRIFT_RATE:
		case API_DEF_DRIFT_HOLD_TIME:
			break;
#ifdef TOUCH_API_SCROLLER
		case API_SLIDER_START_KEY:
		case API_SLIDER_NUM_KEYS:
		case API_SLIDER_RESOL_DB:
		//case API_SLIDER_FILT_CFG:
		case API_SLIDER_POS_HYST:
		case API_SLIDER_MIN_CONTACT:
			force_init_slider();
			break;
#endif
#ifdef TOUCH_API_SURFACE
		case API_SURFACE_CS_START_KEY_H:
		case API_SURFACE_CS_NUM_KEYS_H:
		case API_SURFACE_CS_START_KEY_V:
		case API_SURFACE_CS_NUM_KEYS_V:
		case API_SURFACE_CS_RESOL_DB:
		case API_SURFACE_CS_FILT_CFG:
		case API_SURFACE_CS_POS_HYST:
		case API_SURFACE_CS_MIN_CONTACT:
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
	} else {
		dst = param->buf + trunk_size;
		src = buf;
	}
	
	result = common_read_write(dst, src, size);
	if (result == 0) {
		if (rw == OP_WRITE) {
			force_parameters(param->type, index);
		}
	}
	
	return 0;
}

ssint tsapi_config_op(u8 type, void *buf, size_t size, u8 index, u8 rw)
{
	const qtm_touch_key_group_config_t * const qttkg = &qtlib_key_grp_config_set1;
	const tch_config_callback_t * const tccs = &touch_config_list[0];
	u8 i;
	ssint result = -1;
		
	if (index >= (u8)qttkg->num_key_sensors)
		return result;
	
	for (i = 0; i < TCH_CONFIG_WRITEBACK_NUM; i++) {
		if (tccs[i].type == type) {
			result = tch_config_rw(&tccs[i], buf, size, index, rw);
			break;
		}
	}
		
	return result;
}

u8 tsapi_read_group_config_byte(u8 type, u8 index)
{
	u8 buf[1];
	ssint result;

	result = tsapi_config_op(type, buf, sizeof(buf), index, OP_READ);
	if (result == 0)
		return buf[0];
	
	return (u8)-2;
}

u8 tsapi_read_config_byte(u8 type)
{
	return tsapi_read_group_config_byte(type, 0);
}

void tsapi_calibrate(void)
{
	const qtm_touch_key_group_config_t * const qttkg = &qtlib_key_grp_config_set1;
	u8 i;
	
	for ( i = 0; i < qttkg->num_key_sensors; i++) {
		MTOUCH_Sensor_Calibrate(i);
	}
}

u8 tsapi_get_chip_state(void)
{
	u8 state = 0;

#ifdef OBJECT_T6
	const qtm_touch_key_group_config_t * const qttkg = &qtlib_key_grp_config_set1;
	u8 i;

	for (i = 0; i < qttkg->num_key_sensors; i++) {
		if (!mtouch_sensor[i].enabled)
			state = MXT_T6_STATUS_RESET;
		else if (!mtouch_sensor[i].calibrated || !mtouch_sensor[i].sampled)
			state = MXT_T6_STATUS_CAL;
		
		if (state)
			break;
	}
	
#endif

	return state;
}

ssint tsapi_read_ref_signal_cap(u8 index, cap_sample_value_t *cval)
{
	const qtm_touch_key_group_config_t * const qttkg = &qtlib_key_grp_config_set1;

	if (index >= qttkg->num_key_sensors)
		return -2;

	cval->reference = mtouch_button[index].baseline >> MTOUCH_BUTTON_BASELINE_GAIN;
	cval->signal = mtouch_button[index].reading;
	//FIXME: button order may not match sensor order
	cval->comcap = mtouch_sensor[mtouch_button[index].sensor].addcap;
	cval->cccap = CALCULATE_CAP(cval->comcap);

	return 0;
}

ssint tsapi_read_button_state(u8 index)
{	
	const qtm_touch_key_group_config_t * const qttkg = &qtlib_key_grp_config_set1;

	if (index >= qttkg->num_key_sensors)
		return -2;

	return (ssint)MTOUCH_Button_isPressed(index);
}

#ifdef TOUCH_API_SCROLLER
ssint tsapi_read_slider_state(u8 index, /*t9_point_status_t */void *sts)
{	
	const qtm_scroller_group_config_t * const qsg = &qtm_scroller_group_config1;
    t9_point_status_t *t9_sts = (t9_point_status_t *)sts;
    
	if (index >= qsg->num_scrollers)
		return -2;

	if (MTOUCH_Slider_isPressed(index)) {
		t9_sts->status = MXT_T9_DETECT;
		if (MTOUCH_Slider_isPositionChanged(index))
			t9_sts->status |= MXT_T9_MOVE;
		else
			t9_sts->status |= MXT_T9_PRESS;
	} else {
		t9_sts->status = 0;
	}

	t9_sts->pos.x = MTOUCH_Slider_Position_Get(index);
	t9_sts->pos.y = 0;

	return 0;
}
#endif

#ifdef TOUCH_API_SURFACE
ssint tsapi_read_surface_state(u8 id, /*t9_point_status_t */void *sts)
{	
    t9_point_status_t *t9_sts = (t9_point_status_t *)sts;
    u8 state;

	state = MTOUCH_Surface_Status_Get();
	
	if (state & TOUCH_ACTIVE) {
		t9_sts->status = MXT_T9_DETECT;
		if (state & POSITION_CHANGE)
			t9_sts->status |= MXT_T9_MOVE;
		else
			t9_sts->status |= MXT_T9_PRESS;
	} else {
		t9_sts->status = 0;
	}

	t9_sts->pos.x = MTOUCH_Surface_Position_Get(VERTICAL, id);
	t9_sts->pos.y = MTOUCH_Surface_Position_Get(HORIZONTAL, id);;

	return 0;
}
#endif

u8 tsapi_t8_sensing_mode_translate(u8 mode, u8 rw)
{	
	if (rw == OP_READ) {
		return MXT_T8_MEASALLOW_SELFTCH;
	} else {
		return (u8)-1;
	}
}