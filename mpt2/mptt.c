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

#define MXT_REPORT_ID_COUNT (MXT_GEN_COMMAND_T6_RIDS + MXT_TOUCH_MULTI_T9_RIDS)
#define MXT_MESSAGE_DEPTH_EACH_RID 2
typedef struct message_buffer {
	object_t5_t data[MXT_MESSAGE_DEPTH_EACH_RID];
	u8 curr;
} message_buffer_t;

typedef struct mxt_message_fifo {
	message_buffer_t buffer[MXT_REPORT_ID_COUNT];
	u8 wpos;
	u8 rpos;
	mutex_t lock;
} mxt_message_fifo_t;

mxt_message_fifo_t message_fifo;

data_crc24_t ib_info_crc;

typedef struct objects_ram {
	object_t37_t r37;
	object_t44_t t44;
	object_t5_t t5;
} __attribute__ ((packed)) objects_ram_t;

typedef struct objects_ctrl {
	object_t6_t t6;	
} __attribute__ ((packed)) objects_ctrl_t;

typedef struct objects_config {
	object_t7_t t7;
	object_t8_t t8;
	object_t9_t t9_objs[MXT_TOUCH_MULTI_T9_INST];
	data_crc24_t crc;
} __attribute__ ((packed)) objects_config_t;

typedef struct mxt_objects_reg {
	objects_ram_t ram;
	objects_ctrl_t ctrl;
	objects_config_t cfg;
} __attribute__ ((packed)) mxt_objects_reg_t;

mxt_objects_reg_t ib_objects_reg;

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
};

#define MXT_OBJECTS_NUM (ARRAY_SIZE(ib_objects_tables))

typedef struct object_callback {
	u8 type;
	int (*init)(u8 rid);
	int (*start)(void);
	void (*report)(void);
} object_callback_t;

object_callback_t object_initialize_list[] = {
	{	MXT_DEBUG_DIAGNOSTIC_T37, NULL, NULL	}, 
	{	MXT_SPT_MESSAGECOUNT_T44, NULL, NULL	},
	{	MXT_GEN_MESSAGE_T5, NULL, NULL	},
	{	MXT_GEN_COMMAND_T6, object_t6_init,	object_t6_start, object_t6_report_status	},	
	{	MXT_GEN_POWER_T7, NULL, NULL	},	
	{	MXT_GEN_ACQUIRE_T8, NULL, NULL	},	
	{	MXT_TOUCH_MULTI_T9, object_t9_init, object_t9_start, object_t9_report_status	},	
	{	MXT_SPT_COMMSCONFIG_T18, NULL, NULL	},
	{	MXT_SPT_SELFTEST_T25, NULL, NULL	},	
};
#define MXT_OBJECTS_INITIALIZE_LIST_NUM (ARRAY_SIZE(object_initialize_list))

#define MXT_ID_INFORMATION_START 0
#define MXT_OBJECT_TABLE_START (MXT_ID_INFORMATION_START + sizeof(ib_id_information))
#define MXT_INFO_CRC_START (MXT_OBJECT_TABLE_START + sizeof(ib_objects_tables))
#define MXT_OBJECTS_START (MXT_INFO_CRC_START + sizeof(ib_info_crc))
#define MXT_OBJECTS_RAM_START (MXT_OBJECTS_START + offsetof(mxt_objects_reg_t, ram))
#define MXT_OBJECTS_CTRL_START (MXT_OBJECTS_START + offsetof(mxt_objects_reg_t, ctrl))
#define MXT_OBJECTS_CFG_START (MXT_OBJECTS_START + offsetof(mxt_objects_reg_t, cfg))
#define MXT_MEMORY_END (MXT_OBJECTS_START + sizeof(ib_objects_reg))

hal_interface_info_t hal_interface;

u8 get_report_id(mxt_object_t *ibots, u8 regid);

