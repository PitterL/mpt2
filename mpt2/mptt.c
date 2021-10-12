/*
 * mpt2.c
 *
 * Created: 6/7/2019 9:48:06 PM
 *  Author: A41450

 v2.1
	<1> Add Infoblock crc are in config for version match
    <2> Add Scanning mode switch in T8

 */ 

#include <string.h>
#include "include/types.h"

#include "include/list.h"
#include "arch/cpu.h"
#include "arch/tslapi.h"
#include "objects/txx.h"
#include "crc.h"
#include "tsl.h"
#include "mptt.h"

/* use for sync the version with pack-build firmware */
#include <pack.h>
/*
	FW version:
	
	v21: added t25 pinfault detection
	v22: <1> added t38 object
		 <2> add 'aks' function at T15
	v23(1.0): draft merge from low power scanning support
		 <1> support idle(low power) and active scanning rate setting
		 <2> support switch from active to idle by timeout setting
		 <3> set i2c bus mornitor ticks from current measure time: mptt_bus_monitor_ticks(measurement_period_store);
		 <4> disable API_DEF_TCH_DRIFT_RATE sync with qtlib for calibration directly, it will affect the lowpower dynamical setting(caused calibration)
		 
		 the current in idle mode:
	v23(1.1): 
		 <1> add T126 object for low power control
		 <2> wake up node mask
		 <3> re-grouped the state machine of `qlib_touch_flag` to fix the frozen symptom after I2C command issued
		 <4> Fixed the scanning issue in status switch
		 <5> Fixed T126 skip message bug
		 <6> Preliminary tested the t25 command in lowpower mode
		 <8> Change mpt_api_request_irq() LOCK zone and placed into each call before sleep
	v23(2.0):
		 <1> add T126 diagnostic debug data support in T37
		 <2> add T126 wakeup message report
		 <3> T6 backup command result check added 
	v24(2.1):
		<1> I2c end callback(flash data) only when written transfer larger than 2 bytes
		<2> Set bus_asset_irq in each cycle for fast response(special in sleep state)
		<3> Use gpio_release_chg() for retrigger int pulse(not verified)
		<4> rename `get_bus_line_level` to `gpio_get_bus_line_level`
		<5> remove `tsl_suspend()` in pinfault.c
		<6> async handle calibration and initialize sensor by `qtm_init_sensor_key_post` and `calibrate_node_post`, note the all sensor will be processed whatever
		<7> move `touch_process()` to `mptt_process()`
		<8> add `tsl_assert_irq()` at `mptt_post_process()` becasue some message generated after process()
		<9> check i2c status twice by call mptt_get_bus_state() in mptt_sleep() before sleep_cpu(), that avoids I2C snatched be sleep instruction
		<10> add mpt_get_message_count() for message count, which will decide `t5.reportid != MXT_RPTID_NOMSG`. That will avoid non-standard reading size of accessing t5 to discard a valid message
		<11> update t44 value only in object read cycle
		<12> T25 Pin fault test with library idle check object_ts_state_idle() limit, and set object_ts_suspend()
		<13> T37 SIGNAL will return delta in refmode `DBG_CAP`
		<14> change sleep logic: mptt_sleep()->tsl_sleep()->touch_sleep()->touch_process_lowpower()=>sleep_cpu(), it will check tsapi_get_chip_state() and mptt_get_bus_state()
		<15> change to`qlib_touch_state` from `qlib_touch_flag` for touch lib state management
		<16> add `measurement_state` for for calibration and initialization post process
	v25(1.0):
		<1> Fuse check with `SigErr` T6 message
		<2> Config check with `CfgErr` T6 message
		<3> cccap calculation cache at call CALCULATE_CAP() to save power comsuption
		<4> tsapi_get_number_sensor_channels() and tsapi_get_number_key_sensors() for fast access sensor count at main loop to save power comsuption
		<5> move `buttons_config`, `surfaces_sliders_config` and `tsl_qtouch_def` from tsl.c to board.c
		<6> mpt_api_set_sensor_data() use pointer to transfer parameter value(T37/T25/T109)
		<7> Remove T111 default
		
		(1.1)
		<1> Support Signature Row data
		<2> Check suspend channel in data update
		<3> Put `channel switch` in from touch_process() to touch_sleep() to make post_process with correct channels

		(1.2)
		<1> Revert the `channel switch` to touch_process() and use touch_state_sleep() to decide wether execute touch_post_process()
		
		(1.3)
		<1> Add t9/t15 callback hook
		<2> Use t15 as primary gain/threshold parameter 
		(1.4)
		<1> T25 test scanning mode switch will issue a calibration command, signal limit test will pending after switched
		<2> T25 Test All is splitted into the pinfault test(handled in tsl_pre_process()) and signal limit test(tsl_post_process()) to supply stable test cycle
		<3> T25 signal limit test for T9 bug fixed and use T15 as baseref source
		<4> T25 test command will wakeup chip for processing in short time
		<5> T6 calibration command will wakeup chip and done in next drift cycle
		<6> Drift decide with get_next_measurement_period() and set margin from 200ms to 40ms to save power and avoid frozen timer issue(restart timer before timeout)
		<7> Fix the issue I2C communication will be stalled if Pinfault detected at bootup 
		<8> Tested the sleep mode in 5*6 surface(15uA@event system system, 55uA@software sleep / 1.5mA@active)
		(1.5)
		<1> Use indenpent global varible for qlib_touch_state_xxx states instead of bit mask, which cause un-atomic mutex between interupt and main thread
		<2> Add overflow counter for qtm_ptc_start_measurement_seq() timeout, which will report overflow tag in T6 message
		<3> split touch_process() to touch_handle_measurement() and touch_handle_acquisition_process(), the later one will called first
		(1.6)
		<1> T111 controllable with parameters
		<2> add `WK_RSV_NEG_BREACH` in T126 wakeup message
		<3> add touch_non_ptc_pin_config() at touch_init() for non PTC pin initialize
		<4> #define DEF_TOUCH_MEASUREMENT_OVERFLOW_FORCE_DONE 200
		<5> add LED0 callback when button touched
		(1.7)
		<1> Add Fuse output in `MXT_DIAGNOSTIC_DEVICE_INFO` at page 1
		<2> Set Pin Fault and AVDD test looped check if POR found failure, it could recovery when symptom gone 
		<3> Disable I2C controller at init() stage, enable is at open()

		Report ID Object Table Index Object Type Object Instance
		0 = 0x00                  0           0               0
		1 = 0x01                  3           6               0
		2 = 0x02                  7          15               0
		3 = 0x03                  7          15               1
		4 = 0x04                  8          25               0
		5 = 0x05                  9         126               0

		Object Table Index Object Type Address Size Instances Report IDs
		0          37      71   66         1          -
		1          44     137    1         1          -
		2           5     138   10         1          -
		3           6     148    6         1          1
		4          38     154   16         1          -
		5           7     170    4         1          -
		6           8     174   15         1          -
		7          15     189   11         2      2 - 3
		8          25     211   16         1          4
		9         126     227    9         1          5

*/

