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

typedef struct txx_data t18_data_t;

void object_t18_process(u8 rw);
ssint object_t18_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
bool object_t18_check_retrigger(void);

#endif /* T18_H_ */