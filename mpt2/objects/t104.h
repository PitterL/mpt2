/*
 * t104.h
 *
 * Created: 6/12/2019 2:17:06 PM
 *  Author: A41450
 */ 


#ifndef T104_H_
#define T104_H_

# define MXT_T104_CTRL 0

#define MXT_T104_CTRL_ENABLE BIT(0)

typedef struct object_t104 {
	u8 ctrl;
	u8 xgain;
	u8 xtchthr;
	u8 xtchhyst;
	u8 xintthr;
	u8 xinthyst;
	u8 ygain;
	u8 ytchthr;
	u8 ytchhyst;
	u8 yintthr;
	u8 yinthyst;
} object_t104_t;

#include "txx.h"
typedef struct txx_data t104_data_t;

void object_t104_process(void);
ssint object_t104_init(u8 rid,  const /*sensor_config_t*/void *cfg, void *mem, void *cb);

#endif /* T104_H_ */