#define MPTT_FW_FAMILY_ID 0xa6
#define MPTT_FW_VARIANT_ID 0x11
#define MPTT_FW_VERSION 0x25
/* use the latest byte for the build version */
#define MPTT_FW_BUILD (PROJECT_CODE & 0xFF)

typedef struct mxt_info {
	u8 family_id;
	u8 variant_id;
	u8 version;
	u8 build;
	u8 matrix_xsize;
	u8 matrix_ysize;
	u8 object_num;
} __attribute__ ((packed)) mxt_info_t;

mxt_info_t ib_id_information = {
	MPTT_FW_FAMILY_ID,	/* family_id */
	MPTT_FW_VARIANT_ID,	/* variant_id */
	MPTT_FW_VERSION,	/* version */
	MPTT_FW_BUILD,	/* build */
};
#define MXT_ID_INFORMATION_SIZE (sizeof(ib_id_information))

typedef struct mxt_object {
	u8 type;
	u16 start_address;
	u8 size_minus_one;
	u8 instances_minus_one;
	u8 num_report_ids;
} __attribute__ ((packed)) mxt_object_t;

data_crc24_t ib_info_crc;
#define MXT_INFOBLOCK_CRC_SIZE (sizeof(ib_info_crc))

typedef struct objects_ram {
#ifdef OBJECT_T37
	object_t37_t t37;
#endif
#ifdef OBJECT_T44
	object_t44_t t44;
#endif
#ifdef OBJECT_T5
	object_t5_t t5;
#endif
} __attribute__ ((packed)) objects_ram_t;

#define MXT_OBJECTS_RAM_SIZE (sizeof(struct objects_ram))

typedef struct objects_ctrl {
#ifdef OBJECT_T6
	object_t6_t t6;
#endif
} __attribute__ ((packed)) objects_ctrl_t;

#define MXT_OBJECTS_CTRL_SIZE (sizeof(struct objects_ctrl))

typedef struct objects_uncfg {
#ifdef OBJECT_T38
	object_t38_t t38;
#endif
} __attribute__ ((packed)) objects_uncfg_t;

typedef struct objects_config {
#ifdef OBJECT_T7
	object_t7_t t7;
#endif
#ifdef OBJECT_T8
	object_t8_t t8;
#endif
#ifdef OBJECT_T9
	object_t9_t t9_objs[MXT_TOUCH_MULTI_T9_INST];
#endif
#ifdef OBJECT_T15
	object_t15_t t15_objs[MXT_TOUCH_KEYARRAY_T15_INST];
#endif
#ifdef OBJECT_T18
	object_t18_t t18;
#endif
#ifdef OBJECT_T25
	object_t25_t t25;
#endif
#ifdef OBJECT_T104
	object_t104_t t104;
#endif
#ifdef OBJECT_T109
	object_t109_t t109;
#endif
#ifdef OBJECT_T111
	object_t111_t t111;
#endif
#ifdef OBJECT_T126
	object_t126_t t126;
#endif
	data_crc24_t crc;
	data_crc24_t ib;	// This is redundant design for checking FW match config 
} __attribute__ ((packed)) objects_config_t;

#define MXT_OBJECTS_CONFIG_SIZE (sizeof(struct objects_config))

typedef struct mxt_objects_reg {
	objects_ram_t ram;
	objects_ctrl_t ctrl;
	objects_uncfg_t noncfg;	// config without crc calculated. Noted: maxtouch protocol, the noncrc-config should between ctrl and crc-config zone
	objects_config_t cfg;
} __attribute__ ((packed)) mxt_objects_reg_t;

mxt_objects_reg_t ib_objects_reg; // The order of the struct must align ib_objects_tables

#define MXT_OBJECTS_SIZE (sizeof(ib_objects_reg))

