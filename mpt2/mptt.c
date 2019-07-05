/*
 * mpt2.c
 *
 * Created: 6/7/2019 9:48:06 PM
 *  Author: A41450
 */ 

#include <compiler.h>
#include <string.h>
#include "types.h"
#include "mutex.h"
#include "crc.h"
#include "list.h"
#include "tsl.h"
#include "mptt.h"

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
#ifdef OBJECT_T111
	object_t111_t t111;
#endif
	data_crc24_t crc;
} __attribute__ ((packed)) objects_config_t;

#define MXT_OBJECTS_CONFIG_SIZE (sizeof(struct objects_config))

typedef struct mxt_objects_reg {
	objects_ram_t ram;
	objects_ctrl_t ctrl;
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
#ifdef OBJECT_T111	
	{	MXT_SPT_SELFCAPCONFIG_T111, /*start_address*/-1, sizeof(struct object_t111) - 1, /*instances_minus_one*/MXT_SPT_SELFCAPCONFIG_T111_INST - 1, /*num_report_ids*/0	},
#endif
};

#define MXT_OBJECTS_NUM ((u8)ARRAY_SIZE(ib_objects_tables))
#define MXT_OBJECTS_TABLE_SIZE (sizeof(ib_objects_tables))

/* Should add all report count here */
#define MXT_REPORT_ID_START 1
#define MXT_REPORT_ID_COUNT (MXT_GEN_COMMAND_T6_RIDS + \
								MXT_TOUCH_MULTI_T9_RIDS * MXT_TOUCH_MULTI_T9_INST + \
								MXT_TOUCH_KEYARRAY_T15_RIDS * MXT_TOUCH_KEYARRAY_T15_INST +\
								MXT_SPT_SELFTEST_T25_RIDS)
								
#if MXT_REPORT_ID_COUNT > 254
#error "Report id count too large, varible may overflow"
#endif
#define MXT_MESSAGE_DEPTH_EACH_RID 2
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
#define MXT_OBJECTS_CFG_START (MXT_OBJECTS_START + offsetof(mxt_objects_reg_t, cfg))
#define MXT_MEMORY_END (MXT_OBJECTS_START + MXT_OBJECTS_SIZE)

/* Callback interface */
typedef struct object_callback {
	u8 type;	/* Report ID */

	ssint (*init)(u8 rid, const /* (qtouch_config_t *) */const void *, /* mem space */void *, /* (const qtouch_api_callback_t*) */const void *);
	void (*start)(/* load default config */u8 loaded);
	void (*process)(u8 rw);
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
#ifdef OBJECT_T7
	{	MXT_GEN_POWER_T7, object_t7_init, object_t7_start, object_t7_process, NULL, (void *)&ib_objects_reg.cfg.t7	},	
#endif
#ifdef OBJECT_T8	
	{	MXT_GEN_ACQUIRE_T8,	object_t8_init, object_t8_start, object_t8_process, NULL, (void *)&ib_objects_reg.cfg.t8	},	
#endif
//The following project should later initialized than t8, since it request sensor scanning mode
#ifdef OBJECT_T9	
	{	MXT_TOUCH_MULTI_T9, object_t9_init, object_t9_start, object_t9_process, object_t9_report_status, (void *)ib_objects_reg.cfg.t9_objs	},
#endif
#ifdef OBJECT_T15
	{	MXT_TOUCH_KEYARRAY_T15, object_t15_init, object_t15_start, object_t15_process, object_t15_report_status, (void *)ib_objects_reg.cfg.t15_objs	},
#endif
#ifdef OBJECT_T18
	{	MXT_SPT_COMMSCONFIG_T18, object_t18_init, NULL, NULL, NULL, (void *)&ib_objects_reg.cfg.t18},
#endif
#ifdef OBJECT_T25	
	{	MXT_SPT_SELFTEST_T25, object_t25_init, object_t25_start, object_t25_process, object_t25_report_status, (void *)&ib_objects_reg.cfg.t25	},	
#endif
#ifdef OBJECT_T104
	{	MXT_SPT_AUXTOUCHCONFIG_T104,  object_t104_init, object_t104_start, object_t104_process, NULL, (void *)&ib_objects_reg.cfg.t104	},
#endif
#ifdef OBJECT_T111
	{	MXT_SPT_SELFCAPCONFIG_T111, object_t111_init, object_t111_start, object_t111_process, NULL, (void *)&ib_objects_reg.cfg.t111	},
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
	u8 mark[(MXT_OBJECTS_INITIALIZE_LIST_NUM >> 3) + 1];	//Mask the config change each bit
	u8 cacheid;	//cache the dirty regid before marked at dirty array
} dirty_marker_t;
#endif

