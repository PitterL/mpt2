/*
 * t8.h
 *
 * Created: 6/9/2019 10:33:10 AM
 *  Author: A41450
 */ 


#ifndef T8_H_
#define T8_H_

/* T8 Memory Space */
typedef struct object_t8 {
	u8 chrgtime;
	u8 /*rsv*/atchdrift;
	u8 tchdrift;
	u8 driftst;
	u8 tchautocal;
	u8 sync;
	u8 atchcalst;
	u8 atchcalsthr;
	u8 atchfrccalthr;
	u8 atchfrccalratio;

	/* Current measure mode */
	u8 measallow;

	/* Inspection measure mode */
	u8 meastestdef;

	/* Normal measure mode */
	u8 measactvdef;

	/* measure mode control 
		bit[0]: using ref and signal in t37 debug register switch - delta command get the reference, reference command will get cccap
		bit[1]: TBD
	*/
	u8 refmode;
	u8 cfg;
} __attribute__ ((packed)) object_t8_t;

#define MXT_T8_MEASALLOW_MUTUALTCH	BIT(0)
#define MXT_T8_MEASALLOW_SELFTCH	BIT(1)
#define MXT_T8_MEASALLOW_HOVER	BIT(2)
#define MXT_T8_MEASALLOW_SELFPROX	BIT(3)
// Below definition not use in MaxTouch
#define MXT_T8_MEASALLOW_MUTUALTCH_4P	(BIT(4) | MXT_T8_MEASALLOW_MUTUALTCH)
#define MXT_T8_MEASALLOW_MUTUALTCH_8P	(BIT(5) | MXT_T8_MEASALLOW_MUTUALTCH)

#define MXT_T8_MEASALLOW_ALLOWED (MXT_T8_MEASALLOW_MUTUALTCH|MXT_T8_MEASALLOW_MUTUALTCH_4P| MXT_T8_MEASALLOW_MUTUALTCH_8P | MXT_T8_MEASALLOW_SELFTCH | MXT_T8_MEASALLOW_SELFPROX)

#define MXT_T8_REFMODE_RS_SWITCH BIT(0)
#define MXT_T8_REFMODE_IDLE_MEASURE BIT(1)

typedef struct txx_data t8_data_t;

ssint object_t8_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
void object_t8_start(u8 loaded);
void object_t8_data_sync(u8 rw);

#ifdef OBJECT_T8_RSD_SWITCH
u8 object_api_t8_ref_mode(void);
#endif

ssint object_t8_switch_measure_mode(u8 test);

u8 object_t8_get_measure_mode(u8 *meas);
u8 object_api_t8_measuring_self(void);
u8 object_api_t8_measuring_mutual(void);

#endif /* T8_H_ */