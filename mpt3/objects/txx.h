/*
 * txx.h
 *
 * Created: 6/9/2019 4:09:54 PM
 *  Author: A41450
 */ 


#ifndef TXX_H_
#define TXX_H_

#include <common/include/types.h>

/* Object types */
#define MXT_DEBUG_DIAGNOSTIC_T37  37
#define MXT_GEN_MESSAGE_T5    5
#define MXT_GEN_COMMAND_T6    6
#define MXT_GEN_POWER_T7    7
#define MXT_GEN_ACQUIRE_T8    8
#define MXT_GEN_DATASOURCE_T53    53
#define MXT_TOUCH_MULTI_T9    9
#define MXT_TOUCH_KEYARRAY_T15    15
#define MXT_TOUCH_PROXIMITY_T23    23
#define MXT_TOUCH_PROXKEY_T52    52
#define MXT_PROCI_GRIPFACE_T20    20
#define MXT_PROCG_NOISE_T22    22
#define MXT_PROCI_ONETOUCH_T24    24
#define MXT_PROCI_TWOTOUCH_T27    27
#define MXT_PROCI_GRIP_T40    40
#define MXT_PROCI_PALM_T41    41
#define MXT_PROCI_TOUCHSUPPRESSION_T42  42
#define MXT_PROCI_STYLUS_T47    47
#define MXT_PROCG_NOISESUPPRESSION_T48  48
#define MXT_SPT_COMMSCONFIG_T18    18
#define MXT_SPT_GPIOPWM_T19    19
#define MXT_SPT_SELFTEST_T25    25
#define MXT_SPT_CTECONFIG_T28    28
#define MXT_SPT_USERDATA_T38    38
#define MXT_SPT_DIGITIZER_T43    43
#define MXT_SPT_MESSAGECOUNT_T44  44
#define MXT_SPT_CTECONFIG_T46    46
#define MXT_SPT_DYNAMICCONFIGURATIONCONTAINER_T71 71
#define MXT_PROCI_SYMBOLGESTUREPROCESSOR  92
#define MXT_PROCI_TOUCHSEQUENCELOGGER  93
#define MXT_TOUCH_MULTITOUCHSCREEN_T100 100
#define MXT_SPT_AUXTOUCHCONFIG_T104 104
#define MXT_SPT_SELFCAPCONFIG_T111 111
#define MXT_PROCI_ACTIVESTYLUS_T107  107
#define MXT_SPT_SELFCAPGLOBALCONFIG_T109 109
#define MXT_SPT_LOWPOWERIDLECONFIG_T126 126

enum {
  NAPI_DEF_TOUCH_MEASUREMENT_PERIOD_MS = 0,
  NAPI_DEF_TOUCH_MEASUREMENT_IDLE_PERIOD_MS,
  NAPI_DEF_TOUCH_ACTIVE_IDLE_TIMEOUT,
  NAPI_DEF_SENSOR_TYPE,
  NAPI_DEF_PTC_CAL_OPTION,
  NAPI_DEF_PTC_INTERRUPT_PRIORITY,
  NAPI_DEF_PTC_TAU_TARGET,
  NAPI_DEF_PTC_CAL_AUTO_TUNE,
  NAPI_DEF_SEL_FREQ_INIT,

  NAPI_NUM_CHANNELS,
  NAPI_NUM_SENSORS,
  NAPI_NODE_PARAMS_PRE_CHARGE,
  NAPI_NODE_PARAMS_CSD,
  NAPI_NODE_PARAMS_PRECHARGE,
  NAPI_NODE_PARAMS_RESISTOR_PRESCALER,
  NAPI_NODE_PARAMS_GAIN,
  NAPI_NODE_PARAMS_ADC_OVERSAMPLING,

  NAPI_KEY_PARAMS_THRESHOLD,
  NAPI_KEY_PARAMS_HYSTERESIS,
  NAPI_KEY_PARAMS_AKS_GROUP,

  NAPI_DEF_TOUCH_DET_INT,
  NAPI_DEF_ANTI_TCH_DET_INT,
  NAPI_DEF_ANTI_TCH_RECAL_THRSHLD,
  NAPI_DEF_TCH_DRIFT_RATE,
  NAPI_DEF_ANTI_TCH_DRIFT_RATE,
  NAPI_DEF_DRIFT_HOLD_TIME,
  NAPI_DEF_REBURST_MODE,
  NAPI_DEF_MAX_ON_DURATION,
  
  NAPI_DEF_QTM_AUTOSCAN_DRIFT_PERIOD_MS,
  NAPI_DEF_QTM_AUTOSCAN_THRESHOLD,
  NAPI_DEF_QTM_AUTOSCAN_DRIFT_TARGET_NODE,
  NAPI_DEF_QTM_AUTOSCAN_NODE,
  NAPI_DEF_QTM_AUTOSCAN_TUNING_PARAM,
  NAPI_DEF_QTM_AUTOSCAN_CSD,
  NAPI_DEF_QTM_AUTOSCAN_RESISTOR_PRESCALER,
  NAPI_DEF_QTM_AUTOSCAN_GAIN,
  NAPI_DEF_QTM_AUTOSCAN_ADC_OVERSAMPLING,

