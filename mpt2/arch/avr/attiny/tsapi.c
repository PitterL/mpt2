/*
 * trans.c
 *
 * Created: 6/14/2019 10:14:51 PM
 *  Author: A41450
 */ 


#include <string.h>

#include "include/types.h"
#include "arch/cpu.h"
#include "arch/fuse.h"
#include "objects/txx.h"
#include "arch/tslapi.h"
#include "pack.h"

#ifdef TOUCH_API_KEYS_H
#ifdef OBJECT_T15
#define TOUCH_API_BUTTON
#endif
/* Node configure parameters */
extern qtm_acq_4p_t321x_config_t ptc_seq_node_cfg1[];

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

#if defined(OBJECT_T25) || defined(OBJECT_T37)
extern qtm_comp_to_cc_cache_t ptc_node_cccap_cache[];
#endif

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
extern uint8_t measurement_period_active_store;
extern uint8_t measurement_period_idle_store;
extern uint8_t measurement_active_to_idle;
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

#ifdef OBJECT_T126
extern qtm_auto_scan_config_t auto_scan_setup;
extern uint8_t measurement_period_idle_drift;
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
	/* { API_DEF_TOUCH_MEASUREMENT_PERIOD_MS } Not support untill put it into a global 'measurement_period_active_store' */
	{API_DEF_TOUCH_MEASUREMENT_PERIOD_MS, &measurement_period_active_store, sizeof(measurement_period_active_store), 0 },
	{API_DEF_TOUCH_MEASUREMENT_IDLE_PERIOD_MS, &measurement_period_idle_store, sizeof(measurement_period_idle_store), 0 },
	{API_DEF_TOUCH_ACTIVE_IDLE_TIMEOUT, &measurement_active_to_idle, sizeof(measurement_active_to_idle), 0 },
#endif
	//TBD
	{API_DEF_SEL_FREQ_INIT, &ptc_qtlib_acq_gen1.freq_option_select, sizeof(ptc_qtlib_acq_gen1.freq_option_select), 0 },
	
	//qtm_init_sensor_key() + calibrate_node()
	{API_DEF_SENSOR_TYPE, &ptc_qtlib_acq_gen1.acq_sensor_type, sizeof(ptc_qtlib_acq_gen1.acq_sensor_type), 0 },
	{API_NUM_CHANNELS, &ptc_qtlib_acq_gen1.num_sensor_nodes, sizeof(ptc_qtlib_acq_gen1.num_sensor_nodes), 0 },

	//qtm_init_sensor_key() + calibrate_node()
	{API_NODE_PARAMS_CSD, &ptc_seq_node_cfg1[0].node_csd, sizeof(ptc_seq_node_cfg1[0].node_csd), sizeof(ptc_seq_node_cfg1[0]) },
	{API_NODE_PARAMS_RESISTOR_PRESCALER, &ptc_seq_node_cfg1[0].node_rsel_prsc, sizeof(ptc_seq_node_cfg1[0].node_rsel_prsc), sizeof(ptc_seq_node_cfg1[0]) },
	{API_NODE_PARAMS_GAIN, &ptc_seq_node_cfg1[0].node_gain, sizeof(ptc_seq_node_cfg1[0].node_gain), sizeof(ptc_seq_node_cfg1[0]) },
	{API_NODE_PARAMS_ADC_OVERSAMPLING, &ptc_seq_node_cfg1[0].node_oversampling, sizeof(ptc_seq_node_cfg1[0].node_oversampling), sizeof(ptc_seq_node_cfg1[0]) },
	
	//qtm_init_sensor_key()
	{API_KEY_PARAMS_THRESHOLD, &qtlib_key_configs_set1[0].channel_threshold, sizeof(qtlib_key_configs_set1[0].channel_threshold), sizeof(qtlib_key_configs_set1[0]) },
	{API_KEY_PARAMS_HYSTERESIS, &qtlib_key_configs_set1[0].channel_hysteresis, sizeof(qtlib_key_configs_set1[0].channel_hysteresis), sizeof(qtlib_key_configs_set1[0]) },
	{API_KEY_PARAMS_AKS_GROUP, &qtlib_key_configs_set1[0].channel_aks_group, sizeof(qtlib_key_configs_set1[0].channel_aks_group), sizeof(qtlib_key_configs_set1[0]) },
	
	//qtm_init_sensor_key()
	{API_NUM_SENSORS, &qtlib_key_grp_config_set1.num_key_sensors, sizeof(qtlib_key_grp_config_set1.num_key_sensors), 0 },
	{API_DEF_TOUCH_DET_INT, &qtlib_key_grp_config_set1.sensor_touch_di, sizeof(qtlib_key_grp_config_set1.sensor_touch_di), 0 },
