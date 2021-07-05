/*
 * tslapi.h
 *
 * Created: 6/15/2019 2:37:52 PM
 *  Author: A41450
 */ 


#ifndef TSLAPI_H_
#define TSLAPI_H_

#include "include/types.h"

enum {
	API_DEF_TOUCH_MEASUREMENT_PERIOD_MS = 0,
	API_DEF_TOUCH_MEASUREMENT_IDLE_PERIOD_MS,
	API_DEF_TOUCH_ACTIVE_IDLE_TIMEOUT,
	API_DEF_SENSOR_TYPE,
	API_DEF_PTC_CAL_OPTION,
	API_DEF_PTC_INTERRUPT_PRIORITY,
	API_DEF_PTC_TAU_TARGET,
	API_DEF_PTC_CAL_AUTO_TUNE,
	API_DEF_SEL_FREQ_INIT,

	API_NUM_CHANNELS,
	API_NUM_SENSORS,
	API_NODE_PARAMS_CSD,
	API_NODE_PARAMS_RESISTOR_PRESCALER,
	API_NODE_PARAMS_GAIN,
	API_NODE_PARAMS_ADC_OVERSAMPLING,

	API_KEY_PARAMS_THRESHOLD,
	API_KEY_PARAMS_HYSTERESIS,
	API_KEY_PARAMS_AKS_GROUP,

	API_DEF_TOUCH_DET_INT,
	API_DEF_ANTI_TCH_DET_INT,
	API_DEF_ANTI_TCH_RECAL_THRSHLD,
	API_DEF_TCH_DRIFT_RATE,
	API_DEF_ANTI_TCH_DRIFT_RATE,
	API_DEF_DRIFT_HOLD_TIME,
	API_DEF_REBURST_MODE,
	API_DEF_MAX_ON_DURATION,
	
	API_DEF_TOUCH_DRIFT_PERIOD_MS,
	API_DEF_QTM_AUTOSCAN_THRESHOLD,
	API_DEF_QTM_AUTOSCAN_NODE,
	
	API_NUM_SLIDERS,
	API_SLIDER_START_KEY,
	API_SLIDER_NUM_KEYS,
	API_SLIDER_RESOL_DB,
	//API_SLIDER_FILT_CFG,
	API_SLIDER_POS_HYST,
	API_SLIDER_MIN_CONTACT,
	
	API_SURFACE_CS_START_KEY_H,
	API_SURFACE_CS_NUM_KEYS_H,
	API_SURFACE_CS_START_KEY_V,
	API_SURFACE_CS_NUM_KEYS_V,
	API_SURFACE_CS_RESOL_DB,
	API_SURFACE_CS_FILT_CFG,
	API_SURFACE_CS_POS_HYST,
	API_SURFACE_CS_MIN_CONTACT,
	
	API_TAP_RELEASE_TIMEOUT,
	API_TAP_HOLD_TIMEOUT,
	API_SWIPE_TIMEOUT,
	API_HORIZONTAL_SWIPE_DISTANCE_THRESHOLD,
	API_VERTICAL_SWIPE_DISTANCE_THRESHOLD,
	API_TAP_AREA,
	API_SEQ_TAP_DIST_THRESHOLD,
	API_EDGE_BOUNDARY,
	API_WHEEL_POSTSCALER,
	API_WHEEL_START_QUADRANT_COUNT,
	API_WHEEL_REVERSE_QUADRANT_COUNT,
	API_DEF_GESTURE_TIME_BASE_MS,
	
	API_NUM_FREQ_STEPS,
	API_DEF_MEDIAN_FILTER_FREQUENCIES,
	API_DEF_FREQ_AUTOTUNE_ENABLE,
	API_FREQ_AUTOTUNE_MAX_VARIANCE,
	API_FREQ_AUTOTUNE_COUNT_IN,
	
    API_NODE_DELTA_VALUE,
    API_NODE_SIGNAL_VALUE,
    API_NODE_REFERENCE_VALUE,
	API_NODE_COMPCAP_VALUE,
};

enum {SENSOR_SELFCAP, SENSOR_SELFCAP_SHIELD, SENSOR_MUTUAL, SENSOR_MUTUAL_4P, SENSOR_MUTUAL_8P, NUM_SENSOR_TYPES};

typedef ssint (*cb_qlib_sync_op_t)(u8 type, /*read or write */void *buf, size_t size, u8 index, u8 rw);
typedef void (*cb_qlib_calibrate_t)(void);
typedef bool (*cb_qlib_state_idle_t)(void);
typedef void (*cb_qlib_suspend_t)(bool);

typedef union {
	struct {
		u8 origin;
		u8 size;
	};
	u16 value;
} nodes_desc_t;

typedef struct qbutton_config {
	nodes_desc_t node;
} qbutton_config_t;

enum {
	NODE_X,
	NODE_Y,
	NUM_NODE_2D	
};

