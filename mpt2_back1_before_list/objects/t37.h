/*
 * t37.h
 *
 * Created: 6/9/2019 12:23:54 PM
 *  Author: A41450
 */ 


#ifndef T37_H_
#define T37_H_


#define T37_DATA_SIZE 14

/* T37 Memory space */
typedef struct object_t37 {
	u8 mode;
	u8 page;
	u8 data[T37_DATA_SIZE];
} __attribute__ ((packed)) object_t37_t;


#endif /* T37_H_ */