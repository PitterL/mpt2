/*
 * mpt2.c
 *
 * Created: 6/7/2019 9:48:06 PM
 *  Author: A41450
 */ 

#include <compiler.h>
#include <string.h>
#include "mptt.h"
#include "mutex.h"
#include "crc.h"
#include "list.h"

typedef struct mxt_info {
	u8 family_id;
	u8 variant_id;
	u8 version;
	u8 build;
	u8 matrix_xsize;
	u8 matrix_ysize;
	u8 object_num;
} __attribute__ ((packed)) mxt_info_t;

typedef struct mxt_object {
	u8 type;
	u16 start_address;
	u8 size_minus_one;
	u8 instances_minus_one;
	u8 num_report_ids;
} __attribute__ ((packed)) mxt_object_t;

mxt_info_t ib_id_information = {
	MPTT_FW_FAMILY_ID,	/* family_id */
	MPTT_FW_VARIANT_ID,	/* variant_id */
	MPTT_FW_VERSION,	/* version */
	MPTT_FW_BUILD,	/* build */
};
#define MXT_ID_INFORMATION_SIZE (sizeof(ib_id_information))

/* Should add all report count here */
#define MXT_REPORT_ID_COUNT (MXT_GEN_COMMAND_T6_RIDS + MXT_TOUCH_MULTI_T9_RIDS + MXT_TOUCH_KEYARRAY_T15_RIDS + MXT_SPT_SELFTEST_T25_RIDS)
#define MXT_MESSAGE_DEPTH_EACH_RID 2
#define MXT_MESSAGE_FIFO_SIZE (MXT_REPORT_ID_COUNT * MXT_MESSAGE_DEPTH_EACH_RID)

typedef struct message_buffer{
	object_t5_t message;
	struct list_head node;
}message_buffer_t;
message_buffer_t message_cache[MXT_MESSAGE_FIFO_SIZE];

typedef struct mxt_message_fifo {
	struct list_head fifo;
	u8 reporter[MXT_REPORT_ID_COUNT];	//message count for each reporter
	u8 count;	//message sum
	mutex_t lock;
} mxt_message_fifo_t;

mxt_message_fifo_t message_fifo;

data_crc24_t ib_info_crc;
#define MXT_INFOBLOCK_CRC_SIZE (sizeof(ib_info_crc))

typedef struct objects_ram {
	object_t37_t r37;
	object_t44_t t44;
	object_t5_t t5;
} __attribute__ ((packed)) objects_ram_t;

#define MXT_OBJECTS_RAM_SIZE (sizeof(struct objects_ram))

typedef struct objects_ctrl {
	object_t6_t t6;	
} __attribute__ ((packed)) objects_ctrl_t;

#define MXT_OBJECTS_CTRL_SIZE (sizeof(struct objects_ctrl))

