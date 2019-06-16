/*
 * mptt.h
 *
 * Created: 6/7/2019 9:48:20 PM
 *  Author: A41450
 */ 

#ifndef MPTT_H_
#define MPTT_H_

#include "types.h"
#include "crc.h"

#define MPTT_FW_FAMILY_ID 0xa6	//0x81
#define MPTT_FW_VARIANT_ID 0x08	//0x01
#define MPTT_FW_VERSION 0x10
#define MPTT_FW_BUILD 0xBB

#include "objects/txx.h"

ssint mpt_chip_init(const void *tsl_ptr);

void mpt_chip_start(void);

/* Will lock FIFO in write operation */
ssint mpt_write_message(const /*object_t5_t*/void *msg_ptr);

/* Will lock FIFO in read operation */
ssint mpt_read_message(object_t5_t *msg);

ssint mpt_mem_read(u16 baseaddr, u16 offset, u8 *out_ptr);
ssint mpt_mem_write(u16 regaddr, u8 val);

ssint mpt_object_read(u8 regid, u8 instance, u16 offset, u8 *out_ptr, u8 size);
ssint mpt_object_write(u8 regid, u8 instance, u16 offset, const u8 *ptr, u8 size);

void mpt_api_process(void);
void mpt_api_set_sensor_data(u8 channel, u8 state, u16 reference, u16 signal, u16 cap);
void mpt_api_set_pointer_location(u8 id, u8 status, u16 x, u16 y);

typedef void (* message_cb_t)(u8/* message_count*/ , bool /*retrigger*/);
/* Will lock FIFO get count operation */
void mpt_api_request_irq(message_cb_t cb);

#endif /* MPTT_H_ */