int mpt_init(const hal_interface_info_t *hal)
{
	mxt_info_t *ibinf = &ib_id_information;;
	mxt_object_t *ibots = ib_objects_tables;
	data_crc24_t *ibcrc = &ib_info_crc;
	mxt_message_fifo_t *msg_fifo = &message_fifo;

	const crc_data_blocks_t dblock[] = { {(u8 *)ibinf, sizeof(*ibinf)}, {(u8 *)ibots, sizeof(*ibots)} };
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
		ibots->start_address = offset;
		offset += (ibots->size_minus_one + 1) * (ibots->instances_minus_one + 1);
	}
	
	// calculate Information Block CRC
	crc = calc_blocks_crc24(dblock, ARRAY_SIZE(dblock));
	ibcrc->data[0] = crc & 0xff;
	ibcrc->data[1] = (crc >> 8) & 0xff;
	ibcrc->data[2] = (crc >> 16) & 0xff;
	
	// Set all message invalid
	memset(msg_fifo->buffer, MXT_RPTID_NOMSG, sizeof(msg_fifo->buffer));	
	mutex_init(&msg_fifo->lock);
	
	// Initialize each object
	for (i = 0; i < MXT_OBJECTS_INITIALIZE_LIST_NUM; i++) {
		if (ocb[i].init) {
			reportid = get_report_id(ibots, ocb[i].type);
			ocb[i].init(reportid);
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
			ocb[i].report();
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

u8 get_report_id(mxt_object_t *ibots, u8 regid)
{
	mxt_object_t *obj;
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

u8 message_count(const mxt_message_fifo_t *msg_fifo)
{
	u16 rpos, wpos;
	
	//Make FIFO flat
	rpos = msg_fifo->rpos;
	wpos = msg_fifo->wpos;
	if (wpos < rpos) {
		wpos += MXT_REPORT_ID_COUNT;
	}
	
	return (u8)(wpos - rpos);
}

int mpt_get_message_count(void)
{
	mxt_message_fifo_t *msg_fifo = &message_fifo;
	int count;
	
	lock(&msg_fifo->lock);
	
	count = message_count(msg_fifo);
	
	unlock(&msg_fifo->lock);
	
	return count;
}

int mpt_read_message(object_t5_t *msg)
{
	mxt_objects_reg_t * ibreg = &ib_objects_reg;
	mxt_message_fifo_t *msg_fifo = &message_fifo;
	message_buffer_t *msg_buf;
	
	u16 rpos, curr;
	int i;
	
	lock(&msg_fifo->lock);
	
	//Get first message from FIFO
	rpos = msg_fifo->rpos % MXT_REPORT_ID_COUNT;
	msg_buf = &msg_fifo->buffer[rpos];
	for (i = msg_buf->curr + 1; i < msg_buf->curr + MXT_MESSAGE_DEPTH_EACH_RID; i++) {
		curr = i % MXT_MESSAGE_DEPTH_EACH_RID;
		if (msg_buf->data[curr].reportid && msg_buf->data[curr].reportid != MXT_RPTID_NOMSG)
		break;
	}
	
	curr = i % MXT_MESSAGE_DEPTH_EACH_RID;
	memcpy(msg, &msg_buf->data[curr], sizeof(*msg));
	msg_buf->data[curr].reportid = MXT_RPTID_NOMSG;
	
	//Last valid message in this buffer
	if (curr == msg_buf->curr) {
		if (msg_buf->data[curr].reportid != MXT_RPTID_NOMSG) {
			msg_fifo->rpos = (msg_fifo->rpos + 1) % MXT_REPORT_ID_COUNT;
		}
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
	message_buffer_t *msg_buf = NULL;	
	int i, result = 0;
	u16 rpos, wpos, pos;
	
	lock(&msg_fifo->lock);
	
	//Make FIFO flat
	rpos = msg_fifo->rpos;
	wpos = msg_fifo->wpos;
	if (wpos < rpos) {
		wpos += MXT_REPORT_ID_COUNT;
	}
	
	//Search current message FIFO
	for (i = rpos; i < wpos; i++) {
		pos = i % MXT_REPORT_ID_COUNT;
		msg_buf = &msg_fifo->buffer[pos];
		if (msg_buf[msg_buf->curr].data->reportid == msg->reportid)
			break;
	}
	
	//Not exist, get a new buffer
	if (i == wpos) {	//Not found in Fifo, alloc new buffer
		pos = i % MXT_REPORT_ID_COUNT;
		if (pos < rpos && pos + 1 == rpos) {	//Fifo is full, something must be wrong
			msg_buf = NULL;
		}else {	/* Not full, alloc the new buffer */
			msg_buf = &msg_fifo->buffer[pos];
		}
	}
	
	//Copy to new buffer
	if (msg_buf) {
		msg_buf->curr = (msg_buf->curr + 1) % MXT_MESSAGE_DEPTH_EACH_RID;
		memcpy(msg_buf[msg_buf->curr].data, msg, sizeof(*msg));
		
		if (wpos - rpos < MXT_REPORT_ID_COUNT)
			msg_fifo->wpos = (pos + 1) % MXT_REPORT_ID_COUNT;
	}else {
		result = -3;
	}
	
	//Update T44
	ibreg->ram.t44.count = message_count(msg_fifo);
	
	unlock(&msg_fifo->lock);
	
	return result;
}

/*
enum {
	CMD_READ_ID_INFORMATION = 0,
	CMD_READ_INFORMATION_TABLE,
	CMD_READ_INFO_CRC,
	CMD_OBJETS,
};

int address_to_command(u16 addr)
{
	if (addr < MXT_OBJECT_TABLE_START) {
		return CMD_READ_ID_INFORMATION;
	}else if (addr < MXT_INFO_CRC_START) {
		return CMD_READ_INFORMATION_TABLE;
	}else if (addr < MXT_OBJECT_START) {
		return CMD_READ_INFO_CRC;
	}else {
		return CMD_OBJETS;
	}
}
*/

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
	mxt_object_t *ibots = ib_objects_tables;
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
		if (regaddr == obj_t5->start_address) {
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
	mxt_object_t *ibots = ib_objects_tables;
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
	const mxt_object_t *ibots = ib_objects_tables;
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