// The order of the struct must align struct mxt_objects_reg
mxt_object_t ib_objects_tables[] = {
#ifdef OBJECT_T37
	{	MXT_DEBUG_DIAGNOSTIC_T37, /*start_address*/-1, sizeof(struct object_t37) - 1, /*instances_minus_one*/0, /*num_report_ids*/0	},
#endif
#ifdef OBJECT_T44	
	{	MXT_SPT_MESSAGECOUNT_T44, /*start_address*/-1, sizeof(struct object_t44) - 1, /*instances_minus_one*/0, /*num_report_ids*/0	},
#endif
#ifdef OBJECT_T5
	{	MXT_GEN_MESSAGE_T5, /*start_address*/-1, sizeof(struct object_t5) - 1, /*instances_minus_one*/0, /*num_report_ids*/0	},
#endif
#ifdef OBJECT_T6	
	{	MXT_GEN_COMMAND_T6, /*start_address*/-1, sizeof(struct object_t6) - 1, /*instances_minus_one*/0, /*num_report_ids*/MXT_GEN_COMMAND_T6_RIDS	},
#endif
#ifdef OBJECT_T38
	{	MXT_SPT_USERDATA_T38, /*start_address*/-1, sizeof(struct object_t38) - 1, /*instances_minus_one*/0, /*num_report_ids*/0	},
#endif
#ifdef OBJECT_T7	
	{	MXT_GEN_POWER_T7, /*start_address*/-1, sizeof(struct object_t7) - 1, /*instances_minus_one*/0, /*num_report_ids*/0	},
#endif	
#ifdef OBJECT_T8	
	{	MXT_GEN_ACQUIRE_T8, /*start_address*/-1, sizeof(struct object_t8) - 1, /*instances_minus_one*/0, /*num_report_ids*/0	},
#endif	
#ifdef OBJECT_T9
	{	MXT_TOUCH_MULTI_T9, /*start_address*/-1, sizeof(struct object_t9) - 1, /*instances_minus_one*/MXT_TOUCH_MULTI_T9_INST - 1, /*num_report_ids*/MXT_TOUCH_MULTI_T9_RIDS	},
#endif
#ifdef OBJECT_T15	
	{	MXT_TOUCH_KEYARRAY_T15, /*start_address*/-1, sizeof(struct object_t15) - 1, /*instances_minus_one*/MXT_TOUCH_KEYARRAY_T15_INST - 1, /*num_report_ids*/MXT_TOUCH_KEYARRAY_T15_RIDS	},
#endif	
#ifdef OBJECT_T18
	{	MXT_SPT_COMMSCONFIG_T18, /*start_address*/-1, sizeof(struct object_t18) - 1, /*instances_minus_one*/0, /*num_report_ids*/0	},
#endif
#ifdef OBJECT_T25
	{	MXT_SPT_SELFTEST_T25, /*start_address*/-1, sizeof(struct object_t25) - 1, /*instances_minus_one*/0, /*num_report_ids*/MXT_SPT_SELFTEST_T25_RIDS	},
#endif
#ifdef OBJECT_T104
	{	MXT_SPT_AUXTOUCHCONFIG_T104, /*start_address*/-1, sizeof(struct object_t104) - 1, /*instances_minus_one*/MXT_SPT_AUXTOUCHCONFIG_T104_INST - 1, /*num_report_ids*/0	},
#endif
#ifdef OBJECT_T109
	{	MXT_SPT_SELFCAPGLOBALCONFIG_T109, /*start_address*/-1, sizeof(struct object_t109) - 1, /*instances_minus_one*/0, /*num_report_ids*/MXT_SPT_SELFCAPGLOBALCONFIG_T109_RIDS	},
#endif
#ifdef OBJECT_T111	
	{	MXT_SPT_SELFCAPCONFIG_T111, /*start_address*/-1, sizeof(struct object_t111) - 1, /*instances_minus_one*/MXT_SPT_SELFCAPCONFIG_T111_INST - 1, /*num_report_ids*/0	},
#endif
#ifdef OBJECT_T126
	{	MXT_SPT_LOWPOWERIDLECONFIG_T126, /*start_address*/-1, sizeof(struct object_t126) - 1, /*instances_minus_one*/0, /*num_report_ids*/MXT_SPT_LOWPOWERIDLECONFIG_T126_RIDS	},
#endif
};

#define MXT_OBJECTS_NUM ((u8)ARRAY_SIZE(ib_objects_tables))
#define MXT_OBJECTS_TABLE_SIZE (sizeof(ib_objects_tables))

/* Should add all report count here */
#define MXT_REPORT_ID_START 1
#define MXT_REPORT_ID_COUNT (MXT_GEN_COMMAND_T6_RIDS + \
								MXT_TOUCH_MULTI_T9_RIDS * MXT_TOUCH_MULTI_T9_INST + \
								MXT_TOUCH_KEYARRAY_T15_RIDS * MXT_TOUCH_KEYARRAY_T15_INST +\
								MXT_SPT_SELFTEST_T25_RIDS + \
								MXT_SPT_SELFCAPGLOBALCONFIG_T109_RIDS + \
								MXT_SPT_LOWPOWERIDLECONFIG_T126_RIDS)
								
#if MXT_REPORT_ID_COUNT > 254
#error "Report id count too large, varible may overflow"
#endif
#define MXT_MESSAGE_DEPTH_EACH_RID 3
#define MXT_MESSAGE_FIFO_SIZE (MXT_REPORT_ID_COUNT * MXT_MESSAGE_DEPTH_EACH_RID + 1)
#if MXT_MESSAGE_FIFO_SIZE > 255
#error "Message Fifo too large, may overflow"
#endif
#define CHECK_REPORT_ID(_rid) ((_rid) >= MXT_REPORT_ID_START && (_rid) < (MXT_REPORT_ID_START + MXT_REPORT_ID_COUNT))

#define MXT_ID_INFORMATION_START 0
#define MXT_OBJECT_TABLE_START (MXT_ID_INFORMATION_START + MXT_ID_INFORMATION_SIZE)
#define MXT_INFO_CRC_START (MXT_OBJECT_TABLE_START + MXT_OBJECTS_TABLE_SIZE)
#define MXT_OBJECTS_START (MXT_INFO_CRC_START + MXT_INFOBLOCK_CRC_SIZE)
#define MXT_OBJECTS_RAM_START (MXT_OBJECTS_START + offsetof(mxt_objects_reg_t, ram))
#define MXT_OBJECTS_CTRL_START (MXT_OBJECTS_START + offsetof(mxt_objects_reg_t, ctrl))
#define MXT_OBJECTS_NONCFG_START (MXT_OBJECTS_START + offsetof(mxt_objects_reg_t, noncfg))
#define MXT_OBJECTS_CFG_START (MXT_OBJECTS_START + offsetof(mxt_objects_reg_t, cfg))
#define MXT_MEMORY_END (MXT_OBJECTS_START + MXT_OBJECTS_SIZE)

/* Callback interface */
typedef struct object_callback {
	u8 type;	/* Report ID */

	ssint (*init)(u8 rid, /* const (qtouch_config_t *) */const void *, /* mem space */void *, /* (const qtouch_api_callback_t*) */const void *);
	void (*start)(/* load default config */u8 loaded);
	void (*sync)(u8 rw);
	void (*report)(u8 force);

	void *mem;	// Reg memory pointer
} object_callback_t;

