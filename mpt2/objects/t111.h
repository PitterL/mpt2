/*
 * t111.h
 *
 * Created: 6/12/2019 2:16:44 PM
 *  Author: A41450
 */ 


#ifndef T111_H_
#define T111_H_

typedef struct object_t111 {
	u8 ctrl;
	u8 dbgctrl;
	u8 inttime;
	u8 delaytime;
	u8 idlesyncsperl;
	u8 actvsyncsperl;
	u8 drift;
	u8 driftst;
	u8 rsv0;
	u8 calrecst;
	u8 rsv1[5];
	u8 inrushcfg;
	u8 altinttimex;
	u8 altdelaytimex;
	u8 dcdrift;
	u8 rsv2;
	u8 dcfilter;
	u8 dccalrecstr;
	u8 dccalerrratio;
	u8 rsv3[2];
	u8 dcgainsf;
	u8 dcthrx;
	u8 dcthry;
	u8 dcidleslewmin;	
} object_t111_t;

#define MXT_SPT_SELFCAPCONFIG_T111_INST 1

#endif /* T111_H_ */