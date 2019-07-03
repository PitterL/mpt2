/*
 * t37.h
 *
 * Created: 6/9/2019 12:23:54 PM
 *  Author: A41450
 */ 


#ifndef T37_H_
#define T37_H_

/* T37 Memory space */
#define T37_DATA_SIZE 32	//More than 128 will careful for object_api_t37_set_sensor_data pos overflow
#define T37_CACHE_PAGES_NUM 3
#define T37_CACHE_PAGE_SIZE (T37_DATA_SIZE * T37_CACHE_PAGES_NUM)

typedef struct object_t37 {
	u8 mode;
	u8 page;
	u16 data[T37_DATA_SIZE];
} __attribute__ ((packed)) object_t37_t;

typedef struct dbgcache {
	u16 data[T37_CACHE_PAGE_SIZE];
}dbgcache_t;

typedef t6_debug_command_t t37_page_status_t;

typedef struct t37_data {
	txx_data_t common;
	t37_page_status_t status;
} t37_data_t;

ssint object_t37_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
void object_t37_start(void);

void object_api_t37_set_data_page(u8 cmd, u8 page);
void object_api_t37_set_sensor_data(u8 channel, u16 reference, u16 signal, u16 cap);

#endif /* T37_H_ */