#ifdef OBJECT_T8
	{API_DEF_MAX_ON_DURATION, &qtlib_key_grp_config_set1.sensor_max_on_time, sizeof(qtlib_key_grp_config_set1.sensor_max_on_time), 0 },
	{API_DEF_ANTI_TCH_DET_INT, &qtlib_key_grp_config_set1.sensor_anti_touch_di, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_di), 0 },
	{API_DEF_ANTI_TCH_RECAL_THRSHLD, &qtlib_key_grp_config_set1.sensor_anti_touch_recal_thr, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_recal_thr), 0 },
	{API_DEF_TCH_DRIFT_RATE, &qtlib_key_grp_config_set1.sensor_touch_drift_rate, sizeof(qtlib_key_grp_config_set1.sensor_touch_drift_rate), 0 },
	{API_DEF_ANTI_TCH_DRIFT_RATE, &qtlib_key_grp_config_set1.sensor_anti_touch_drift_rate, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_drift_rate), 0 },
	{API_DEF_DRIFT_HOLD_TIME, &qtlib_key_grp_config_set1.sensor_drift_hold_time, sizeof(qtlib_key_grp_config_set1.sensor_drift_hold_time), 0 },		
#endif
#ifdef OBJECT_T126
	{API_DEF_TOUCH_DRIFT_PERIOD_MS, &measurement_period_idle_drift, sizeof(measurement_period_idle_drift), 0 },
	{API_DEF_QTM_AUTOSCAN_THRESHOLD, &auto_scan_setup.auto_scan_node_threshold, sizeof(auto_scan_setup.auto_scan_node_threshold), 0 },
	{API_DEF_QTM_AUTOSCAN_NODE, &auto_scan_setup.auto_scan_node_number, sizeof(auto_scan_setup.auto_scan_node_number), 0 },
#endif
	//
#ifdef OBJECT_T109
	{API_NODE_COMPCAP_VALUE, &ptc_qtlib_node_stat1[0].node_comp_caps, sizeof(ptc_qtlib_node_stat1[0].node_comp_caps), sizeof(ptc_qtlib_node_stat1[0]) },	
#endif
#ifdef TOUCH_API_SCROLLER
	{API_NUM_SLIDERS, &qtm_scroller_group_config1.num_scrollers, sizeof(qtm_scroller_group_config1.num_scrollers), 0},
	{API_SLIDER_START_KEY, &qtm_scroller_config1[0].start_key, sizeof(qtm_scroller_config1[0].start_key), sizeof(qtm_scroller_config1[0]) },
	{API_SLIDER_NUM_KEYS, &qtm_scroller_config1[0].number_of_keys, sizeof(qtm_scroller_config1[0].number_of_keys), sizeof(qtm_scroller_config1[0]) },
	{API_SLIDER_POS_HYST, &qtm_scroller_config1[0].position_hysteresis, sizeof(qtm_scroller_config1[0].position_hysteresis), sizeof(qtm_scroller_config1[0]) },
	{API_SLIDER_MIN_CONTACT, &qtm_scroller_config1[0].contact_min_threshold, sizeof(qtm_scroller_config1[0].contact_min_threshold), sizeof(qtm_scroller_config1[0]) },
	{API_SLIDER_RESOL_DB, &qtm_scroller_config1[0].resol_deadband, sizeof(qtm_scroller_config1[0].resol_deadband), sizeof(qtm_scroller_config1[0]) },
