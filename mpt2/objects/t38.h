/*
 * t38.h
 *
 * Created: 3/18/2021 10:41:00 AM
 *  Author: A41450
 */ 


#ifndef T38_H_
#define T38_H_

/* T38 Memory space */
#define T38_DATA_SIZE	16

typedef struct object_t38 {
	u8 data[T38_DATA_SIZE];
} __attribute__ ((packed)) object_t38_t;

typedef struct t38_data {
	txx_data_t common;
} t38_data_t;

ssint object_t38_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
void object_t38_start(void);

#endif /* T38_H_ */