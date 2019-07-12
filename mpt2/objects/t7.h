/*
 * t7.h
 *
 * Created: 6/9/2019 10:30:19 AM
 *  Author: A41450
 */ 


#ifndef T7_H_
#define T7_H_

#include "../types.h"

/* MXT_GEN_POWER_T7 field */
struct t7_config {
	u8 idle;
	u8 active;
}  __attribute__ ((packed));

#define MXT_POWER_CFG_RUN		0
#define MXT_POWER_CFG_DEEPSLEEP		1

/* T7 Memory space */
typedef struct object_t7 {
	u8 idleacqint;
	u8 actvaqint;
	u8 actv2idleto;
	u8 cfg;
} __attribute__ ((packed)) object_t7_t;

#define MXT_T7_CFG_IDLEPIPEEN BIT(0)
#define MXT_T7_CFG_ACTVPIPEEN BIT(0)
#define MXT_T7_CFG_OVFRPTSUP BIT(6)
#define MXT_T7_CFG_INITACTV BIT(0)

typedef struct txx_data t7_data_t;

void object_t7_data_sync(u8 rw);
ssint object_t7_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
void object_t7_start(u8 loaded);

ssint object_t7_report_overflow(void);

#endif /* T7_H_ */