/*
 * t15.h
 *
 * Created: 6/9/2019 10:39:04 PM
 *  Author: A41450
 */ 


#ifndef T15_H_
#define T15_H_

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
	u8 rsv;	
} __attribute__ ((packed)) object_t15_t;

/* MXT_TOUCH_KEYARRAY_T15 status */
#define MXT_T15_DETECT		BIT(7)

#define BTN_COUNT_EACH_INSTANCE 32	//Limited max key counts in each instance to 32, if need more, object_t15_set_button_status()

typedef union t15_button_status {
	u8 data[BTN_COUNT_EACH_INSTANCE >> 3];
	u32 status;
}t15_button_status_t;

#define MXT_TOUCH_KEYARRAY_T15_INST 3
#define MXT_TOUCH_KEYARRAY_T15_RIDS 1	// Must be 1

typedef struct t15_data {
	txx_data_t common;
	const /*qbutton_config_t*/void *btndef;
	t15_button_status_t button;
} t15_data_t;

void object_t15_process(u8 rw);
ssint object_t15_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
void object_t15_start(u8 loaded);
void object_t15_process(u8 rw);
void object_t15_report_status(u8 force);

ssint object_t15_set_button_status(/* Slot id */u8 id, u8 pressed);
#endif /* T15_H_ */