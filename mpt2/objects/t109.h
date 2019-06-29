/*
 * t109.h
 *
 * Created: 6/16/2019 2:48:23 AM
 *  Author: A41450
 */ 


#ifndef T109_H_
#define T109_H_

typedef struct object_t109 {
	u8 ctrl;
	u8 rsv0;
	u8 cmdonreset;
	u8 cmd;
	u8 rsv1[4];
	u8 tunecfg;
} object_t109_t;

#include "txx.h"
typedef struct txx_data t109_data_t;

ssint object_t109_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);

#endif /* T109_H_ */