object_callback_t object_initialize_list[] = {
#ifdef OBJECT_T37	
	{	MXT_DEBUG_DIAGNOSTIC_T37, object_t37_init, NULL, NULL, NULL, (void *)&ib_objects_reg.ram.t37	}, 
#endif
#ifdef OBJECT_T44	
	{	MXT_SPT_MESSAGECOUNT_T44	},
#endif	
#ifdef OBJECT_T5	
	{	MXT_GEN_MESSAGE_T5, object_t5_init, NULL, NULL, NULL, (void *)&ib_objects_reg.ram.t5	},
#endif
#ifdef OBJECT_T6
	{	MXT_GEN_COMMAND_T6, object_t6_init,	object_t6_start, NULL, object_t6_report_status, (void *)&ib_objects_reg.ctrl.t6	},	
#endif
#ifdef OBJECT_T38
	{	MXT_SPT_USERDATA_T38, object_t38_init, NULL, NULL, NULL, (void *)&ib_objects_reg.noncfg.t38	},
#endif
#ifdef OBJECT_T7
	{	MXT_GEN_POWER_T7, object_t7_init, object_t7_start, object_t7_data_sync, NULL, (void *)&ib_objects_reg.cfg.t7	},	
#endif
#ifdef OBJECT_T8	
	{	MXT_GEN_ACQUIRE_T8,	object_t8_init, object_t8_start, object_t8_data_sync, NULL, (void *)&ib_objects_reg.cfg.t8	},	
#endif
//The following project should later initialized than t8, since it request sensor scanning mode
#ifdef OBJECT_T9	
	{	MXT_TOUCH_MULTI_T9, object_t9_init, object_t9_start, object_t9_data_sync, object_t9_report_status, (void *)ib_objects_reg.cfg.t9_objs	},
#endif
#ifdef OBJECT_T15
	{	MXT_TOUCH_KEYARRAY_T15, object_t15_init, object_t15_start, object_t15_data_sync, object_t15_report_status, (void *)ib_objects_reg.cfg.t15_objs	},
#endif
#ifdef OBJECT_T18
	{	MXT_SPT_COMMSCONFIG_T18, object_t18_init, object_t18_start, /*object_t18_data_sync*/NULL, NULL, (void *)&ib_objects_reg.cfg.t18},
#endif
#ifdef OBJECT_T25	
	{	MXT_SPT_SELFTEST_T25, object_t25_init, object_t25_start, object_t25_data_sync, object_t25_report_status, (void *)&ib_objects_reg.cfg.t25	},	
#endif
#ifdef OBJECT_T104
	{	MXT_SPT_AUXTOUCHCONFIG_T104,  object_t104_init, object_t104_start, object_t104_data_sync, NULL, (void *)&ib_objects_reg.cfg.t104	},
#endif
#ifdef OBJECT_T109
{	MXT_SPT_SELFCAPGLOBALCONFIG_T109, object_t109_init, object_t109_start, object_t109_data_sync, NULL, (void *)&ib_objects_reg.cfg.t109	},
#endif
#ifdef OBJECT_T111
	{	MXT_SPT_SELFCAPCONFIG_T111, object_t111_init, object_t111_start, object_t111_data_sync, NULL, (void *)&ib_objects_reg.cfg.t111	},
#endif
#ifdef OBJECT_T126
	{	MXT_SPT_LOWPOWERIDLECONFIG_T126, object_t126_init, object_t126_start, object_t126_data_sync, object_t126_report_status, (void *)&ib_objects_reg.cfg.t126	},
#endif
};
#define MXT_OBJECTS_INITIALIZE_LIST_NUM (ARRAY_SIZE(object_initialize_list))

#ifdef OBJECT_T5
typedef struct message_buffer{
	object_t5_t message;
	struct list_head node;
}message_buffer_t;
message_buffer_t message_caches[MXT_MESSAGE_FIFO_SIZE];

typedef struct mxt_message_fifo {
	struct list_head fifo;
	u8 reporter[MXT_REPORT_ID_COUNT];	//message count for each reporter
	u8 count;	//message sum
} mxt_message_fifo_t;

mxt_message_fifo_t message_fifo;
#endif

#ifdef OBJECT_WRITEBACK
typedef struct dirty_mark {
#define DIRTY_BIT_WIDTH_SHIFT 3	//8 bit, shift is 3
#define DIRTY_BIT_WIDTH_MASK (0x7)
	u8 mark[(MXT_OBJECTS_INITIALIZE_LIST_NUM + DIRTY_BIT_WIDTH_MASK) >> DIRTY_BIT_WIDTH_SHIFT];	//Mask the config change each bit
} dirty_marker_t;
#endif

typedef struct config_manager {
	/* Interface from  TSL */
	const tsl_interface_info_t *tsl;
	
	mpt_api_callback_t *api;

#ifdef OBJECT_WRITEBACK
	/* config whether dirty */
	dirty_marker_t dirty;
#endif
} config_manager_t;

config_manager_t chip_config_manager;

static void mpt_chip_reset(void);
static ssint mpt_chip_backup(/*data_crc24_t*/ void *crc_ptr);
static void mpt_chip_calibrate(void);
static void mpt_chip_reportall(void);
static void mpt_chip_get_config_crc(/*data_crc24_t*/ void *ptr);
static ssint mpt_chip_load_config(void);
static void mpt_chip_assert_irq(u8 assert, bool retrigger);
#ifdef OBJECT_T5
static void init_buffer(message_buffer_t *buf);
static u8 message_count(const mxt_message_fifo_t *msg_fifo);
#endif
/* Will lock FIFO in write operation */
static ssint mpt_write_message(const /*object_t5_t*/void *msg_ptr);
/* Will lock FIFO in read operation */
static ssint mpt_read_message(object_t5_t *msg);
static u8 get_report_id(const mxt_object_t *ibots, u8 regid);

mpt_api_callback_t mpt_api_info = {
#ifdef OBJECT_T6
	.reset = mpt_chip_reset,
	.calibrate = mpt_chip_calibrate,
	.backup = mpt_chip_backup,
	.report_all = mpt_chip_reportall,
	.get_config_crc = mpt_chip_get_config_crc,
	.assert_irq = mpt_chip_assert_irq,
#endif
#ifdef OBJECT_T5
	.write_message = mpt_write_message,
#endif
};