typedef struct objects_config {
	object_t7_t t7;
	object_t8_t t8;
	object_t9_t t9_objs[MXT_TOUCH_MULTI_T9_INST];
	object_t15_t t15_objs[MXT_TOUCH_KEYARRAY_T15_INST];
	object_t18_t t18;
	object_t25_t t25;
	object_t104_t t104;
	object_t111_t t111;
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
	{	MXT_DEBUG_DIAGNOSTIC_T37, /*start_address*/-1, sizeof(struct object_t37) - 1, /*instances_minus_one*/0, /*num_report_ids*/0	},
	{	MXT_SPT_MESSAGECOUNT_T44, /*start_address*/-1, sizeof(struct object_t44) - 1, /*instances_minus_one*/0, /*num_report_ids*/0	},
	{	MXT_GEN_MESSAGE_T5, /*start_address*/-1, sizeof(struct object_t5) - 1, /*instances_minus_one*/0, /*num_report_ids*/0	},
	{	MXT_GEN_COMMAND_T6, /*start_address*/-1, sizeof(struct object_t6) - 1, /*instances_minus_one*/0, /*num_report_ids*/MXT_GEN_COMMAND_T6_RIDS	},
	{	MXT_GEN_POWER_T7, /*start_address*/-1, sizeof(struct object_t7) - 1, /*instances_minus_one*/0, /*num_report_ids*/0	},
	{	MXT_GEN_ACQUIRE_T8, /*start_address*/-1, sizeof(struct object_t8) - 1, /*instances_minus_one*/0, /*num_report_ids*/0	},
	{	MXT_TOUCH_MULTI_T9, /*start_address*/-1, sizeof(struct object_t9) - 1, /*instances_minus_one*/MXT_TOUCH_MULTI_T9_INST - 1, /*num_report_ids*/MXT_TOUCH_MULTI_T9_RIDS	},
	{	MXT_TOUCH_KEYARRAY_T15, /*start_address*/-1, sizeof(struct object_t15) - 1, /*instances_minus_one*/MXT_TOUCH_KEYARRAY_T15_INST - 1, /*num_report_ids*/MXT_TOUCH_KEYARRAY_T15_RIDS	},
	{	MXT_SPT_COMMSCONFIG_T18, /*start_address*/-1, sizeof(struct object_t18) - 1, /*instances_minus_one*/0, /*num_report_ids*/0	},
	{	MXT_SPT_SELFTEST_T25, /*start_address*/-1, sizeof(struct object_t25) - 1, /*instances_minus_one*/0, /*num_report_ids*/MXT_SPT_SELFTEST_T25_RIDS	},
	{	MXT_SPT_AUXTOUCHCONFIG_T104, /*start_address*/-1, sizeof(struct object_t104) - 1, /*instances_minus_one*/0, /*num_report_ids*/0	},
	{	MXT_SPT_SELFCAPCONFIG_T111, /*start_address*/-1, sizeof(struct object_t111) - 1, /*instances_minus_one*/MXT_SPT_SELFCAPCONFIG_T111_INST - 1, /*num_report_ids*/0	},
};

#define MXT_OBJECTS_NUM (ARRAY_SIZE(ib_objects_tables))
#define MXT_OBJECTS_TABLE_SIZE (sizeof(ib_objects_tables))

typedef struct object_callback {
	u8 type;
	int (*init)(u8 rid, const /*sensor_config_t*/void *, void *);
	void (*start)(void);
	void (*report)(void);
	void *mem;	// Reg memory pointer
} object_callback_t;

object_callback_t object_initialize_list[] = {
	{	MXT_DEBUG_DIAGNOSTIC_T37, object_t37_init, NULL, NULL, (void *)&ib_objects_reg.ram.r37	}, 
	{	MXT_SPT_MESSAGECOUNT_T44	},
	{	MXT_GEN_MESSAGE_T5, object_t5_init, NULL, NULL, (void *)&ib_objects_reg.ram.t5	},
	{	MXT_GEN_COMMAND_T6, object_t6_init,	object_t6_start, object_t6_report_status, (void *)&ib_objects_reg.ctrl.t6	},	
	{	MXT_GEN_POWER_T7	},	
	{	MXT_GEN_ACQUIRE_T8	},	
	{	MXT_TOUCH_MULTI_T9, object_t9_init, /*object_t9_start*/NULL, object_t9_report_status, (void *)ib_objects_reg.cfg.t9_objs	},	
	{	MXT_SPT_COMMSCONFIG_T18	},
	{	MXT_SPT_SELFTEST_T25	},	
};
#define MXT_OBJECTS_INITIALIZE_LIST_NUM (ARRAY_SIZE(object_initialize_list))

