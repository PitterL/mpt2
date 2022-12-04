/*
 * t5.h
 *
 * Created: 6/9/2019 10:25:04 AM
 *  Author: A41450
 */ 


#ifndef T5_H_
#define T5_H_

#ifdef OBJECT_T5

#define MXT_T5_MESSAGE_SIZE 9

/* T5 Memory space */
typedef struct object_t5 {
	u8 reportid;
	u8 data[MXT_T5_MESSAGE_SIZE - 1];
	u8 crc;
} __attribute__ ((packed)) object_t5_t;

#define T5_MESSAGE_CRC_BIT BIT(15)
#define T5_MESSAGE_ADDR_MASK (~T5_MESSAGE_CRC_BIT)

typedef struct t5_data {
	object_t5_t *mem;
} t5_data_t;

ssint object_t5_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);

#endif /* OBJECT_T5 */

/* MXT_GEN_MESSAGE_T5 object */
#define MXT_RPTID_NOMSG		0xff

#endif /* T5_H_ */