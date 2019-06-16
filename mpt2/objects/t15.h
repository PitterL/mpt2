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
	u8 cfg;	
} __attribute__ ((packed)) object_t15_t;

#include "txx.h"
typedef struct txx_data t15_data_t;

void object_t15_process(void);
ssint object_t15_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);

#define MXT_TOUCH_KEYARRAY_T15_INST 1
#define MXT_TOUCH_KEYARRAY_T15_RIDS 1

#endif /* T15_H_ */