/*
 * t25.h
 *
 * Created: 6/9/2019 10:28:27 PM
 *  Author: A41450
 */ 


#ifndef T25_H_
#define T25_H_

/*
	n = number of touch objects, assigned in the following order:
	All Multiple Touch Touchscreen T100 objects
	All Key Array T15 objects
*/
#define	NUM_TOUCH_OBJECTS (MXT_TOUCH_MULTI_T9_INST + MXT_TOUCH_KEYARRAY_T15_INST)

typedef struct object_t25 {
	u8 ctrl;
	u8 cmd;
	struct {
		u16 up;
		u16	lo;
	} siglim[NUM_TOUCH_OBJECTS];
	
	u8 pindwellus;
	u16 sigrangelim[NUM_TOUCH_OBJECTS];
	u8 pinthr;
} __attribute__ ((packed)) object_t25_t;

#define MXT_SPT_SELFTEST_T25_RIDS 1

#endif /* T25_H_ */