ssint mpt_api_chip_init(const void *tsl_ptr)
{
	const tsl_interface_info_t *tsl = (const tsl_interface_info_t *)tsl_ptr;
	mxt_info_t *ibinf = &ib_id_information;
	mxt_object_t *ibots = &ib_objects_tables[0];
	data_crc24_t *ibcrc = &ib_info_crc;
#ifdef OBJECT_T5
	mxt_message_fifo_t *msg_fifo = &message_fifo;
	message_buffer_t *msg_cache = &message_caches[0];
#endif
	mpt_api_callback_t *api = &mpt_api_info;
	config_manager_t *cfm = &chip_config_manager;
	
	const crc_data_blocks_t dblocks[] = { {(u8 *)ibinf, sizeof(*ibinf)}, {(u8 *)ibots, MXT_OBJECTS_TABLE_SIZE} };
	const object_callback_t *ocbs = &object_initialize_list[0];
	u32 crc;
	u16 offset;
	u8 reportid, i;
	
	// Save TSL interface
	cfm->tsl = tsl;
	cfm->api = api;
	cfm->api->qtapi = cfm->tsl->api;
	// Build ID Information
	ibinf->matrix_xsize = cfm->tsl->qtdef->matrix_nodes[NODE_X].size;
	ibinf->matrix_ysize = cfm->tsl->qtdef->matrix_nodes[NODE_Y].size;
	ibinf->object_num = MXT_OBJECTS_NUM;
	
	// Build Objects tables
	for (i = 0, offset = 0; i < ibinf->object_num; i++) {
		ibots[i].start_address = MXT_OBJECTS_START + offset;
		offset += (ibots[i].size_minus_one + 1) * (ibots[i].instances_minus_one + 1);
	}
	
	// Calculate Information Block CRC
	crc = calc_blocks_crc24(dblocks, ARRAY_SIZE(dblocks));
	ibcrc->data[0] = crc & 0xff;
	ibcrc->data[1] = (crc >> 8) & 0xff;
	ibcrc->data[2] = (crc >> 16) & 0xff;
	
#ifdef OBJECT_T5
	// Create message FIFO
	INIT_LIST_HEAD(&msg_fifo->fifo);
	for (i = 0; i < MXT_MESSAGE_FIFO_SIZE; i++) {
		// set message buffer unused
		init_buffer(&msg_cache[i]);
	}
#endif
	
	// Initialize each object
	for (i = 0; i < MXT_OBJECTS_INITIALIZE_LIST_NUM; i++) {
		if (ocbs[i].init) {
			reportid = get_report_id(ibots, ocbs[i].type);
			ocbs[i].init(reportid, cfm->tsl->qtdef, ocbs[i].mem, cfm->api);
		}
	}
		
	return 0;
}

/**
 * \brief MPTT framework start, 
	load config and start the object
 * @Return: None
 */
void mpt_api_chip_start(void)
{
	const object_callback_t *ocbs = &object_initialize_list[0];
	u8 i;
	ssint result;
	
#ifdef OBJECT_T6
	result = mpt_chip_load_config();
	if (result) {
		object_api_t6_set_status(MXT_T6_STATUS_CFGERR);
	}
#endif
	
	// Run each object
	for (i = 0; i < MXT_OBJECTS_INITIALIZE_LIST_NUM; i++) {
		if (ocbs[i].start)
			ocbs[i].start(result ? 0 : 1);
	}
}

/**
 * \brief MPTT framework hardware check, 
	include fuse check and pin fault/avdd test
 * @Return: Zero means normal, other value means pinfault issue
 */
ssint mpt_api_chip_test(void)
{
#ifdef OBJECT_T6
#ifdef MPTT_FUSE_CHECK
	if (tsapi_fuse_check()) {
		object_api_t6_set_status(MXT_T6_STATUS_SIGERR);
	}
#endif
#endif

	return object_api_t25_pinfault_test();
}

void mem_readback(u8 regid)
{
	const object_callback_t *ocbs = &object_initialize_list[0];
	u8 i;
		
	for (i = 0; i < MXT_OBJECTS_INITIALIZE_LIST_NUM; i++) {
		if (ocbs[i].type == regid) {
			if (ocbs[i].sync)
				ocbs[i].sync(OP_READ);
			break;
		}
	}
}

#ifdef OBJECT_WRITEBACK
void mem_mark_dirty(u8 regid)
{
	dirty_marker_t *dirty = &chip_config_manager.dirty;
	const object_callback_t *ocbs = &object_initialize_list[0];
	u8 i, j, k;

	for (i = 0; i < MXT_OBJECTS_INITIALIZE_LIST_NUM; i++) {
		if (ocbs[i].type == regid) {
			j = i >> DIRTY_BIT_WIDTH_SHIFT;
			k = i & DIRTY_BIT_WIDTH_MASK;
			dirty->mark[j] |= BIT(k);
			break;
		}
	}
}

void mem_writeback(void)
{
	dirty_marker_t *dirty = &chip_config_manager.dirty;
	const object_callback_t *ocbs = &object_initialize_list[0];
	u8 i, j, k;
    bool flush = false;	

	// Run each object
	for (i = 0; i < MXT_OBJECTS_INITIALIZE_LIST_NUM; i++) {
		j = i >> DIRTY_BIT_WIDTH_SHIFT;
		k = i & DIRTY_BIT_WIDTH_MASK;
		if ((dirty->mark[j] >> k) & 0x1) {
			if (ocbs[i].sync) {
				ocbs[i].sync(OP_WRITE);
				dirty->mark[j] &= ~BIT(k);
                flush = true;
			}
		}
	}
	
	// Readback sync many objects share share tsl parameter
    if (flush) {
	    for (i = 0; i < MXT_OBJECTS_INITIALIZE_LIST_NUM; i++) {
		    if (ocbs[i].sync)
			    ocbs[i].sync(OP_READ);
	    }
    }
}
#endif

void mpt_api_process(void)
{
	/*
	LOCK();
		...
		
	UNLOCK();
	*/
}

void mpt_api_writeback(void) 
{
#ifdef OBJECT_T6
	object_api_t6_handle_command();
#endif
#ifdef OBJECT_WRITEBACK
	mem_writeback();
#endif
}

static void mpt_chip_reset(void)
{
	const config_manager_t *cfm = &chip_config_manager;
	
	if (cfm->tsl->hal->reset)
		cfm->tsl->hal->reset();
}

static void mpt_chip_calibrate(void)
{
	const config_manager_t *cfm = &chip_config_manager;
	
	if (cfm->tsl->api->calibrate)
		cfm->tsl->api->calibrate();
}

