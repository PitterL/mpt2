/*
 * t15.h
 *
 * Created: 6/9/2019 10:39:04 PM
 *  Author: A41450
 */ 


#ifndef T15_H_
#define T15_H_

#ifdef OBJECT_T15

typedef struct object_t15 {
	u8 ctrl;
	u8 xorigin;
	u8 yorigin;
	u8 xsize;
	u8 ysize;
	u8 akscfg;
	u8 blen;
	u8 tchthr;
	u8 tchdi;
	u8 tchhyst;
	u8 antitchdi;	
} __attribute__ ((packed)) object_t15_t;

/* MXT_TOUCH_KEYARRAY_T15 field */
#define MXT_T15_CTRL 0
#define MXT_T15_CTRL_ENABLE	BIT(0)
#define MXT_T15_CTRL_RPTEN	BIT(1)

/* MXT_TOUCH_KEYARRAY_T15 status */
#define MXT_T15_DETECT		BIT(7)

typedef union t15_button_status {
	u8 data[_INTEGRAL_BYTES];
	integer_size_t value;
}__attribute__ ((packed)) t15_button_status_t;

typedef struct object_t15_result {
	u8 status;
	t15_button_status_t keystate;
}__attribute__ ((packed)) object_t15_result_t;

#define MXT_TOUCH_KEYARRAY_T15_INST 2
#define MXT_TOUCH_KEYARRAY_T15_RIDS 1	// Must be 1

typedef struct t15_data {
	txx_data_t common;
	const /*qbutton_config_t*/void *btndef;
	object_t15_result_t button;
} t15_data_t;

ssint object_t15_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
void object_t15_start(u8 loaded);
void object_t15_data_sync(u8 rw);
void object_t15_report_status(u8 force);
bool object_t15_is_enabled(u8 inst_id);

ssint object_api_t15_set_button_status(/* Slot id */u8 id, u8 pressed);

/* api */
#define API_KEY_PARAMS_AKS_GROUP NAPI_KEY_PARAMS_AKS_GROUP
#define API_KEY_PARAMS_THRESHOLD NAPI_KEY_PARAMS_THRESHOLD
#define API_DEF_TOUCH_DET_INT NAPI_DEF_TOUCH_DET_INT
#define API_KEY_PARAMS_HYSTERESIS NAPI_KEY_PARAMS_HYSTERESIS
#define API_DEF_ANTI_TCH_DET_INT NAPI_DEF_ANTI_TCH_DET_INT
#define API_NODE_PARAMS_GAIN	NAPI_NODE_PARAMS_GAIN

#else	/* OBJECT_T15 */

#define MXT_TOUCH_KEYARRAY_T15_INST 0	/* Set T15 instance to ZERO if not compiled */
#define MXT_TOUCH_KEYARRAY_T15_RIDS 0

#endif	/* OBJECT_T15 */

#endif /* T15_H_ */