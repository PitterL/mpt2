/*
 * t126.h
 *
 * Created: 6/9/2019 10:39:04 PM
 *  Author: A41450
 */ 


#ifndef T126_H_
#define T126_H_

#ifdef OBJECT_T126

typedef struct object_t126 {
	u8 ctrl;
	u8 node /*rsv*/;
	u8 gain;
	u8 threshold;
	u8 syncsperl;
	u8 driftcoef;
	u8 sigfiltcoef;
	u8 tunparam;
	u8 rsv2;
} __attribute__ ((packed)) object_t126_t;

/* MXT_SPT_LOWPOWERIDLECONFIG_T126 field */
#define MXT_T126_CTRL 0
#define MXT_T126_CTRL_ENABLE	BIT(0)
#define MXT_T126_CTRL_RPTEN	BIT(1)
#define MXT_T126_CTRL_RPTAUTOEN	BIT(2)
#define MXT_T126_CTRL_RPTTCHEN	BIT(3)
#define MXT_T126_CTRL_AUTOEN	BIT(4)
#define MXT_T126_CTRL_DBGEN	BIT(7)

/* MXT_SPT_LOWPOWERIDLECONFIG_T126 status */
#define MXT_T126_STATUS_IDLE	BIT(7)

typedef struct object_t126_result {
	u8 status;
	s16 delta;
}__attribute__ ((packed)) object_t126_result_t;

#define MXT_SPT_LOWPOWERIDLECONFIG_T126_RIDS 1

typedef struct t126_data {
	txx_data_t common;
	object_t126_result_t button;
} t126_data_t;

typedef enum wakeup_type {
	WK_REPORT_ALL,
	WK_POS_BREACH,
	WK_RSV_NEG_BREACH,
	WK_RSV_AUTO_ST,
	WK_RSV_AUTO_ENABLE,
	WK_FORCE,
	NUM_WK_TYPES
} wakeup_type_t;

#define WK_TYPE_MASK 0x7

ssint object_t126_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
void object_t126_start(u8 loaded);
void object_t126_data_sync(u8 rw);
void object_t126_report_status(u8 force);
bool object_api_t126_lowpower_mode_enabled(void);
void object_api_t126_force_waked(int16_t val);
void object_api_t126_breach_waked(int16_t val);
void object_api_t126_breach_sleep(void);
bool object_api_t126_node_skipped(u8 node);
u8 object_api_t126_get_low_power_node_mask(void);
ssint object_api_t126_is_low_power_node(u8 sensor_node);
bool object_api_t126_get_low_power_status(void);

#define API_DEF_QTM_AUTOSCAN_DRIFT_PERIOD_MS NAPI_DEF_QTM_AUTOSCAN_DRIFT_PERIOD_MS
#define API_DEF_QTM_AUTOSCAN_THRESHOLD NAPI_DEF_QTM_AUTOSCAN_THRESHOLD
#define API_DEF_QTM_AUTOSCAN_NODE NAPI_DEF_QTM_AUTOSCAN_NODE

#else	/* OBJECT_T126 */

#define MXT_SPT_LOWPOWERIDLECONFIG_T126_RIDS 0

#endif	/* OBJECT_T126 */

#endif /* T126_H_ */