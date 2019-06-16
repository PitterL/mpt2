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
	u8 data[7];
	u8 crc;
} __attribute__ ((packed)) object_t5_t;

#define T5_MESSAGE_CRC_BIT BIT(15)
#define T5_MESSAGE_ADDR_MASK (~T5_MESSAGE_CRC_BIT)

typedef struct t5_data {
	object_t5_t *mem;
} t5_data_t;

/* MXT_GEN_MESSAGE_T5 object */
#define MXT_RPTID_NOMSG		0xff
ssint object_t5_init(u8 rid,  const /*sensor_config_t*/void *cfg, void *mem, void *cb);

#endif /* T5_H_ */