#endif

#ifdef TOUCH_API_SURFACE
	//qtm_init_surface_cs()
	{API_SURFACE_CS_START_KEY_V, &qtm_surface_cs_config1.start_key_v, sizeof(qtm_surface_cs_config1.start_key_v), 0 },
	{API_SURFACE_CS_START_KEY_H, &qtm_surface_cs_config1.start_key_h, sizeof(qtm_surface_cs_config1.start_key_h), 0 },
	{API_SURFACE_CS_NUM_KEYS_V, &qtm_surface_cs_config1.number_of_keys_v, sizeof(qtm_surface_cs_config1.number_of_keys_v), 0 },
	{API_SURFACE_CS_NUM_KEYS_H, &qtm_surface_cs_config1.number_of_keys_h, sizeof(qtm_surface_cs_config1.number_of_keys_h), 0 },
	{API_SURFACE_CS_POS_HYST, &qtm_surface_cs_config1.position_hysteresis, sizeof(qtm_surface_cs_config1.position_hysteresis), 0 },
	{API_SURFACE_CS_FILT_CFG, &qtm_surface_cs_config1.position_filter, sizeof(qtm_surface_cs_config1.position_filter), 0 },
	{API_SURFACE_CS_MIN_CONTACT, &qtm_surface_cs_config1.contact_min_threshold, sizeof(qtm_surface_cs_config1.contact_min_threshold), 0 },
	{API_SURFACE_CS_RESOL_DB, &qtm_surface_cs_config1.resol_deadband, sizeof(qtm_surface_cs_config1.resol_deadband), 0 }
#endif
};

#define TCH_CONFIG_WRITEBACK_NUM ARRAY_SIZE(touch_config_list)