  NAPI_NUM_SLIDERS,
  NAPI_SLIDER_START_KEY,
  NAPI_SLIDER_NUM_KEYS,
  NAPI_SLIDER_RESOL_DB,
  //NAPI_SLIDER_FILT_CFG,
  NAPI_SLIDER_POS_HYST,
  NAPI_SLIDER_MIN_CONTACT,
  
  NAPI_SURFACE_CS_START_KEY_H,
  NAPI_SURFACE_CS_NUM_KEYS_H,
  NAPI_SURFACE_CS_START_KEY_V,
  NAPI_SURFACE_CS_NUM_KEYS_V,
  NAPI_SURFACE_CS_RESOL_DB,
  NAPI_SURFACE_CS_FILT_CFG,
  NAPI_SURFACE_CS_POS_HYST,
  NAPI_SURFACE_CS_MIN_CONTACT,

  NAPI_TAP_RELEASE_TIMEOUT,
  NAPI_TAP_HOLD_TIMEOUT,
  NAPI_SWIPE_TIMEOUT,
  NAPI_HORIZONTAL_SWIPE_DISTANCE_THRESHOLD,
  NAPI_VERTICAL_SWIPE_DISTANCE_THRESHOLD,
  NAPI_TAP_AREA,
  NAPI_SEQ_TAP_DIST_THRESHOLD,
  NAPI_EDGE_BOUNDARY,
  NAPI_WHEEL_POSTSCALER,
  NAPI_WHEEL_START_QUADRANT_COUNT,
  NAPI_WHEEL_REVERSE_QUADRANT_COUNT,
  NAPI_DEF_GESTURE_TIME_BASE_MS,
  
  NAPI_NUM_FREQ_STEPS,
  NAPI_DEF_MEDIAN_FILTER_FREQUENCIES,
  NAPI_DEF_FREQ_AUTOTUNE_ENABLE,
  NAPI_FREQ_AUTOTUNE_MAX_VARIANCE,
  NAPI_FREQ_AUTOTUNE_COUNT_IN,

  NAPI_NODE_DELTA_VALUE,
  NAPI_NODE_SIGNAL_VALUE,
  NAPI_NODE_REFERENCE_VALUE,
  NAPI_NODE_COMPCAP_VALUE,

  NAPI_CHANNEL_BASE_REF,
  NAPI_SELFTEST
};

typedef struct txx_data {
  u8 rid;
  void *mem;
  
  const /*mpt_api_callback_t*/void *cb;
  const /*qtouch_config_t*/void *def;
} txx_data_t;

typedef struct txx_cb_param {
  u8 type;
  void *src; 
  u8 size;
} txx_cb_param_t;

enum {SENSOR_SELFCAP, SENSOR_SELFCAP_SHIELD, SENSOR_MUTUAL, SENSOR_MUTUAL_4P, SENSOR_MUTUAL_8P, NUM_SENSOR_TYPES};

#ifdef OBJECT_T37_DEBUG_PLATFORM_INFO
typedef const u8 *(*cb_get_signature_row)(u8 *);
#endif
#ifdef MPTT_FUSE_CHECK
typedef const u8 *(*cb_get_fuse_data)(u8 *);
#endif
#ifdef OBJECT_T15_USE_STATE_CB
typedef void (*cb_button_state_change)(u8, u32);
#endif
#ifdef OBJECT_T9_USE_STATE_CB
typedef void (*cb_touch_state_change)(u8 inst, /* Slot id */u8 id, u8 status, u16 x, u16 y, u16 max_resol);
#endif

typedef uint8_t (*cb_sensor_node_mapping)(uint8_t sensor_node, uint8_t group);
typedef uint8_t (*cb_channel_node_mapping)(uint8_t channel_node);

typedef union {
	struct {
		u8 origin;
		u8 size;
	};
	u16 value;
} nodes_desc_t;

typedef struct qbutton_config {
	nodes_desc_t node;
	
#ifdef OBJECT_T15_USE_STATE_CB
	cb_button_state_change set_button_state;
#endif
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
	
#ifdef OBJECT_T9_USE_STATE_CB
	cb_touch_state_change set_touch_state;
#endif
} qsurface_config_t;

typedef struct qtouch_params {
	u8 max_prsc_div;
	u8 max_gain;
	u8 max_filter_count;
	u8 max_resl;
}qtouch_params_t;

typedef struct chip_info_cb {
#ifdef OBJECT_T37_DEBUG_PLATFORM_INFO
	cb_get_signature_row get_sigrow;
#endif
#ifdef MPTT_FUSE_CHECK
	cb_get_fuse_data get_fuse;
#endif
} chip_info_cb_t;

typedef struct chip_node_map_cb {
	cb_sensor_node_mapping to_channel;
	cb_channel_node_mapping to_sensor; 
} chip_node_map_cb_t;

