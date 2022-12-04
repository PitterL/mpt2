/*
 * t9.h
 *
 * Created: 6/9/2019 9:37:45 AM
 *  Author: A41450
 */ 


#ifndef T9_M_H_
#define T9_M_H_

#ifdef OBJECT_T9_DUMMY

/* T9 configure */
typedef struct object_t9 {
	u8 ctrl;
	u8 xorigin;
	u8 yorigin;
	u8 xsize;
	u8 ysize;
	u8 akscfg;
	u8 blen;
	u8 tchthr;
	u8 tchdi;
	u8 orient;
	u8 mrgtimeout;
	u8 movhysti;
	u8 movhystn;
	nibble_t movfilter;	//Bit 7: Disable; Bit[6:4] Filter Limit; Bit[3:0] Filter ADAPTHR
	u8 numtouch;
	u8 mrghyst;
	u8 mrgthr;
	u8 amphyst;
	u16 xrange;
	u16 yrange;
	u8 xloclip;
	u8 xhiclip;
	u8 yloclip;
	u8 yhiclip;
	u8 xedgectrl;
	u8 xedgedist;
	u8 yedgectrl;
	u8 yedgedist;
	u8 jumplimit;
	u8 tchhyst;
	u8 xpitch;
	u8 ypitch;
	u8 nexttchdi;
} __attribute__ ((packed)) object_t9_t;

/* MXT_TOUCH_MULTI_T9 field */
#define MXT_T9_CTRL		0
#define MXT_T9_CTRL_ENABLE		BIT(0)
#define MXT_T9_CTRL_RPTEN		BIT(1)
#define MXT_T9_CTRL_DISSCRMSG0	BIT(2)
#define MXT_T9_CTRL_SCANEN		BIT(3)

#define MXT_TOUCH_MULTI_T9_INST 1
#define MXT_TOUCH_MULTI_T9_RIDS 0   /* No report support of the dummy object */

typedef struct txx_data t9_data_t;

ssint object_t9_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
void object_t9_start(u8 loaded);
void object_t9_report_status(u8 force);
void object_t9_data_sync(u8 rw);

#else

#define MXT_TOUCH_MULTI_T9_INST 0	/* Set T9 instance to ZERO if not compiled */
#define MXT_TOUCH_MULTI_T9_RIDS 0

#endif /* OBJECT_T9_DUMMY */

#endif /* T9_M_H_ */