static ssint mpt_chip_backup(/*data_crc24_t*/ void *crc_ptr)
{
	const config_manager_t *cfm = &chip_config_manager;
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	const data_crc24_t *ibcrc = &ib_info_crc;
	u32 crc;
	ssint result;

	// Whatever we have save interface, we need calculate crc
	crc = calc_crc24((u8 *)&ibreg->cfg, offsetof(/* typeof(ibreg->cfg) */objects_config_t, crc));
	ibreg->cfg.crc.data[0] = crc & 0xff;
	ibreg->cfg.crc.data[1] = (crc >> 8) & 0xff;
	ibreg->cfg.crc.data[2] = (crc >> 16) & 0xff;

	ibreg->cfg.ib.value = ibcrc->value;

	if (!cfm->tsl->hal->save_cfg)
		return -2;
	
	result = cfm->tsl->hal->save_cfg((u8 *)&ibreg->noncfg,  sizeof(ibreg->noncfg) + sizeof(ibreg->cfg));
	if (result) {
		/* something error */
		result = -3;
	} else {
		if (crc_ptr) {
			//((data_crc24_t *)crc_ptr)->value = ibreg->cfg.crc.value;
			memcpy(crc_ptr, &ibreg->cfg.crc, sizeof(ibreg->cfg.crc));
		}
		
		object_api_t6_clr_status(MXT_T6_STATUS_CFGERR);
	}
	
	return result;
}

/**
 * \brief Load object config from memory 
	do crc check after loaded, zero the config if failed
 * @Return: Zero means normal, other value means something error detected
 */
static ssint mpt_chip_load_config(void)
{
#ifdef MPTT_SAVE_CONFIG
	const config_manager_t *cfm = &chip_config_manager;
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	const data_crc24_t *ibcrc = &ib_info_crc;
	u32 crc;
	ssint result = -2;

	if (!cfm->tsl->hal->load_cfg)
		return -2;
		
	result = cfm->tsl->hal->load_cfg((u8 *)&ibreg->noncfg,  sizeof(ibreg->noncfg) + sizeof(ibreg->cfg));
	if (result) {
		/* something error */
		result = -3;
	} else if (ibreg->cfg.ib.value != ibcrc->value) {	// Check FW match the config
		result = -4;
	} else {
		crc = calc_crc24((u8 *)&ibreg->cfg,  offsetof(/* typeof(ibreg->cfg) */objects_config_t, crc));
		if (crc != ibreg->cfg.crc.value) {
			/* crc mismatch */
			result = -5;
		}
	}

	if (result)
		memset(&ibreg->noncfg, 0,  sizeof(ibreg->noncfg) + sizeof(ibreg->cfg));
	
	return result;
#else
	return -2;
#endif
}

static void mpt_chip_reportall(void)
{
	const object_callback_t *ocbs = &object_initialize_list[0];
	u8 i;
	
	// Report each object
	for (i = 0; i < MXT_OBJECTS_INITIALIZE_LIST_NUM; i++) {
		if (ocbs[i].report)
			ocbs[i].report(1);
	}
}

void mpt_api_report_status(void)
{
	const object_callback_t *ocbs = &object_initialize_list[0];
	u8 i;
	
	// Report object status if necessary
	for (i = 0; i < MXT_OBJECTS_INITIALIZE_LIST_NUM; i++) {
		if (ocbs[i].report)
			ocbs[i].report(0);
	}
}

static void mpt_chip_get_config_crc(/*data_crc24_t*/ void *ptr)
{
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	
	memcpy(ptr, &ibreg->cfg.crc, sizeof(ibreg->cfg.crc));
}

static void mpt_chip_assert_irq(u8 assert, bool retrigger)
{
	const config_manager_t *cfm = &chip_config_manager;
	
	if (cfm->tsl->hal->assert_irq)
		cfm->tsl->hal->assert_irq(assert, retrigger);	
}

static const mxt_object_t *ib_get_object(const mxt_object_t *ibots, u8 regid) 
{
	const mxt_object_t *obj;
	u8 i;
	
	for (i = 0; i < MXT_OBJECTS_NUM; i++) {
		obj = &ibots[i];
		if (obj->type == regid) {
			return obj;
		}
	}
	
	return NULL;
}

static const mxt_object_t *ib_get_object_by_address(const mxt_object_t *ibots, u16 addr)
{
	const mxt_object_t *obj;
	u8 i;
	
	for (i = 0; i < MXT_OBJECTS_NUM; i++) {
		obj = &ibots[i];
		if (addr >= obj->start_address && addr < obj->start_address + (obj->size_minus_one + 1) * (obj->instances_minus_one + 1)) {
			return obj;
		}
	}
	
	return NULL;
}

static u8 get_report_id(const mxt_object_t *ibots, u8 regid)
{
	const mxt_object_t *obj;
	u8 reportid = MXT_REPORT_ID_START;
	u8 i;
	
	for (i = 0; i < MXT_OBJECTS_NUM; i++) {
		obj = &ibots[i];
		if (obj->type == regid) {
			if (obj->num_report_ids)
				return reportid;
			else
				return MXT_RPTID_NOMSG;
		}
		reportid += obj->num_report_ids * (obj->instances_minus_one + 1);
	}
	
	return MXT_RPTID_NOMSG;
}

#ifdef OBJECT_T5
static message_buffer_t *pop_message_fifo(u8 rid)
{
	mxt_message_fifo_t *msg_fifo = &message_fifo;
	message_buffer_t *buf = NULL;
	struct list_head *ptr, *head = &msg_fifo->fifo;;
	
	if (list_empty(head))
		return NULL;
	
	if (!CHECK_REPORT_ID(rid)) {
		buf = list_first_entry(head, struct message_buffer, node);	
	} else {
		list_for_each(ptr, head) {
			buf = container_of(ptr, struct message_buffer, node);
			if (buf->message.reportid == rid)
				break;
		}
		
		if (ptr->next == head)
			buf = NULL;
	}
	
	if (CHECK_REPORT_ID(buf->message.reportid))
		msg_fifo->reporter[buf->message.reportid - MXT_REPORT_ID_START]--;
	msg_fifo->count--;
	
	//  Not delete here, call destory_buffer when you finished using
	//	list_del(&buf->node);	
	
	return buf;
}

