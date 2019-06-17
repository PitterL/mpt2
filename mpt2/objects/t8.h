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
#ifdef	OBJECT_T8_MC_SC
	u8 measallow;
	u8 measidledef;
	u8 measactvdef;
	u8 refmode;
	u8 cfg;
#endif
} __attribute__ ((packed)) object_t8_t;

#define MXT_T8_MEASALLOW_MUTUALTCH	BIT(0)
#define MXT_T8_MEASALLOW_SELFTCH	BIT(1)
#define MXT_T8_MEASALLOW_HOVER	BIT(2)
#define MXT_T8_MEASALLOW_SELFPROX	BIT(3)

#include "txx.h"
typedef struct txx_data t8_data_t;

ssint object_t8_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
void object_t8_start(u8 loaded);
void object_t8_process(void);

#endif /* T8_H_ */