typedef struct config_manager {
	/* Interface from  TSL */
	const tsl_interface_info_t *tsl;
	
	// Touch default config
	//const qtouch_config_t *def;
	
	mpt_api_callback_t *api;

#ifdef OBJECT_WRITEBACK
	/* config whether dirty */
	dirty_marker_t dirty;
#endif

} config_manager_t;

config_manager_t chip_config_manager;

void mpt_chip_reset(void);
ssint mpt_chip_backup(void);
void mpt_chip_calibrate(void);
void mpt_chip_reportall(void);
void mpt_chip_get_config_crc(/*data_crc24_t*/ void *ptr);
ssint mpt_chip_load_config(void);
void mpt_chip_assert_irq(u8 assert, bool retrigger);
#ifdef OBJECT_T5
ssint mpt_write_message(const /*object_t5_t*/void *msg);
void init_buffer(message_buffer_t *buf);
#endif
#ifdef OBJECT_WRITEBACK
ssint mpt_object_write(u8 regid, u8 instance, u16 offset, const u8 *ptr, u8 size);
#endif
u8 get_report_id(const mxt_object_t *ibots, u8 regid);

mpt_api_callback_t mpt_api_info = {
#ifdef OBJECT_T6	
	.reset = mpt_chip_reset,
	.calibrate = mpt_chip_calibrate,
	.backup = mpt_chip_backup,
	.report_all = mpt_chip_reportall,
	.cb_get_config_crc = mpt_chip_get_config_crc,
	.cb_assert_irq = mpt_chip_assert_irq,
#endif
#ifdef OBJECT_T5
	.cb_write_message = mpt_write_message,
#endif
#ifdef OBJECT_WRITEBACK
	//.cb_object_write = mpt_object_write,
#endif
};

ssint mpt_chip_init(const void *tsl_ptr)
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

void mpt_chip_start(void)
{
	const object_callback_t *ocbs = &object_initialize_list[0];
	u8 i;
	ssint result = -2;

#ifdef OBJECT_T6
	result = mpt_chip_load_config();
#endif

	// Send a calibration
	//object_api_t6_handle_command(MXT_COMMAND_CALIBRATE, 1);
	
	// Run each object
	for (i = 0; i < MXT_OBJECTS_INITIALIZE_LIST_NUM; i++) {
		if (ocbs[i].start)
			ocbs[i].start(result ? 0 : 1);
	}
	
	//mpt_chip_reportall();
}

void mem_readback(u8 regid)
{
	const object_callback_t *ocbs = &object_initialize_list[0];
	u8 i;
		
	for (i = 0; i < MXT_OBJECTS_INITIALIZE_LIST_NUM; i++) {
		if (ocbs[i].type == regid) {
			if (ocbs[i].process)
				ocbs[i].process(OP_READ);
			break;
		}
	}
}

#ifdef OBJECT_WRITEBACK
void mem_mark_dirty(u8 regid, u8 first)
{
	dirty_marker_t *dirty = &chip_config_manager.dirty;
	const object_callback_t *ocbs = &object_initialize_list[0];
	u8 i, j, k;
	
	if (!first || dirty->cacheid == regid)
		return;
	
	for (i = 0; i < MXT_OBJECTS_INITIALIZE_LIST_NUM; i++) {
		if (ocbs[i].type == regid) {
			j = i >> DIRTY_BIT_WIDTH_SHIFT;
			k = i  - (j << DIRTY_BIT_WIDTH_SHIFT);
			dirty->mark[j] |= BIT(k);
			break;
		}
	}
	dirty->cacheid = regid;
}

void mpt_api_process(void)
{
	dirty_marker_t *dirty = &chip_config_manager.dirty;
	const object_callback_t *ocbs = &object_initialize_list[0];
	u8 i, j, k;
	
	LOCK();
	
	//clean cached id
	dirty->cacheid = 0;
		
	// Run each object
	for (i = 0; i < MXT_OBJECTS_INITIALIZE_LIST_NUM; i++) {
		j = i >> DIRTY_BIT_WIDTH_SHIFT;
		k = i  - (j << DIRTY_BIT_WIDTH_SHIFT);
		if ((dirty->mark[j] >> k) & 0x1) {
			if (ocbs[i].process) {
				ocbs[i].process(OP_WRITE);
				dirty->mark[j] &= ~BIT(k);
			}
		}
	}
	
	UNLOCK();
}
#endif

