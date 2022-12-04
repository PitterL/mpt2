/*
 * t25.h
 *
 * Created: 6/9/2019 10:28:27 PM
 *  Author: A41450
 */ 


#ifndef T25_H_
#define T25_H_

#ifdef OBJECT_T25

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
		cap_t up;
		cap_t	lo;
	} siglim[NUM_TOUCH_OBJECTS];
	
	u8 pindwellus;
	ref_t sigrangelim[NUM_TOUCH_OBJECTS];
	u8 pinthr;
	u8 pertstinterval;	// 100ms unit
	u8 pertstholdoff;
	u8 pertstrptfactor;
	u8 pertstrptwidth;
	u8 pertstcfg;
} __attribute__ ((packed)) object_t25_t;

/* MXT_SPT_SELFTEST_T25 field */
#define MXT_T25_CTRL_ENABLE		BIT(0)
#define MXT_T25_CTRL_RPTEN		BIT(1)
#define MXT_T25_CTRL_PERSTMODE_MASK		(BIT(3) | BIT(4))
#define MXT_T25_CTRL_DISPERTSTDET	BIT(5)

#define MXT_T25_PERST_MODE_NONE	(0 << 3)		// Power test only
#define MXT_T25_PERST_MODE_FAILURE	(0x1 << 3)		// Periodic, report on failure
#define MXT_T25_PERST_MODE_SUCCESS	(0x2 << 3)		// Periodic, report on success
#define MXT_T25_PERST_MODE_BOTH	(0x3 << 3)		// Periodic, report on failure and success

//#define MXT_T25_CMD_
#define MXT_T25_CMD_NONE	0x0
#define MXT_T25_CMD_TEST_AVDD	0x01
#define MXT_T25_CMD_TEST_PIN_FAULT	0x12
#define MXT_T25_CMD_TEST_SIGNAL_LIMIT	0x17
#define MXT_T25_CMD_TEST_ALL	0xFE

//#define MXT_T25_PERTSTCFG_
#define MXT_T25_PERTSTCFG_PWRTSTEN	BIT(0)
#define MXT_T25_PERTSTCFG_SIGLIMTSTEN	BIT(1)
#define MXT_T25_PERTSTCFG_PINFAULTEN	BIT(2)

/* MXT_SPT_SELFTEST_T25 status */
//#define MXT_T25_INFO_RESULT_
#define MXT_T25_INFO_RESULT_NO_DATA		0x00
#define MXT_T25_INFO_RESULT_PASS		0xFE
#define MXT_T25_INFO_RESULT_INVALID		0xFD
#define MXT_T25_INFO_RESULT_NO_AVDD		0x01
#define MXT_T25_INFO_RESULT_PIN_FAULT	0x12
#define MXT_T25_INFO_RESULT_SIGNAL_LIMIT	0x17

enum T25_TEST_OBJECTS_TYPE {
#ifdef OBJECT_T9	
	INSPECT_SURFACE_SLIDER,
#endif

#ifdef OBJECT_T5	
	INSPECT_BUTTON,
#endif
	NUM_TEST_OBJECTS_TYPES
};

#ifdef OBJECT_T25_EXTENSION
#define MXT_T25_RESULT_INFO_SIZE 7
#else
#define MXT_T25_RESULT_INFO_SIZE 3
#endif

typedef struct object_t25_result {
		u8 status;
		u8 info[MXT_T25_RESULT_INFO_SIZE];
}__attribute__ ((packed)) object_t25_result_t;

/*
Defined in pinfault.h

typedef struct pinfault_test_setting {
	u8 sequence;
	u8 index;	// the node index in node list
	u8 threshold;
	u8 delay;
	u8 count;
	u8 value;
	s8 done;
} pinfault_test_setting_t;
*/
typedef enum pinfault_test_setting_enum {
	PF_SEQ = 0,
	PF_NODE_IDX,
	PF_THRESHOLD,
	PF_DELAY,
	PF_COUNT,
	PF_VALUE,
	PF_DONE,
	NUM_PINFAULT_SETTING
}pinfault_test_setting_enum_t;

#define PF_COUNT_TEST_ALL 0
#define PF_COUNT_TEST_ONE 1

enum {
	CYCLING_TEST,
	CMD_TEST,
	NUM_OP_TYPES,
};

typedef struct t25_data {
	txx_data_t common;
	object_t25_result_t result;
	
	/* pinfault_test_setting_t */u8 pinfault_test_setting[NUM_PINFAULT_SETTING];

	u8 opmask[NUM_OP_TYPES];
	u16 cycling_interval;
	u16 current_ticks;
} t25_data_t;

enum {
	TEST_AVDD = 0,
	TEST_PINFAULT,
	TEST_SIGNAL_LIMIT,
	NUM_TEST_ITEMS,
	FLAG_CYCLING
};
#define ALL_TESTS_MASK (BIT(NUM_TEST_ITEMS) - 1)

enum PINFAULT_CMD {
	REQUEST_ADC_RESOURCE,
	AVDD_CMD_RUN, 
	PINFAULT_CMD_SET_PARAM,
	PINFAULT_CMD_RUN
};

/* Move to pinfault.h */
/*
enum PIN_FAULT_SEQ_CODE {
	DRIVEN_GROUND = 1,
	DRIVEN_HIGH,
	WALKING_1,
	WALKING_0,
	NUM_PIN_FAULT_TEST,
	HIGH_VOLTAGE = 0x7,
};
*/
#define MXT_SPT_SELFTEST_T25_RIDS 1

ssint object_t25_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
void object_t25_start(u8 unused);
void object_t25_data_sync(u8 rw);
void object_t25_report_status(u8 force);

ssint object_api_t25_selftest_porstage(void);
ssint object_api_t25_selftest_prestage(void);
ssint object_api_t25_selftest_poststage(u8 channel, /*const cap_sample_value_t **/ const void * cv);
u8 object_api_t25_test_inprogress(void);
void object_t25_cycling_ticks(uint8_t ticks);

/* api */
#define API_CHANNEL_BASE_REF NAPI_CHANNEL_BASE_REF
#define API_SELFTEST NAPI_SELFTEST

#endif /* OBJECT_T25 */

#endif /* T25_H_ */