#define MXT_ID_INFORMATION_START 0
#define MXT_OBJECT_TABLE_START (MXT_ID_INFORMATION_START + MXT_ID_INFORMATION_SIZE)
#define MXT_INFO_CRC_START (MXT_OBJECT_TABLE_START + MXT_OBJECTS_TABLE_SIZE)
#define MXT_OBJECTS_START (MXT_INFO_CRC_START + MXT_INFOBLOCK_CRC_SIZE)
#define MXT_OBJECTS_RAM_START (MXT_OBJECTS_START + offsetof(mxt_objects_reg_t, ram))
#define MXT_OBJECTS_CTRL_START (MXT_OBJECTS_START + offsetof(mxt_objects_reg_t, ctrl))
#define MXT_OBJECTS_CFG_START (MXT_OBJECTS_START + offsetof(mxt_objects_reg_t, cfg))
#define MXT_MEMORY_END (MXT_OBJECTS_START + MXT_OBJECTS_SIZE)

hal_interface_info_t hal_interface;
sensor_config_t sensor_default_config;

u8 get_report_id(const mxt_object_t *ibots, u8 regid);
void init_buffer(message_buffer_t *buf);

int mpt_init(const hal_interface_info_t *hal)
{
	mxt_info_t *ibinf = &ib_id_information;
	mxt_object_t *ibots = &ib_objects_tables[0];
	data_crc24_t *ibcrc = &ib_info_crc;
	mxt_message_fifo_t *msg_fifo = &message_fifo;
	message_buffer_t *msg_cache = message_cache;
	sensor_config_t *cfg = &sensor_default_config;
	
	const crc_data_blocks_t dblocks[] = { {(u8 *)ibinf, sizeof(*ibinf)}, {(u8 *)ibots, MXT_OBJECTS_TABLE_SIZE} };
	const object_callback_t *ocb = object_initialize_list;
	u32 crc;
	u16 offset;
	u8 reportid, i;
	
	// Save hal interface
	memcpy(&hal_interface, hal, sizeof(*hal));
	
	// Build ID Information
	ibinf->matrix_xsize = hal->matrix_xsize;
	ibinf->matrix_ysize = hal->matrix_ysize;
	ibinf->object_num = MXT_OBJECTS_NUM;
	
	// Build Objects tables
	for (i = 0, offset = 0; i < ibinf->object_num; i++) {
		ibots[i].start_address = MXT_OBJECTS_START + offset;
		offset += (ibots[i].size_minus_one + 1) * (ibots[i].instances_minus_one + 1);
	}
	
	// calculate Information Block CRC
	crc = calc_blocks_crc24(dblocks, ARRAY_SIZE(dblocks));
	ibcrc->data[0] = crc & 0xff;
	ibcrc->data[1] = (crc >> 8) & 0xff;
	ibcrc->data[2] = (crc >> 16) & 0xff;
	
	// Set all message invalid
	INIT_LIST_HEAD(&msg_fifo->fifo);
	mutex_init(&msg_fifo->lock);
	for (i = 0; i < MXT_MESSAGE_FIFO_SIZE; i++)
		init_buffer(&msg_cache[i]);
	
	// Initialize each object
	memset(cfg, 0, sizeof(*cfg));
	cfg->matrix_xsize = hal->matrix_xsize;
	cfg->matrix_ysize = hal->matrix_ysize;
	cfg->measallow = hal->measallow;
	for (i = 0; i < MXT_OBJECTS_INITIALIZE_LIST_NUM; i++) {
		if (ocb[i].init) {
			reportid = get_report_id(ibots, ocb[i].type);
			ocb[i].init(reportid, cfg, ocb[i].mem);
		}
	}
	
	mpt_chip_load_config();
	
	return 0;
}

void mpt_chip_reset(void)
{
	const hal_interface_info_t *hal = &hal_interface;
	
	if (hal->fn_reset)
		hal->fn_reset();
}

int mpt_chip_load_config(void)
{
	const hal_interface_info_t *hal = &hal_interface;
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	int crc;
	int result = -2;

	if (!hal->fn_load_cfg)
		return -2;
		
	result = hal->fn_load_cfg((u8 *)&ibreg->cfg, sizeof(ibreg->cfg));
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
}

