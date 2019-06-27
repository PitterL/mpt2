/*
 * tslapi.h
 *
 * Created: 6/15/2019 2:37:52 PM
 *  Author: A41450
 */ 


#ifndef TSLAPI_H_
#define TSLAPI_H_

#include "types.h"

enum {
	DEF_TOUCH_MEASUREMENT_PERIOD_MS = 0,
	DEF_SENSOR_TYPE,
	DEF_PTC_CAL_OPTION,
	DEF_PTC_INTERRUPT_PRIORITY,
	DEF_PTC_TAU_TARGET,
	DEF_PTC_CAL_AUTO_TUNE,
	DEF_SEL_FREQ_INIT,

	NUM_CHANNELS,
	NUM_SENSORS,
	NODE_PARAMS_CSD,
	NODE_PARAMS_RESISTOR_PRESCALER,
	NODE_PARAMS_GAIN,
	NODE_PARAMS_ADC_OVERSAMPLING,

	KEY_PARAMS_THRESHOLD,
	KEY_PARAMS_HYSTERESIS,
	KEY_PARAMS_AKS_GROUP,

	DEF_TOUCH_DET_INT,
	DEF_ANTI_TCH_DET_INT,
	DEF_ANTI_TCH_RECAL_THRSHLD,
	DEF_TCH_DRIFT_RATE,
	DEF_ANTI_TCH_DRIFT_RATE,
	DEF_DRIFT_HOLD_TIME,
	DEF_REBURST_MODE,
	DEF_MAX_ON_DURATION,
	
	SURFACE_CS_START_KEY_H,
	SURFACE_CS_NUM_KEYS_H,
	SURFACE_CS_START_KEY_V,
	SURFACE_CS_NUM_KEYS_V,
	SURFACE_CS_RESOL_DB,
	SURFACE_CS_FILT_CFG,
	SURFACE_CS_POS_HYST,
	SURFACE_CS_MIN_CONTACT,
	
	TAP_RELEASE_TIMEOUT,
	TAP_HOLD_TIMEOUT,
	SWIPE_TIMEOUT,
	HORIZONTAL_SWIPE_DISTANCE_THRESHOLD,
	VERTICAL_SWIPE_DISTANCE_THRESHOLD,
	TAP_AREA,
	SEQ_TAP_DIST_THRESHOLD,
	EDGE_BOUNDARY,
	WHEEL_POSTSCALER,
	WHEEL_START_QUADRANT_COUNT,
	WHEEL_REVERSE_QUADRANT_COUNT,
	DEF_GESTURE_TIME_BASE_MS,
	
	NUM_FREQ_STEPS,
	DEF_MEDIAN_FILTER_FREQUENCIES,
	DEF_FREQ_AUTOTUNE_ENABLE,
	FREQ_AUTOTUNE_MAX_VARIANCE,
	FREQ_AUTOTUNE_COUNT_IN,
};

typedef ssint (*cb_writeback_t)(u8 type, /*read or write */void *buf, size_t size, u8 index);
typedef ssint (*cb_sync_op_t)(u8 type, /*read or write */void *buf, size_t size, u8 index, u8 rw);
typedef void (*cb_calibrate_t)(void);

typedef union {
	struct {
		u8 origin: 4;
		u8 size: 4;
	};
	u8 value;
} nodes_desc_t;

typedef struct qbutton_config {
	nodes_desc_t node;
} qbutton_config_t;

typedef struct qsurface_config {
	nodes_desc_t xnode;
	nodes_desc_t ynode;
	u8 resolution_bit;
} qsurface_config_t;

typedef struct qtouch_config {
	u8 matrix_xsize;
	u8 matrix_ysize;
	u8 num_sensor_params;
	qbutton_config_t *buttons;
	u8 num_buttons;
	qsurface_config_t *surface_sliders;
	u8 num_surfaces_slider;
} qtouch_config_t;

#define QTOUCH_CONFIG_VAL(_p, _n) (((qtouch_config_t *)(_p))->_n)

typedef struct qtouch_api_callback {
	cb_writeback_t write;
	cb_writeback_t read;
	cb_sync_op_t sync;
	cb_calibrate_t calibrate;
} qtouch_api_callback_t;
#define QTOUCH_API_CALLBACK(_cb, _fn) (((qtouch_api_callback_t *)(_cb))->_fn)

typedef struct mpt_api_callback {
	const qtouch_api_callback_t *qtapi;
	void (*reset)(void);
	void (*calibrate)(void);
	ssint (*backup)(void);
	void (*report_all)(void);
	void (*cb_get_config_crc)(/*data_crc24_t*/void *crc_ptr);
	ssint (*cb_write_message)(const /*object_t5_t*/void *msg);
	ssint (*cb_object_write)(u8 regid, u8 instance, u16 offset, const u8 *ptr, u8 size);
} mpt_api_callback_t;

#define MPT_API_CALLBACK(_cb, _fn) (((mpt_api_callback_t *)(_cb))->_fn)
#define MPT_QTAPI_CALLBACK(_cb, _fn) ((((mpt_api_callback_t *)(_cb))->qtapi)->_fn)

#endif /* TSLAPI_H_ */