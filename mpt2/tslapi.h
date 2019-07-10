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
	
	SLIDER_START_KEY,
	SLIDER_NUM_KEYS,
	SLIDER_RESOL_DB,
	//SLIDER_FILT_CFG,
	SLIDER_POS_HYST,
	SLIDER_MIN_CONTACT,
	
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
	u8 maxtrix_channel_count;

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
	//cb_writeback_t write;
	//cb_writeback_t read;
	cb_sync_op_t sync;
	cb_calibrate_t calibrate;
} qtouch_api_callback_t;
#define QTOUCH_API_CALLBACK(_cb, _fn) (((qtouch_api_callback_t *)(_cb))->_fn)

typedef struct mpt_api_callback {
	const qtouch_api_callback_t *qtapi;

#ifdef OBJECT_T6	
	void (*reset)(void);
	void (*calibrate)(void);
	ssint (*backup)(void);
	void (*report_all)(void);
	void (*cb_get_config_crc)(/*data_crc24_t*/void *crc_ptr);
	void (*cb_assert_irq)(u8 assert, bool retrigger);
#endif
#ifdef OBJECT_T5	
	ssint (*cb_write_message)(const /*object_t5_t*/void *msg);
#endif
#ifdef OBJECT_WRITEBACK
	//ssint (*cb_object_write)(u8 regid, u8 instance, u16 offset, const u8 *ptr, u8 size);
#endif
} mpt_api_callback_t;

#define MPT_API_CALLBACK(_cb, _fn) (((mpt_api_callback_t *)(_cb))->_fn)
#define MPT_QTAPI_CALLBACK(_cb, _fn) ((((mpt_api_callback_t *)(_cb))->qtapi)->_fn)

enum {
	OP_WRITE = 0,
	OP_READ,
};

/* 
	cc value formula:
		(val & 0x0F)*0.00675 + ((val >> 4) & 0x0F)*0.0675 + ((val >> 8) & 0x0F)*0.675 + ((val >> 12) & 0x3) * 6.75
	Here, multiply 1000 for calculation:
*/

// 7 = 4 + 2 + 1 = 2^2 + 2^1 + 2^0
#define MUL_7(_v0) (((_v0) << 2) + ((_v0) << 1) + (_v0))

// 68 = 64 + 4 = 2^6 + 2^2
#define MUL_68(_v1) (((_v1) << 6) + ((_v1) << 2))

// 675 = 512 + 128 + 32 + 4 - 1 = 2^9 + 2^7 + 2^5 + 2^2 - 2^0
#define MUL_675(_v2) (((_v2) << 9) + ((_v2) << 7) + ((_v2) << 5) + ((_v2) << 2) - (_v2))

// 6750 = 4096 + 2048 + 512 + 64 + 32 - 2 = 2^12 + 2^11 + 2^9 + 2^6 + 2^5 - 2^1
#define MUL_6750(_v3) (((_v3) << 12) + ((_v3) << 11) + ((_v3) << 9) + ((_v3) << 6) + ((_v3) << 5) - ((_v3) << 2))

//#define CALCULATE_CAP(_v) (MUL_7((_v) & 0x0F) + MUL_68(((_v) >> 4) & 0x0F) + MUL_675(((_v) >> 8) & 0x0F) + MUL_6750(((_v) >> 12) & 0x3))
#define CALCULATE_CAP(_v) (((_v) & 0x0F) * 7 + (((_v) >> 4) & 0x0F) * 68 + (((_v) >> 8) & 0x0F) *675 + (((_v) >> 12) & 0x3) * 6750)

#define SENSOR_BASE_REF_VALUE 512

#endif /* TSLAPI_H_ */