static void push_message_fifo(message_buffer_t *buf)
{
	mxt_message_fifo_t *msg_fifo = &message_fifo;
	struct list_head *head = &msg_fifo->fifo;

	if (!CHECK_REPORT_ID(buf->message.reportid))
		return;

	list_add_tail(&buf->node, head);
	
	msg_fifo->count++;
	msg_fifo->reporter[buf->message.reportid - MXT_REPORT_ID_START]++;
}

static bool buffer_empty(message_buffer_t *buf)
{
	if (list_empty(&buf->node))
		return true;
	
	return false;
}

static void init_buffer(message_buffer_t *buf)
{
	INIT_LIST_HEAD(&buf->node);
}

void destory_buffer(message_buffer_t *buf)
{
	list_del_init(&buf->node);
	memset(&buf->message, 0,  sizeof(buf->message));
}

static message_buffer_t *alloc_message_buffer(void)
{
	mxt_message_fifo_t *msg_fifo = &message_fifo;
	message_buffer_t *buf, *msg_caches = &message_caches[0];
	u8 i, count = 0;
	u8 rid = 0;

	//First search whether empty buffer
	for (i = 0; i < MXT_MESSAGE_FIFO_SIZE; i++) {
		buf = &msg_caches[i];
		if (buffer_empty(buf))
			return buf;
	}

	//No empty, pop max count reporter
	for (i = 0; i < MXT_REPORT_ID_COUNT; i++) {
		if (count < msg_fifo->reporter[i]) {
			count = msg_fifo->reporter[i];
			rid = MXT_REPORT_ID_START + i;
		}
	}
	
	//Pop the rid of first position
	buf = pop_message_fifo(rid);
	if (buf) {
		destory_buffer(buf);
		return buf;
	}
	
	return NULL;
}

static u8 message_count(const mxt_message_fifo_t *msg_fifo)
{
	const struct list_head *head = &msg_fifo->fifo;
	struct list_head *ptr;
	u8 count = 0;

	//FIXME: Walk list for count of direct get? return msg_fifo->count;
	
	list_for_each(ptr, head)
		count++;
	
	return count;
}

u8 mpt_get_message_count(void) 
{
	mxt_message_fifo_t *msg_fifo = &message_fifo;
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	u8 count = 0;
	
	LOCK();
	
	count = message_count(msg_fifo);
	// If T5 has cached data last time, 1 more message when `count` available
	if (ibreg->ram.t5.reportid != MXT_RPTID_NOMSG) {
		count++;
	}
		
	UNLOCK();
	
	return count;
}
#endif

u8 mpt_api_request_irq(void)
{
	u8 count = 0;
	
#ifdef OBJECT_T5
	bool retrigger = false;

#ifdef OBJECT_T6	
	if (object_t6_check_chip_critical())
		return count;
#endif
		
	count = mpt_get_message_count();
	
#if OBJECT_T18
	retrigger = object_t18_check_retrigger();
#endif		
	mpt_chip_assert_irq(count, retrigger);
#endif

	return count;
}

#ifdef OBJECT_T5
static ssint mpt_read_message(object_t5_t *msg)
{
	message_buffer_t *buf;
#if (defined(OBJECT_T6) && defined(OBJECT_T7) && defined(OBJECT_T7_OVERFLOW))
	mxt_message_fifo_t *msg_fifo = &message_fifo;
	u8 count = 0;
#endif
	int result = 0;
	
	LOCK();
	
	//Pop first message from FIFO
	buf = pop_message_fifo(0);	// Note this function will Pop one message to target ram
	if (buf) {
		memcpy(msg, &buf->message, sizeof(*msg));
		destory_buffer(buf);
#ifdef OBJECT_T44
#if (defined(OBJECT_T6) && defined(OBJECT_T7) && defined(OBJECT_T7_OVERFLOW))
	if (object_t7_report_overflow()) {
		count = message_count(msg_fifo);
		if (count < MXT_MESSAGE_FIFO_SIZE - 1) {	//at lease 2 empty spaces
			object_api_t6_clr_status(MXT_T6_STATUS_OFL);
		} else {
			//If FIFO is full, we need send a overflow message
			object_api_t6_set_status(MXT_T6_STATUS_OFL);
		}
	}
#endif
#endif
	} else {
		msg->reportid = MXT_RPTID_NOMSG;
		result = -2;
	}

	UNLOCK();
	
	return result;
}

static ssint mpt_write_message(const /*object_t5_t*/void *msg_ptr) 
{
	const object_t5_t *msg = (const object_t5_t *)msg_ptr;
	message_buffer_t *buf;
	ssint result = 0;
	
	LOCK();
	
	buf = alloc_message_buffer();
	if (buf) {
		memcpy(&buf->message, msg, sizeof(*msg));
		push_message_fifo(buf);
	} else {
		result = -2;
	}
	
	UNLOCK();
	
	return result;
}
#endif

