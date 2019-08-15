/*
 * t9.h
 *
 * Created: 6/9/2019 9:37:45 AM
 *  Author: A41450
 */ 


#ifndef T9_H_
#define T9_H_

#ifdef OBJECT_T9

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
	u8 movfilter;
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

#define MXT_T9_ORIENT		9
/* MXT_TOUCH_MULTI_T9 orient */
#define MXT_T9_ORIENT_SWITCH	BIT(0)
#define MXT_T9_ORIENT_INVERTX	BIT(1)
#define MXT_T9_ORIENT_INVERTY	BIT(2)

#define MXT_T9_RANGE		18

/* MXT_TOUCH_MULTI_T9 status */
#define MXT_T9_UNGRIP		BIT(0)
#define MXT_T9_SUPPRESS		BIT(1)
#define MXT_T9_AMP		BIT(2)
#define MXT_T9_VECTOR		BIT(3)
#define MXT_T9_MOVE		BIT(4)
#define MXT_T9_RELEASE		BIT(5)
#define MXT_T9_PRESS		BIT(6)
#define MXT_T9_DETECT		BIT(7)

typedef struct t9_range {
	u16 x;
	u16 y;
}  __attribute__ ((packed)) t9_range_t;

typedef struct t9_point_status {
	u8 status;
	t9_range_t pos;
}t9_point_status_t;

#define MXT_TOUCH_MULTI_T9_INST 1	/* Only support 1 instance now */
#define MXT_TOUCH_MULTI_T9_RIDS 1

typedef struct t9_data {
	txx_data_t common;
	const /*qsurface_config_t*/void *surdef;
	t9_point_status_t points[MXT_TOUCH_MULTI_T9_RIDS];
	u16 resolution_max;
} t9_data_t;

ssint object_t9_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
void object_t9_start(u8 loaded);
void object_t9_data_sync(u8 rw);
void object_t9_report_status(u8 force);
u16 object_t9_get_surface_slider_base_ref(u8 inst);
ssint object_api_t9_set_pointer_location(u8 inst, u8 id, u8 status, u16 x, u16 y);

#else	/* OBJECT_T9 */

#define MXT_TOUCH_MULTI_T9_INST 0	/* Set T9 instance to ZERO if not compiled */
#define MXT_TOUCH_MULTI_T9_RIDS 0

#endif /* OBJECT_T9 */

#endif /* T9_H_ */