void mpt_chip_reset(void)
{
	config_manager_t *cfm = &chip_config_manager;
	
	if (cfm->tsl->hal->fn_reset)
		cfm->tsl->hal->fn_reset();
}

void mpt_chip_calibrate(void)
{
	config_manager_t *cfm = &chip_config_manager;
	
	if (cfm->tsl->api->calibrate)
		cfm->tsl->api->calibrate();
}

ssint mpt_chip_backup(void)
{
	config_manager_t *cfm = &chip_config_manager;
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	u32 crc;
	ssint result;

	// Whatever we have save interface, we need calculate crc
	crc = calc_crc24((u8 *)&ibreg->cfg, offsetof(typeof(ibreg->cfg), crc));
	ibreg->cfg.crc.data[0] = crc & 0xff;
	ibreg->cfg.crc.data[1] = (crc >> 8) & 0xff;
	ibreg->cfg.crc.data[2] = (crc >> 16) & 0xff;

	if (!cfm->tsl->hal->fn_save_cfg)
		return -2;
	
	result = cfm->tsl->hal->fn_save_cfg((u8 *)&ibreg->cfg, sizeof(ibreg->cfg));
	if (result) {
		/* something error */
		result = -3;
	}
	
	return result;
}

ssint mpt_chip_load_config(void)
{
#ifdef FLASH_SAVE_CONFIG
	config_manager_t *cfm = &chip_config_manager;
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	u32 crc;
	ssint result = -2;

	if (!cfm->tsl->hal->fn_load_cfg)
		return -2;
		
	result = cfm->tsl->hal->fn_load_cfg((u8 *)&ibreg->cfg, sizeof(ibreg->cfg));
	if (result) {
		/* something error */
		result = -3;
	}else {
		crc = calc_crc24((u8 *)&ibreg->cfg, sizeof(ibreg->cfg));
		if (crc) {
			/* crc mismatch */
			result = -4;
		}
	}

	if (result)
		memset(&ibreg->cfg, 0, sizeof(ibreg->cfg));
		
	return result;
#else
	return -2;
#endif
}

void mpt_chip_reportall(void)
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

void mpt_chip_get_config_crc(/*data_crc24_t*/ void *ptr)
{
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	
	memcpy(ptr, &ibreg->cfg.crc, sizeof(ibreg->cfg.crc));
}

void mpt_chip_assert_irq(u8 assert, bool retrigger)
{
	config_manager_t *cfm = &chip_config_manager;
	
	if (cfm->tsl->hal->fn_assert_irq)
		cfm->tsl->hal->fn_assert_irq(assert, retrigger);	
}

const mxt_object_t *ib_get_object(const mxt_object_t *ibots, u8 regid) 
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

const mxt_object_t *ib_get_object_by_address(const mxt_object_t *ibots, u16 addr)
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