int mpt_chip_backup(void)
{
	const hal_interface_info_t *hal = &hal_interface;
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	u32 crc;
	int result;

	if (!hal->fn_save_cfg)
		return -2;

	crc = calc_crc24((u8 *)&ibreg->cfg, offsetof(typeof(ibreg->cfg), crc));
	ibreg->cfg.crc.data[0] = crc & 0xff;
	ibreg->cfg.crc.data[1] = (crc >> 8) & 0xff;
	ibreg->cfg.crc.data[2] = (crc >> 16) & 0xff;
	
	result = hal->fn_save_cfg((u8 *)&ibreg->cfg, sizeof(ibreg->cfg));
	if (result) {
		/* something error */
		result = -3;
	}
	
	return result;
}

void mpt_chip_calibrate(void)
{
	const hal_interface_info_t *hal = &hal_interface;
	
	if (hal->fn_calibrate)
		hal->fn_calibrate();
}

void mpt_chip_start(void)
{
	const object_callback_t *ocb = object_initialize_list;
	int i;
	
	// Send a calibration
	object_t6_handle_command(MXT_COMMAND_CALIBRATE, 1);
	
	// Run each object
	for (i = 0; i < MXT_OBJECTS_INITIALIZE_LIST_NUM; i++) {
		if (ocb[i].start)
			ocb[i].start();
	}
}

void mpt_chip_reportall(void)
{
	const object_callback_t *ocb = object_initialize_list;
	int i;
	
	// Report each object
	for (i = 0; i < MXT_OBJECTS_INITIALIZE_LIST_NUM; i++) {
		if (ocb[i].report)
			ocb[i].report();
	}
}

void mpt_chip_get_config_crc(data_crc24_t *ptr)
{
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	
	memcpy(ptr, &ibreg->cfg.crc, sizeof(*ptr));
}

const mxt_object_t *ib_get_object(const mxt_object_t *ibots, u8 regid) 
{
	const mxt_object_t *obj;
	int i;
	
	for (i = 0; i < MXT_OBJECTS_NUM; i++) {
		obj = &ibots[i];
		if (obj->type == regid) {
			return obj;
		}
	}
	
	return NULL;
}

const mxt_object_t *ib_get_object_by_address(const mxt_object_t *ibots, int addr)
{
	const mxt_object_t *obj;
	int i;
	
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
	u8 reportid = 1;
	int i;
	
	for (i = 0; i < MXT_OBJECTS_NUM; i++) {
		obj = &ibots[i];
		if (obj->type == regid) {
			if (obj->num_report_ids)
				return reportid;
			else
				return MXT_RPTID_NOMSG;
		}
		reportid += obj->num_report_ids;
	}
	
	return MXT_RPTID_NOMSG;
}

message_buffer_t *pop_message_fifo(u8 rid)
{
	mxt_message_fifo_t *msg_fifo = &message_fifo;
	message_buffer_t *buf = NULL;
	struct list_head *ptr;
	
	if (list_empty(&msg_fifo->fifo))
		return NULL;
	
	if (rid == 0) {
		buf = list_first_entry(&msg_fifo->fifo, struct message_buffer, node);	
	}else {
		list_for_each(ptr, &msg_fifo->fifo) {
			buf = container_of(ptr, struct message_buffer, node);
			if (buf->message.reportid == rid)
				break;
		}
		
		if (ptr->next == &msg_fifo->fifo)
			buf = NULL;
	}
	
	if (buf->message.reportid == 0)
		return NULL;
		
	if (buf->message.reportid < MXT_REPORT_ID_COUNT)
		msg_fifo->reporter[buf->message.reportid]--;
	msg_fifo->count--;
	//list_del(&buf->node);	
	
	return buf;
}

