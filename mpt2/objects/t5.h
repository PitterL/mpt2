/*
 * t5.h
 *
 * Created: 6/9/2019 10:25:04 AM
 *  Author: A41450
 */ 


#ifndef T5_H_
#define T5_H_

#include "../types.h"

/* T5 Memory space */
typedef struct object_t5 {
	u8 reportid;
	u8 data[5];
#ifdef SUPPORT_MESSAGE_CRC
	u8 crc;
#endif
} __attribute__ ((packed)) object_t5_t;

typedef struct t5_data {
	object_t5_t *mem;
} t5_data_t;

/* MXT_GEN_MESSAGE_T5 object */
#define MXT_RPTID_NOMSG		0xff

int object_t5_init(u8 rid,  const /*sensor_config_t*/void *cfg, void *mem);

#endif /* T5_H_ */