u8 get_report_id(const mxt_object_t *ibots, u8 regid)
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
message_buffer_t *pop_message_fifo(u8 rid)
{
	mxt_message_fifo_t *msg_fifo = &message_fifo;
	message_buffer_t *buf = NULL;
	struct list_head *ptr, *head = &msg_fifo->fifo;;
	
	if (list_empty(head))
		return NULL;
	
	if (!CHECK_REPORT_ID(rid)) {
		buf = list_first_entry(head, struct message_buffer, node);	
	}else {
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

void push_message_fifo(message_buffer_t *buf)
{
	mxt_message_fifo_t *msg_fifo = &message_fifo;
	struct list_head *head = &msg_fifo->fifo;

	if (!CHECK_REPORT_ID(buf->message.reportid))
		return;

	list_add_tail(&buf->node, head);
	
	msg_fifo->count++;
	msg_fifo->reporter[buf->message.reportid - MXT_REPORT_ID_START]++;
}

bool buffer_empty(message_buffer_t *buf)
{
	if (list_empty(&buf->node))
		return true;
	
	return false;
}

void init_buffer(message_buffer_t *buf)
{
	INIT_LIST_HEAD(&buf->node);
}

void destory_buffer(message_buffer_t *buf)
{
	list_del_init(&buf->node);
	memset(&buf->message, 0,  sizeof(buf->message));
}

message_buffer_t *alloc_message_buffer(void)
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

u8 message_count(const mxt_message_fifo_t *msg_fifo)
{
	const struct list_head *head = &msg_fifo->fifo;
	struct list_head *ptr;
	u8 count = 0;

	//FIXME: Walk list for count of direct get? return msg_fifo->count;
	
	list_for_each(ptr, head)
		count++;
	
	return count;
}
#endif

void mpt_api_request_irq(void)
{
#ifdef OBJECT_T5
	mxt_message_fifo_t *msg_fifo = &message_fifo;
	u8 count;
	bool retrigger = false;

	LOCK();

#ifdef OBJECT_T6	
	if (object_t6_check_chip_critical())
		count = 0;
	else
#endif
		count = message_count(msg_fifo);
	
#if OBJECT_T18
	retrigger = object_t18_check_retrigger();
#endif		
	mpt_chip_assert_irq(count, retrigger);
	
	UNLOCK();
#endif
}

#ifdef OBJECT_T5
ssint mpt_read_message(object_t5_t *msg)
{
#ifdef OBJECT_T44
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	mxt_message_fifo_t *msg_fifo = &message_fifo;
#endif	
	message_buffer_t *buf;
	int result = 0;
		
	LOCK();
	
	//Pop first message from FIFO
	buf = pop_message_fifo(0);
	if (buf) {
		memcpy(msg, &buf->message, sizeof(*msg));
		destory_buffer(buf);

#ifdef OBJECT_T44
		//Update T44
		ibreg->ram.t44.count = message_count(msg_fifo);
#ifdef OBJECT_T6
		if (ibreg->ram.t44.count < MXT_MESSAGE_FIFO_SIZE - 1) {	//at lease 2 empty spaces
			object_api_t6_clr_status(MXT_T6_STATUS_OFL);
		}else {			
			//If FIFO is full, we need send a overflow message
			object_api_t6_set_status(MXT_T6_STATUS_OFL);
		}
#endif
#endif
	}else {
		msg->reportid = MXT_RPTID_NOMSG;
		result = -2;
	}

	UNLOCK();
	
	return result;
}

ssint mpt_write_message(const /*object_t5_t*/void *msg_ptr) 
{
	const object_t5_t *msg = (const object_t5_t *)msg_ptr;
#ifdef OBJECT_T44
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	mxt_message_fifo_t *msg_fifo = &message_fifo;
#endif
	message_buffer_t *buf;
	ssint result = 0;
	
	LOCK();
	
	buf = alloc_message_buffer();
	if (buf) {
		memcpy(&buf->message, msg, sizeof(*msg));
		push_message_fifo(buf);

#ifdef OBJECT_T44
		//Update T44
		ibreg->ram.t44.count = message_count(msg_fifo);
#endif	
	}else {
		result = -2;
	}
	
	UNLOCK();
	
	return result;
}
#endif

void mpt_api_handle_command(void)
{
#ifdef OBJECT_T6
	object_api_t6_handle_command();
#endif
}

ssint mpt_mem_read(u16 baseaddr, u16 offset, u8 *out_ptr) 
{
	mxt_info_t *ibinf = &ib_id_information;
	mxt_object_t *ibots = &ib_objects_tables[0];
	data_crc24_t *ibcrc = &ib_info_crc;
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	const mxt_object_t *obj;
#ifdef OBJECT_T5	
	const mxt_object_t *obj_t5 = ib_get_object(ibots, MXT_GEN_MESSAGE_T5);
	u8 size = 0, checksum = 0, discard = 0;
#endif	
	u16 regaddr;
	u8 *dst = NULL;
	ssint result = 0;
	
	LOCK();
	
#ifdef OBJECT_T5
	//If start address is T5 address, that means looping read message fifo
	if (baseaddr == obj_t5->start_address) {
		//Check whether CRC read
		if (baseaddr & T5_MESSAGE_CRC_BIT) {
			baseaddr &= T5_MESSAGE_ADDR_MASK;
			size = obj_t5->size_minus_one + 1;
			checksum = 1;
		}else {
			size = obj_t5->size_minus_one;
		}
		while(offset > size)	// offset %= obj_t5->size_minus_one + 1
			offset -= size;
	}
#endif
	regaddr = baseaddr + offset;
	
	if (regaddr < MXT_OBJECT_TABLE_START) {
		dst = (u8 *)ibinf + regaddr;
	}else if (regaddr < MXT_INFO_CRC_START) {
		dst = (u8 *)ibots + regaddr - MXT_OBJECT_TABLE_START;
	}else if (regaddr < MXT_OBJECTS_START) {
		dst = (u8 *)ibcrc + regaddr - MXT_INFO_CRC_START;
	}else if (regaddr < MXT_MEMORY_END) {
#ifdef OBJECT_T5		
		/*
			If T5 ram hasn't get message, pop message data from buffer,
			After message sent out, mark T5 ram out date 
		*/
		
		if (baseaddr == obj_t5->start_address) {
			if (offset == 0) {	// First byte in T5 memory, reload message if read from at T5 and msg is invalid
				if (ibreg->ram.t5.reportid == MXT_RPTID_NOMSG) {
					// Read new message to T5 memory
					mpt_read_message(&ibreg->ram.t5);
				}
			}else if (offset == size - 1) {	// Last byte in T5 memory
				if (checksum) {
					ibreg->ram.t5.crc = calc_crc8((u8 *)&ibreg->ram.t5, size -1);
				}
				discard = 1;
			}
		}else 
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
	}else {
		/* Address out of range */
	}
	
	if (dst) {	
		*out_ptr = *dst; // memcpy(out_ptr, dst, 1);
#ifdef OBJECT_T5
		if (discard)
			ibreg->ram.t5.reportid = MXT_RPTID_NOMSG;
#endif	
	} else {
		/* Address out of range */
		result = -2;
	}
	
	UNLOCK();
		
	return result;
}

ssint mpt_mem_write(u16 baseaddr, u16 offset, u8 val) 
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
		result = -2;
	}else if (regaddr < MXT_OBJECTS_CTRL_START) {
		/* No write access for ram area */
		result = -3;
	}else if (regaddr < MXT_MEMORY_END) {
		/* Config area */
		obj = ib_get_object_by_address(ibots, regaddr);
		if (obj) {
			dst = (u8 *)&ibreg->cfg + regaddr - MXT_OBJECTS_CFG_START;
			dst[0] = val; //memcpy(dst, &val, 1);
#ifdef OBJECT_WRITEBACK			
			mem_mark_dirty(obj->type, (u8)!offset);
#endif		
		}
	}else {
		/* Address out of range */
		result = -6;
	}
	
	UNLOCK();

	return result;
}

