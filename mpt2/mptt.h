/*
 * mptt.h
 *
 * Created: 6/7/2019 9:48:20 PM
 *  Author: A41450
 */ 

#ifndef MPTT_H_
#define MPTT_H_

ssint mpt_api_chip_init(const void *tsl_ptr);
ssint mpt_api_chip_start(void);
ssint mpt_api_mem_read(u16 baseaddr, u16 offset, u8 *out_ptr);
ssint mpt_api_mem_write(u16 baseaddr, u16 offset, u8 val);
void mpt_api_pre_process(void);
void mpt_api_process(void);
void mpt_api_set_sensor_data(u8 channel, u16 reference, u16 signal, u16 cap, u16 comcap);
void mpt_api_set_button_status(u8 id, u8 status);
void mpt_api_set_pointer_location(u8 type, u8 id, u8 status, u16 x, u16 y);
void mpt_api_set_chip_status(u8 mask, u8 set);
void mpt_api_report_status(void);

/* Will lock FIFO get count operation */
void mpt_api_request_irq(void);

//void mpt_api_handle_command(void);
void mpt_api_writeback(void);

#endif /* MPTT_H_ */