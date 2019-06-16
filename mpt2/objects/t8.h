/*
 * t8.h
 *
 * Created: 6/9/2019 10:33:10 AM
 *  Author: A41450
 */ 


#ifndef T8_H_
#define T8_H_

#include "../types.h"

/* T8 Memory Space */
typedef struct object_t8 {
	u8 chrgtime;
	u8 rsv;
	u8 tchdrift;
	u8 driftst;
	u8 tchautocal;
	u8 sync;
	u8 atchcalst;
	u8 atchcalsthr;
	u8 atchfrccalthr;
	u8 atchfrccalratio;
} __attribute__ ((packed)) object_t8_t;

#define MXT_T8_MEASALLOW_MUTUALTCH	BIT(0)
#define MXT_T8_MEASALLOW_SELFTCH	BIT(1)
#define MXT_T8_MEASALLOW_HOVER	BIT(2)
#define MXT_T8_MEASALLOW_SELFPROX	BIT(3)

#include "txx.h"
typedef struct txx_data t8_data_t;

void object_t8_process(void);
ssint object_t8_init(u8 rid,  const /*sensor_config_t*/void *cfg, void *mem, void *cb);

#endif /* T8_H_ */