u8 *get_object_mem_address(u8 regid, u8 instance, u16 offset, u8 size)
{
	const mxt_object_t *ibots = &ib_objects_tables[0];
	const mxt_objects_reg_t * ibreg = &ib_objects_reg;
	const mxt_object_t *obj;
	u16 regaddr;
	u8 *dst;
	
	obj = ib_get_object(ibots, regid);
	if (!obj)
		return NULL;
	
	if (instance > obj->instances_minus_one)
		return NULL;
	
	if (offset + size > obj->size_minus_one + 1)
		return NULL;
	
	regaddr = obj->start_address + (obj->size_minus_one + 1) * instance + offset;
	dst = (u8 *)ibreg + regaddr - MXT_OBJECTS_START;
	
	return dst;
}

ssint mpt_object_read(u8 regid, u8 instance, u16 offset, u8 *out_ptr, u8 size)
{
	u8 *dst;
	
	dst = get_object_mem_address(regid, instance, offset, size);
	if (!dst)
		return -2;
		
	memcpy(out_ptr, dst, size);
	
	return 0;
}

ssint mpt_object_write(u8 regid, u8 instance, u16 offset, const u8 *ptr, u8 size)
{
	u8 *dst;
	
	dst = get_object_mem_address(regid, instance, offset, size);
	if (!dst)
		return -2;
	
	memcpy(dst, ptr, size);
	
	return 0;
}

void mpt_api_set_sensor_data(u8 channel, u8 state, u16 reference, u16 signal, u16 cap)
{
#ifdef OBJECT_T37
	object_api_t37_set_sensor_data(channel, reference, signal, cap);
#endif

#ifdef OBJECT_T25
	object_api_t25_set_sensor_data(channel, reference, signal, cap);
#endif
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