void push_message_fifo(message_buffer_t *buf)
{
	mxt_message_fifo_t *msg_fifo = &message_fifo;

	if (!buf->message.reportid)
		return;
		
	if (buf->message.reportid >= MXT_REPORT_ID_COUNT)
		return;

	list_add_tail(&buf->node, &msg_fifo->fifo);
	msg_fifo->count++;
	msg_fifo->reporter[buf->message.reportid]++;
}

void init_buffer(message_buffer_t *buf)
{
	INIT_LIST_HEAD(&buf->node);
}

bool buffer_empty(message_buffer_t *buf)
{
	if (list_empty(&buf->node))
		return true;
	
	return false;
}

void destory_buffer(message_buffer_t *buf)
{
	list_del_init(&buf->node);
	memset(&buf->message, 0,  sizeof(buf->message));
}

message_buffer_t *alloc_message_buffer(void)
{
	mxt_message_fifo_t *msg_fifo = &message_fifo;
	message_buffer_t *buf, *msg_cache = message_cache;
	int count = 0;
	u8 rid = 1;
	int i;

	//First search whether empty buffer
	for (i = 0; i < MXT_MESSAGE_FIFO_SIZE; i++) {
		buf = &msg_cache[i];
		if (buffer_empty(buf))
		return buf;
	}

	//No empty, pop max count reporter
	for (i = 0; i < MXT_REPORT_ID_COUNT; i++) {
		if (count < msg_fifo->reporter[i]) {
			count = msg_fifo->reporter[i];
			rid = i;
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
	return msg_fifo->count;
}

u8 mpt_get_message_count(void)
{
	mxt_message_fifo_t *msg_fifo = &message_fifo;
	u8 count;
	
	lock(&msg_fifo->lock);
	
	count = message_count(msg_fifo);
	
	unlock(&msg_fifo->lock);
	
	return count;
}

int mpt_read_message(object_t5_t *msg)
{
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	mxt_message_fifo_t *msg_fifo = &message_fifo;
	message_buffer_t *buf;
		
	lock(&msg_fifo->lock);
	
	//Pop first message from Fifo
	buf = pop_message_fifo(0);
	if (buf) {
		memcpy(msg, &buf->message, sizeof(*msg));
		destory_buffer(buf);
	}else {
		msg->reportid = MXT_RPTID_NOMSG;	
	}
		
	//Update T44
	ibreg->ram.t44.count = message_count(msg_fifo);
	
	unlock(&msg_fifo->lock);
	
	return 0;
}

int mpt_write_message(const object_t5_t *msg) 
{
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	mxt_message_fifo_t *msg_fifo = &message_fifo;
	message_buffer_t *buf;
	
	lock(&msg_fifo->lock);
	
	buf = alloc_message_buffer();
	if (!buf)
		return -2;
	
	memcpy(&buf->message, msg, sizeof(*msg));
	push_message_fifo(buf);
		
	//Update T44
	ibreg->ram.t44.count = message_count(msg_fifo);
	
	unlock(&msg_fifo->lock);
	
	return 0;
}

int handle_object_command(const mxt_object_t *obj, u16 offset, u8 cmd)
{
	int result;
	
	switch(obj->type) {
		case MXT_GEN_COMMAND_T6:
			result = object_t6_handle_command(offset, cmd);
		break;
		default:
			result = -2; 
	}
	
	return result;
}

int mpt_mem_read(u16 baseaddr, u16 offset, u8 *out_ptr) 
{
	mxt_info_t *ibinf = &ib_id_information;
	mxt_object_t *ibots = &ib_objects_tables[0];
	data_crc24_t *ibcrc = &ib_info_crc;
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	const mxt_object_t *obj_t5 = ib_get_object(ibots, MXT_GEN_MESSAGE_T5);
	u16 regaddr;
	u8 *dst = NULL;
	
	//If start address is T5 address, that means looping read message fifo
	if (baseaddr == obj_t5->start_address) {
		offset %= obj_t5->size_minus_one + 1;
	}
	regaddr = baseaddr + offset;
	
	if (regaddr < MXT_OBJECT_TABLE_START) {
		dst = (u8 *)ibinf + regaddr;
	}else if (regaddr < MXT_INFO_CRC_START) {
		dst = (u8 *)ibots + regaddr - MXT_OBJECT_TABLE_START;
	}else if (regaddr < MXT_OBJECTS_START) {
		dst = (u8 *)ibcrc + regaddr - MXT_INFO_CRC_START;
	}else if (regaddr < MXT_MEMORY_END) {
		/*
			If T5 ram hasn't get message, pop message data from buffer,
			After message sent out, mark T5 ram out date 
		*/
		if (regaddr == obj_t5->start_address) {	//Reload message if read from at T5 and msg is invalid
			if (ibreg->ram.t5.reportid == MXT_RPTID_NOMSG) {
				mpt_read_message(&ibreg->ram.t5);
			}
		}else if (regaddr == obj_t5->start_address + obj_t5->size_minus_one) {
			ibreg->ram.t5.reportid = MXT_RPTID_NOMSG;
		}
		dst = (u8 *)ibreg + regaddr - MXT_OBJECTS_START;	
	}else {
		/* Address out of range */
	}
	
	if (!dst)
		return -2;
		
	memcpy(out_ptr, dst, 1);
		
	return 0;
}

int mpt_mem_write(u16 regaddr, u8 val) 
{
	mxt_object_t *ibots = &ib_objects_tables[0];
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	const mxt_object_t *obj;
	u8 *dst;
	int result = 0;
	
	// Only write config memory area
	if (regaddr < MXT_OBJECTS_START) {
		/* No write access */
		result = -2;
	}else if (regaddr < MXT_OBJECTS_CTRL_START) {
		/* No write access for ram area */
		result = -3;
	}else if (regaddr < MXT_OBJECTS_CFG_START) {
		/* Control command */
		obj = ib_get_object_by_address(ibots, regaddr);
		if (obj) {
			result = handle_object_command(obj, regaddr - obj->start_address, val);
		}else {
			result = -4;
		}
	}else if (regaddr < MXT_MEMORY_END) {
		/* Config area */
		dst = (u8 *)&ibreg->cfg + regaddr - MXT_OBJECTS_START;
		memcpy(dst, &val, 1);
	}else {
		/* Address out of range */
		result = -6;
	}

	return result;
}

u8 *get_object_address(u8 regid, u8 instance, u16 offset, u8 size)
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
	
	if (size + offset > obj->size_minus_one + 1)
		return NULL;
	
	regaddr = obj->start_address + (obj->size_minus_one + 1) * instance + offset;
	dst = (u8 *)ibreg + regaddr - MXT_OBJECTS_START;
	
	return dst;
}

int mpt_object_read(u8 regid, u8 instance, u16 offset, u8 *out_ptr, u8 size)
{
	u8 *dst;
	
	dst = get_object_address(regid, instance, offset, size);
	if (!dst)
		return -2;
		
	memcpy(out_ptr, dst, size);
	
	return 0;
}

int mpt_object_write(u8 regid, u8 instance, u16 offset, const u8 *ptr, u8 size)
{
	u8 *dst;
	
	dst = get_object_address(regid, instance, offset, size);
	if (!dst)
		return -2;
	
	memcpy(dst, ptr, size);
	
	return 0;
}

void mpt_set_sensor_data(u8 channel, u8 state, u16 reference, u16 signal, u16 cap)
{
	u8 page;
	const u8 dbgcmd = object_t6_get_diagnostic_status(&page);
	
	object_t37_set_data_page(page);
	object_t37_set_sensor_data(dbgcmd, channel, reference, signal, cap);
}

void mpt_set_pointer_location(u8 id, uint8_t status, uint16_t x, uint16_t y)
{
	object_t9_set_pointer_location(id, status, x, y);
}