typedef struct qtouch_config {
	nodes_desc_t matrix_nodes[NUM_NODE_2D];

  u8 channel_count; // channel node count
	u8 sensor_count;	//	sensor(key) count

#ifdef OBJECT_T15
	qbutton_config_t *buttons;
	u8 num_button;
	u8 num_button_channel_count;
#endif

#ifdef OBJECT_T9
	qsurface_config_t *surface_sliders;
	u8 num_surfaces_slider;
	u8 num_surfaces;
	u8 num_slider;
	u8 num_surfaces_slider_channel_count;
#endif

	qtouch_params_t params;

#ifdef OBJECT_T37_DEBUG_PLATFORM_INFO
	chip_info_cb_t chip_cb;
#endif
	chip_node_map_cb_t map_cb;
} qtouch_config_t;

#define QTOUCH_CONFIG_VAL(_p, _n) (((qtouch_config_t *)(_p))->_n)
#define QTOUCH_PARAMS_VAL(_p, _n) (((qtouch_config_t *)(_p))->params._n)
#define QTOUCH_MAP_CALL(_p, _n) (((qtouch_config_t *)(_p))->map_cb._n)

#define QT_MATRIX_X_ST(_p) (((qtouch_config_t *)(_p))->matrix_nodes[NODE_X].origin)
#define QT_MATRIX_X_SIZE(_p) (((qtouch_config_t *)(_p))->matrix_nodes[NODE_X].size)
#define QT_MATRIX_Y_ST(_p) (((qtouch_config_t *)(_p))->matrix_nodes[NODE_Y].origin)
#define QT_MATRIX_Y_SIZE(_p) (((qtouch_config_t *)(_p))->matrix_nodes[NODE_Y].size)

typedef s16 sig_t;
typedef s16 ref_t;
typedef s16 dlt_t;
typedef u16 cap_t;
typedef u16 comp_t;

typedef struct cap_sample_value{
	ref_t reference;
	sig_t signal;
	comp_t comcap;
	cap_t cccap;
} cap_sample_value_t;

typedef ssint (*cb_qlib_sync_op_t)(u8 type, /*read or write */void *buf, u8 size, u8 index, u8 rw);
typedef void (*cb_qlib_calibrate_t)(void);
typedef bool (*cb_qlib_state_idle_t)(void);
typedef void (*cb_qlib_suspend_t)(bool);

typedef struct qtouch_api_callback {
  const cb_qlib_sync_op_t sync;
  const cb_qlib_calibrate_t calibrate;
  // const cb_qlib_state_idle_t idle;
  // const cb_qlib_suspend_t suspend;
} qtouch_api_callback_t;
#define QTOUCH_API_CALLBACK(_cb, _fn) (((qtouch_api_callback_t *)(_cb))->_fn)

typedef  void (*cb_mpt_api_reset_t)(void);
typedef  void (*cb_mpt_api_calibrate_t)(void);
typedef  ssint (*cb_mpt_api_backup_t)(/*data_crc24_t*/ void *crc_ptr, u8 ccp);
typedef  void (*cb_mpt_api_report_all_t)(void);
typedef  void (*cb_mpt_api_get_config_crc_t)(/*data_crc24_t*/void *crc_ptr);
typedef  void (*cb_mpt_api_assert_irq_t)(u8 assert, bool retrigger);
#ifdef OBJECT_T5  
typedef  ssint (*cb_mpt_api_write_message_t)(const /*object_t5_t*/void *msg);
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
  OP_CMD,
};

#include "t44.h"
#include "t5.h"
#include "t6.h"
#include "t7.h"
#include "t8.h"
#include "t9.h"
#include "t15.h"
#include "t18.h"
#include "t25.h"
#include "t37.h"
#include "t38.h"
#include "t104.h"
#include "t109.h"
#include "t111.h"
#include "t126.h"

ssint object_txx_init(txx_data_t *ptr, u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
void object_txx_readback(const txx_data_t *ptr, const txx_cb_param_t *params, u8 count, u8 index);
void object_txx_writeback(const txx_data_t *ptr, const txx_cb_param_t *params, u8 count, u8 index);
void object_txx_op(const txx_data_t *ptr, const txx_cb_param_t *params, u8 count, u8 index, u8 rw);
ssint object_sync(const txx_data_t *ptr, u8 type, void *buf, u8 size, u8 index, u8 rw);
ssint object_cmd(const txx_data_t *ptr, u8 type, void *buf, u8 size, u8 index);
ssint object_read8(const txx_data_t *ptr, u8 type, u8 *buf, u8 index);
ssint object_read16(const txx_data_t *ptr, u8 type, u16 *buf, u8 index);
void object_txx_report_msg(const txx_data_t *ptr, const void *data, u8 size);
bool object_ts_state_idle(const txx_data_t *ptr);
void object_ts_suspend(const txx_data_t *ptr, bool suspend);

#endif /* TXX_H_ */