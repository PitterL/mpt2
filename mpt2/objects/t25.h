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

/* MXT_SPT_SELFTEST_T25 field */
#define MXT_T25_CTRL_ENABLE		BIT(0)
#define MXT_T25_CTRL_RPTEN		BIT(1)
//#define MXT_T25_CTRL_
#define MXT_T25_CMD_NONE	0x0
#define MXT_T25_CMD_TEST_AVDD	0x01
#define MXT_T25_CMD_TEST_PIN_FAULT	0x12
#define MXT_T25_CMD_TEST_SIGNAL_LIMIT	0x17
#define MXT_T25_CMD_TEST_ALL	0xFE

/* MXT_SPT_SELFTEST_T25 status */
//#define MXT_T25_INFO_RESULT_
#define MXT_T25_INFO_RESULT_NO_DATA		0x00
#define MXT_T25_INFO_RESULT_PASS		0xFE
#define MXT_T25_INFO_RESULT_INVALID		0xFD
#define MXT_T25_INFO_RESULT_NO_AVDD		0x01
#define MXT_T25_INFO_RESULT_PIN_FAULT	0x12
#define MXT_T25_INFO_RESULT_SIGNAL_LIMIT	0x17

enum MSG_T25_PIN_FAULT {
	PIN_FAULT_SEQ = 2,
	PIN_FAULT_X,
	PIN_FAULT_Y
};

enum PIN_FAULT_SEQ_CODE {
	DRIVEN_GROUND = 1,
	DRIVEN_HIGH,
	WALKING_1,
	WALKING_0,
	HIGH_VOLTAGE = 0x7,
};

enum MSG_T25_SIGNAL_LIMIT {
	SIGNAL_LIMIT_OBJECT = 2,
	SIGNAL_LIMIT_INSTANCE,
};

enum T25_TEST_OBJECTS_TYPE {
#ifdef OBJECT_T9	
	INSPECT_SURFACE_SLIDER,
#endif

#ifdef OBJECT_T5	
	INSPECT_BUTTON,
#endif
	NUM_TEST_OBJECTS_TYPES
};

enum T25_TEST_ITEMS_TYPE {
	TEST_AVDD,
	TEST_PINFAULT,
	TEST_T9_SIGNAL_LIMIT,
	TEST_T15_SIGNAL_LIMIT,
	NUM_TEST_ITEMS_TYPE,
	TEST_ALL = ((1 << NUM_TEST_ITEMS_TYPE) - 1),
	TEST_MASK = TEST_ALL
};

#define SIGNAL_LIMIT_MASK (BIT(TEST_T9_SIGNAL_LIMIT) | BIT(TEST_T15_SIGNAL_LIMIT))

#ifdef OBJECT_T25_EXTENSION
#define MXT_T25_RESULT_INFO_SIZE 7
#else
#define MXT_T25_RESULT_INFO_SIZE 3
#endif
typedef struct object_t25_result {
	struct {
		u8 status;
		u8 info[MXT_T25_RESULT_INFO_SIZE];
	}__attribute__ ((packed)) data;
	u8 testop;
	u8 counter[NUM_TEST_OBJECTS_TYPES];
}__attribute__ ((packed)) object_t25_result_t;

typedef struct t25_data {
	txx_data_t common;
	object_t25_result_t cache;
} t25_data_t;

#define MXT_SPT_SELFTEST_T25_RIDS 1

ssint object_t25_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
void object_t25_start(u8 unused);
void object_t25_data_sync(u8 rw);
void object_t25_report_status(u8 force);

void object_api_t25_set_sensor_data(u8 channel, u16 reference, u16 signal, u16 cap);
ssint object_api_t25_pinfault_test(void);

#endif /* T25_H_ */