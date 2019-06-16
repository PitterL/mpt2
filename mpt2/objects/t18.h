/*
 * t18.h
 *
 * Created: 6/9/2019 10:44:33 AM
 *  Author: A41450
 */ 


#ifndef T18_H_
#define T18_H_

#include "../types.h"

/* MXT_SPT_COMMSCONFIG_T18 */
#define MXT_COMMS_CTRL		0
#define MXT_COMMS_CMD		1

#define MXT_COMMS_CTRL_RETRIGEN      BIT(6)

/* T18 Memory Space */
typedef struct object_t18 {
	u8 ctrl;
	u8 cmd;
} __attribute__ ((packed)) object_t18_t;

#include "txx.h"
typedef struct txx_data t18_data_t;

void object_t18_process(void);
ssint object_t18_init(u8 rid,  const /*sensor_config_t*/void *cfg, void *mem, void *cb);
bool object_t18_check_retrigger(void);

#endif /* T18_H_ */