typedef struct qsurface_config {
	nodes_desc_t nodes[NUM_NODE_2D];
	u8 resolution_bit;
	u16 resolution_max;
} qsurface_config_t;

typedef struct qtouch_params {
	u8 max_prsc_div;
	u8 max_gain;
	u8 max_filter_count;
	u8 max_resl;
}qtouch_params_t;

typedef struct qtouch_config {
	nodes_desc_t matrix_nodes[NUM_NODE_2D];
	u8 maxtrix_channel_count;	//	logical channels count(actual channels for selfcap or nodes for mutualcap)

	qbutton_config_t *buttons;
	u8 num_button;
	u8 num_button_channel_count;
	
	qsurface_config_t *surface_sliders;
	u8 num_surfaces_slider;
	u8 num_surfaces;
	u8 num_slider;
	u8 num_surfaces_slider_channel_count;
	
	qtouch_params_t params;
} qtouch_config_t;

#define QTOUCH_CONFIG_VAL(_p, _n) (((qtouch_config_t *)(_p))->_n)
#define QTOUCH_PARAMS_VAL(_p, _n) (((qtouch_config_t *)(_p))->params._n)

#define QT_MATRIX_X_ST(_p) (((qtouch_config_t *)(_p))->matrix_nodes[NODE_X].origin)
#define QT_MATRIX_X_SIZE(_p) (((qtouch_config_t *)(_p))->matrix_nodes[NODE_X].size)
#define QT_MATRIX_Y_ST(_p) (((qtouch_config_t *)(_p))->matrix_nodes[NODE_Y].origin)
#define QT_MATRIX_Y_SIZE(_p) (((qtouch_config_t *)(_p))->matrix_nodes[NODE_Y].size)

typedef struct qtouch_api_callback {
	const cb_qlib_sync_op_t sync;
	const cb_qlib_calibrate_t calibrate;
	const cb_qlib_state_idle_t idle;
	const cb_qlib_suspend_t suspend;
} qtouch_api_callback_t;
#define QTOUCH_API_CALLBACK(_cb, _fn) (((qtouch_api_callback_t *)(_cb))->_fn)

typedef	void (*cb_mpt_api_reset_t)(void);
typedef	void (*cb_mpt_api_calibrate_t)(void);
typedef	ssint (*cb_mpt_api_backup_t)(/*data_crc24_t*/ void *crc_ptr);
typedef	void (*cb_mpt_api_report_all_t)(void);
typedef	void (*cb_mpt_api_get_config_crc_t)(/*data_crc24_t*/void *crc_ptr);
typedef	void (*cb_mpt_api_assert_irq_t)(u8 assert, bool retrigger);

#ifdef OBJECT_T5	
typedef	ssint (*cb_mpt_api_write_message_t)(const /*object_t5_t*/void *msg);
#endif

typedef struct mpt_api_callback {
	const qtouch_api_callback_t * qtapi;
#ifdef OBJECT_T6
	cb_mpt_api_reset_t reset;
	cb_mpt_api_calibrate_t calibrate;
	cb_mpt_api_backup_t backup;	
	cb_mpt_api_report_all_t report_all;
	cb_mpt_api_get_config_crc_t get_config_crc;
	cb_mpt_api_assert_irq_t assert_irq;
#endif
#ifdef OBJECT_T5	
	cb_mpt_api_write_message_t write_message;
#endif
} mpt_api_callback_t;

#define MPT_API_CALLBACK(_cb, _fn) (((mpt_api_callback_t *)(_cb))->_fn)
#define MPT_QTAPI_CALLBACK(_cb, _fn) ((((mpt_api_callback_t *)(_cb))->qtapi)->_fn)

enum {
	OP_WRITE = 0,
	OP_READ,
};

typedef struct cap_sample_value{
	u16 reference;
	u16 signal;
	u16 comcap;
	u16 cccap;
} cap_sample_value_t;

ssint tsapi_config_op(u8 type, void *buf, size_t size, u8 index, u8 rw);
u8 tsapi_read_group_config_byte(u8 type, u8 index);
u8 tsapi_read_config_byte(u8 type);
void tsapi_calibrate(void);
u8 tsapi_get_chip_state(void);
ssint tsapi_read_ref_signal_cap(u8 index, cap_sample_value_t *cval);
ssint tsapi_read_button_state(u8 index);
ssint tsapi_read_slider_state(u8 index, /*t9_point_status_t */ void *sts);
ssint tsapi_read_surface_state(u8 id,/*t9_point_status_t */void *sts);
u8 tsapi_t8_sensing_mode_translate(u8 mode, u8 rw);

u16 tsapi_t6_get_sensor_base_ref(void);
void tsapi_touch_suspend(bool suspend);
void tsapi_touch_inject_event(void);
bool tsapi_touch_state_idle(void);

#endif /* TSLAPI_H_ */