void force_parameters(u8 type, u8 index)
{		
	switch (type)
	{
		case API_DEF_SENSOR_TYPE:
		case API_NODE_PARAMS_CSD:
		case API_NODE_PARAMS_RESISTOR_PRESCALER:
		case API_NODE_PARAMS_GAIN:
		case API_NODE_PARAMS_ADC_OVERSAMPLING:
			qtm_init_sensor_key_post(index);
			calibrate_node_post(index);
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
			qtm_init_sensor_key_post(index);
			break;
#ifdef TOUCH_API_SCROLLER
		case API_SLIDER_START_KEY:
		case API_SLIDER_NUM_KEYS:
		case API_SLIDER_RESOL_DB:
		//case API_SLIDER_FILT_CFG:
		case API_SLIDER_POS_HYST:
		case API_SLIDER_MIN_CONTACT:
			qtm_init_scroller_module_post();
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
			qtm_init_surface_cs_post();
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
	
	// Not overlap, but could change part of the varible
	if (size > param->size)
		size = param->size;
	
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

u8 tsapi_read_group_config_byte(u8 type, u8 index)
{
	u8 buf[1];
	ssint result;

	result = tsapi_config_op(type, buf, sizeof(buf), index, OP_READ);
	if (result == 0)
		return buf[0];
	
	return (u8)-1;	//Default return 0xff
}

u8 tsapi_read_config_byte(u8 type)
{
	return tsapi_read_group_config_byte(type, 0);
}

void tsapi_calibrate(void)
{
	/* Calibrate Node */
	calibrate_node_post((uint8_t)-1);
}

u8 tsapi_get_chip_state(void)
{
	u8 state = 0;
#ifdef OBJECT_T6
	const qtm_touch_key_data_t * const qtkds = &qtlib_key_data_set1[0];
	const qtm_acq_node_data_t * const qtns= &ptc_qtlib_node_stat1[0];
	const qtm_touch_key_group_config_t * const qttkg = &qtlib_key_grp_config_set1;
	u8 sensor_state, cal;
	u8 i, node;

	for (i = 0; i < (u8)qttkg->num_key_sensors; i++) {
		// FIXME, Why node_acq_status may different with sensor_state?
		node = get_sensor_node_mapping(i, false);
		cal = qtns[node].node_acq_status & NODE_CAL_MASK;
		if (cal) {
			state = MXT_T6_STATUS_CAL;
		} else {
			sensor_state = qtkds[i].sensor_state & ~KEY_TOUCHED_MASK;
			switch (sensor_state) {
			case QTM_KEY_STATE_DISABLE:
				state = MXT_T6_STATUS_RESET;
				break;
			case QTM_KEY_STATE_CAL:
			case QTM_KEY_STATE_CAL_ERR:
				state = MXT_T6_STATUS_CAL;
				break;
			default:
				;
			}
		}
		
		if (state)
			break;
	}
#endif
	return state;
}

u8 tsapi_get_number_sensor_channels(void)
{
	// normal we call tsapi_read_config_byte(API_NUM_CHANNELS), but we need fast access here
	
	// Fast read
	return (u8)ptc_qtlib_acq_gen1.num_sensor_nodes;
}

u8 tsapi_get_number_key_sensors(void)
{
	// normal we call tsapi_read_config_byte(API_NUM_SENSORS), but we need fast access here
	
	// Fast read
	return (u8)qtlib_key_grp_config_set1.num_key_sensors;
}

#ifdef TOUCH_API_SCROLLER
u8 tsapi_get_number_slider_sensors(void)
{
	// normal we call tsapi_read_config_byte(API_NUM_SLIDERS), but we need fast access here
	
	// Fast read
	return (u8)qtm_scroller_group_config1.num_scrollers;
}
#endif

#if defined(OBJECT_T25) || defined(OBJECT_T37)
u16 calculate_and_cache_cccap(qtm_comp_to_cc_cache_t * pcap, u16 comcap)
{
	if (pcap->node_comp_caps != comcap) {
		pcap->node_comp_caps = comcap;
		pcap->node_cc_caps = CALCULATE_CAP(comcap);
	}

	return pcap->node_cc_caps;
}
#endif

ssint tsapi_read_ref_signal_cap(u8 index, cap_sample_value_t *cval)
{
	const qtm_touch_key_group_config_t * const qttkg = &qtlib_key_grp_config_set1;
	//const qtm_touch_key_data_t * const qtkds = &qtlib_key_data_set1[0];
	qtm_comp_to_cc_cache_t * const pcap = &ptc_node_cccap_cache[0];

	if (index >= (u8)qttkg->num_key_sensors)
		return -2;

	cval->reference = get_sensor_node_reference(index);
	cval->signal = get_sensor_node_signal(index);
	//FIXME: button order may not match sensor order
	cval->comcap = get_sensor_cc_val(index);
#if defined(OBJECT_T25) || defined(OBJECT_T37)
	cval->cccap = calculate_and_cache_cccap(pcap + index, cval->comcap);
#endif

	return 0;
}

ssint tsapi_read_button_state(u8 index)
{	
	const qtm_touch_key_group_config_t * const qttkg = &qtlib_key_grp_config_set1;
	const qtm_touch_key_data_t * const qtkds = &qtlib_key_data_set1[0];
	u8 status;

	if (index >= (u8)qttkg->num_key_sensors)
		return -2;

	if (qtkds[index].sensor_state & KEY_TOUCHED_MASK) {
		status = 1;
	} else {
		status = 0;
	}

	return (ssint)status;
}

#ifdef TOUCH_API_SCROLLER
ssint tsapi_read_slider_state(u8 index, /*t9_point_status_t */void *sts)
{
	const qtm_scroller_group_config_t * const qsgc = &qtm_scroller_group_config1;
	const qtm_scroller_data_t * const qtsd = &qtm_scroller_data1[0];

    t9_point_status_t *t9_sts = (t9_point_status_t *)sts;

	if (index >= qsgc->num_scrollers)
		return -2;

	if (qtsd[index].scroller_status & TOUCH_ACTIVE) {
		t9_sts->status = MXT_T9_DETECT;
		if(qtsd[index].scroller_status & POSITION_CHANGE)
			t9_sts->status |= MXT_T9_MOVE;
		else
			t9_sts->status |= MXT_T9_PRESS;
	} else {
		t9_sts->status = 0;
	}

	t9_sts->pos.x = 0;
	t9_sts->pos.y = qtsd[index].position;

	return 0;
}
#endif

#ifdef TOUCH_API_SURFACE
ssint tsapi_read_surface_state(u8 id, /*t9_point_status_t */void *sts)
{	
	const qtm_surface_contact_data_t *qtsf = &qtm_surface_cs_data1;
    t9_point_status_t *t9_sts = (t9_point_status_t *)sts;

	if (qtsf->qt_surface_status & TOUCH_ACTIVE) {
		t9_sts->status = MXT_T9_DETECT;
	
		if(qtsf->qt_surface_status & POSITION_CHANGE)
			t9_sts->status |= MXT_T9_MOVE;
		else
			t9_sts->status |= MXT_T9_PRESS;
	} else {
		t9_sts->status = 0;
	}

	t9_sts->pos.x = qtsf->v_position;
	t9_sts->pos.y = qtsf->h_position;;

	return 0;
}
#endif

u8 tsapi_t8_sensing_mode_translate(u8 mode, u8 rw)
{	
    if (rw == OP_READ) {
        switch (mode) {
            case NODE_SELFCAP:
                return MXT_T8_MEASALLOW_SELFPROX;
            case NODE_SELFCAP_SHIELD:
                return MXT_T8_MEASALLOW_SELFTCH;
            case NODE_MUTUAL:
                return MXT_T8_MEASALLOW_MUTUALTCH;
            case NODE_MUTUAL_4P:
                return MXT_T8_MEASALLOW_MUTUALTCH_4P;
            case NODE_MUTUAL_8P:
                return MXT_T8_MEASALLOW_MUTUALTCH_8P;
            default:
                return 0;
        }
    } else {
        if (mode & MXT_T8_MEASALLOW_SELFTCH) {
            return NODE_SELFCAP_SHIELD;
        } else if(mode & MXT_T8_MEASALLOW_MUTUALTCH) {
            if (mode == MXT_T8_MEASALLOW_MUTUALTCH_8P) {
                return NODE_MUTUAL_8P;
            } else if (mode == MXT_T8_MEASALLOW_MUTUALTCH_4P) {
                return NODE_MUTUAL_4P;
            } else {
                return NODE_MUTUAL;
            }
        } else if(mode & MXT_T8_MEASALLOW_SELFPROX) {
            return NODE_SELFCAP;
        }
        return 0;
    }
}

u16 tsapi_t6_get_sensor_base_ref(void)
{
	return SENSOR_BASE_REF_VALUE;
}

void tsapi_touch_suspend(bool suspend) 
{
	touch_suspend(suspend);
}

void tsapi_touch_inject_event(void)
{
	touch_inject_event();
}

bool tsapi_touch_state_idle(void)
{
	return (touch_state_idle() == 0);
}

bool tsapi_touch_state_sleep(void)
{
	return (touch_state_sleep() == 0);
}

uint8_t	tsapi_touch_sleep(void)
{
	return touch_sleep();
}

#ifdef MPTT_FUSE_CHECK
uint8_t tsapi_fuse_check(void)
{
	return fuse_check();
}
#endif

#ifdef OBJECT_T37_DEBUG_PLATFORM_INFO
const u8 *tsapi_get_signature_row_data(u8 *len_ptr)
{
	return get_signature_row_data(len_ptr);
}
#endif

#ifdef MPTT_FUSE_CHECK
const u8 *tsapi_get_fuse_data(u8 *len_ptr)
{
	return get_fuse_data(len_ptr);
}
#endif

bool tsapi_sensor_state_is_suspend(uint8_t node)
{
	return (get_sensor_state(node) == QTM_KEY_STATE_SUSPEND);
}

/* sensor key index to channel node mapping */
uint8_t tsapi_get_sensor_node_mapping(uint8_t sensor_node, int8_t lumped)
{
	return get_sensor_node_mapping(sensor_node, lumped);
}

/* channel node to sensor key index mapping */
uint8_t tsapi_get_channel_node_mapping(uint8_t channel_node)
{
	return get_channel_node_mapping(channel_node);
}