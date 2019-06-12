/*
 * t37.h
 *
 * Created: 6/9/2019 12:23:54 PM
 *  Author: A41450
 */ 


#ifndef T37_H_
#define T37_H_

/* T37 Memory space */
typedef struct object_t37 {
	u8 mode;
	u8 page;
#define T37_DATA_SIZE 16
	u16 data[T37_DATA_SIZE];
} __attribute__ ((packed)) object_t37_t;

typedef struct t37_data {
	u8 measallow;	/* sensor type */
	u8 xsize;
	u8 ysize;
	u8 page;
	object_t37_t *mem;
} t37_data_t;

int object_t37_init(u8 rid, const /*sensor_config_t*/void *cfg, void *mem);
void object_t37_start(void);

void object_t37_set_data_page(u8 page);
void object_t37_set_sensor_data(u8 dbgcmd, u8 channel, u16 reference, u16 signal, u16 cap);

#endif /* T37_H_ */