/*
    API for register read command
    @baseaddr: the reg address when the command begin
    @offset: the current offset to the begin address
    @out_ptr: read value
    return: < 0: error code; == 0 not executed; > 0 read bytes(always 1)
*/
ssint mpt_api_mem_read(u16 baseaddr, u16 offset, u8 *out_ptr) 
{
	mxt_info_t *ibinf = &ib_id_information;
	mxt_object_t *ibots = &ib_objects_tables[0];
	data_crc24_t *ibcrc = &ib_info_crc;
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	const mxt_object_t *obj;
#ifdef OBJECT_T5	
#ifdef OBJECT_T44
	const mxt_object_t *obj_t44 = ib_get_object(ibots, MXT_SPT_MESSAGECOUNT_T44);
#endif	
	const mxt_object_t *obj_t5 = ib_get_object(ibots, MXT_GEN_MESSAGE_T5);
	u8 size = 0, checksum = 0, discard = 0;
#endif	
	u16 regaddr;
	u8 *dst = NULL;
	ssint result = 0;
	
	LOCK();

#ifdef OBJECT_T5
#ifdef OBJECT_T44
	if (baseaddr == obj_t44->start_address) {
		if (offset > 0) {   //That mean T5
			baseaddr++;
			offset--;
		} else {
			ibreg->ram.t44.count = mpt_get_message_count();
		}
	}
#endif
	//If start address is T5 address, that means looping read message fifo
	if (baseaddr == obj_t5->start_address) {
		//Check whether CRC read
		/*
		if (baseaddr & T5_MESSAGE_CRC_BIT) {
			baseaddr &= T5_MESSAGE_ADDR_MASK;
			size = obj_t5->size_minus_one + 1;
			checksum = 1;
		} else {
			size = obj_t5->size_minus_one;
		}
		*/
		size = obj_t5->size_minus_one;
		
		/*while(offset > size)	// offset %= obj_t5->size_minus_one + 1
			offset -= size;
		*/
		offset %= size;
	}
#endif
	regaddr = baseaddr + offset;
	
	if (regaddr < MXT_OBJECT_TABLE_START) {
		dst = (u8 *)ibinf + regaddr;
	} else if (regaddr < MXT_INFO_CRC_START) {
		dst = (u8 *)ibots + regaddr - MXT_OBJECT_TABLE_START;
	} else if (regaddr < MXT_OBJECTS_START) {
		dst = (u8 *)ibcrc + regaddr - MXT_INFO_CRC_START;
	} else if (regaddr < MXT_MEMORY_END) {
#ifdef OBJECT_T5		
		/*
			If T5 ram hasn't get message, pop message data from buffer,
			After message sent out, mark T5 ram out date 
		*/
		
		if (baseaddr == obj_t5->start_address) {
			if (offset == 0) {	// First byte in T5 memory, reload message if read from at T5 and msg is invalid
				if (ibreg->ram.t5.reportid == MXT_RPTID_NOMSG) {
					/* Pop one message to T5 memory if T5 ram message is invalid, 
						Note the message has been removed from Fifo trail
					*/
					mpt_read_message(&ibreg->ram.t5);
				}
				mpt_chip_assert_irq(0, false);	//Release CHG line as protocol, FIXME: release CHG before first byte transferred, which different with protocol
			} else if (offset == size - 1) {	// Last byte in T5 memory
				if (checksum) {
					ibreg->ram.t5.crc = calc_crc8((u8 *)&ibreg->ram.t5, size -1);
				}
				discard = 1;
			}
		} else 
#endif		
		{
			//Sync data with qtlib
			if (regaddr >= MXT_OBJECTS_CFG_START) {
				obj = ib_get_object_by_address(ibots, regaddr);
				if (obj) {
					if (obj->start_address == regaddr)
						mem_readback(obj->type);
				}
			}
		}
		dst = (u8 *)ibreg + regaddr - MXT_OBJECTS_START;	
	} else {
		/* Address out of range */
	}
	
	if (dst) {	
		*out_ptr = *dst; // memcpy(out_ptr, dst, 1);
#ifdef OBJECT_T5
		if (discard)
			ibreg->ram.t5.reportid = MXT_RPTID_NOMSG;
#endif
        result = 1;
	} else {
		/* Address out of range */
		result = -2;
	}
	
	UNLOCK();
		
	return result;
}

/*
    API for register write command
    @baseaddr: the reg address when the command begin
    @offset: the current offset to the begin address
    @val: write value
    return: < 0: error code; == 0 not executed; > 0 writed bytes(always 1)
*/
ssint mpt_api_mem_write(u16 baseaddr, u16 offset, u8 val) 
{
	mxt_object_t *ibots = &ib_objects_tables[0];
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	u8 *dst;
	const mxt_object_t *obj;
	ssint result = 0;
	const u16 regaddr = baseaddr + offset;
	LOCK();
	
	// Only write config memory area
	if (regaddr < MXT_OBJECTS_START) {
		/* No write access */
		result = /*-2*/0;   //Whatever, compatible with QTServer
	} else if (regaddr < MXT_OBJECTS_CTRL_START) {
		/* No write access for ram area */
		result = /*-3*/0;   //Whatever, just not handle it
	} else if (regaddr < MXT_MEMORY_END) {
		/* Config area */
		obj = ib_get_object_by_address(ibots, regaddr);
		if (obj) {
			dst = (u8 *)&ibreg->noncfg + regaddr - MXT_OBJECTS_NONCFG_START;
			if (dst[0] != val) {
				dst[0] = val; //memcpy(dst, &val, 1);
#ifdef OBJECT_WRITEBACK			
				mem_mark_dirty(obj->type/*, (u8)!offset*/);
#endif		
			}
            result = 1;
		}
	} else {
		/* Address out of range */
		result = -6;
	}
	
	UNLOCK();

	return result;
}

/**
 * \notice objects pre-work before touch process, 
 */
void mpt_api_pre_process(void)
{

#ifdef OBJECT_T25
	object_api_t25_selftest((u8)-1, NULL);
#endif

#ifdef OBJECT_T109
	object_t109_param_sync();
#endif
}

/*
    API for dispatch sensor ref/delta/cap(rsd) data
    @channel: sensor channel number
    @cv: the `rsd` data, if import Null, that means to test whether `rsd` data is required
    return: < 0 the rsd data is not required currently; 0 rsd data is processed.
*/

ssint mpt_api_set_sensor_data(u8 channel, /*const cap_sample_value_t * const*/ const void * cv)
{
	ssint checked = -1;
	ssint result;
	
#ifdef OBJECT_T37
	result = object_api_t37_set_sensor_data(channel, cv);
	if (result == 0) {
		checked = 0;
	}
#endif

#ifdef OBJECT_T25
	result = object_api_t25_selftest(channel, cv);
	if (result == 0) {
		checked = 0;
	}
#endif

#ifdef OBJECT_T109
	result = object_api_t109_set_sensor_data(channel, cv);
	if (result == 0) {
		checked = 0;
	}
#endif

	return checked;
}

void mpt_api_set_button_status(u8 id, u8 status)
{
#ifdef OBJECT_T15
	object_api_t15_set_button_status(id, status);
#endif
}

void mpt_api_set_pointer_location(u8 type, u8 id, u8 status, u16 x, u16 y)
{
#ifdef OBJECT_T9
	object_api_t9_set_pointer_location(type, id, status, x, y);
#endif
}

void mpt_api_set_chip_status(u8 mask, u8 set)
{
#ifdef OBJECT_T6
	if (set)
		object_api_t6_set_status(mask);
	else
		object_api_t6_clr_status(mask);
#endif
}

u8 mpt_api_get_selftest_op(void)
{
#ifdef OBJECT_T25
	return object_api_t25_get_test_op();
#else
	return 0;
#endif
}