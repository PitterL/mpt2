/*
 * tsl.h
 *
 * Created: 6/14/2019 10:15:12 PM
 *  Author: A41450
 */ 

#ifndef TSL_H_
#define TSL_H_

#include "tslapi.h"

typedef struct hal_interface_info {
	// HW interface
	void (*fn_set_chg)(u8 assert, bool retrigger);

	void (*fn_reset)(void);
	
	ssint (*fn_load_cfg)(u8 *cfg, size_t size);
	ssint (*fn_save_cfg)(const u8 *cfg, size_t size);

} hal_interface_info_t;

typedef struct tsl_interface_info {
	// HW interface
	hal_interface_info_t hal;

	// Touch lib interface
	u8 matrix_xsize;
	u8 matrix_ysize;
	u8 xsize;
	u8 ysize;
	u8 measallow;
	
	void (*fn_calibrate)(void);
	cb_writeback_t fn_writeback; // void (*fn_writeback)(u8 type, void *src, size_t size, u8 index);
	
}tsl_interface_info_t;

void tsl_init(const hal_interface_info_t *hal);
void tsl_start(void);
void tsl_process(void);

/* Will lock FIFO in write operation */
ssint tsl_write_message(const void *msg);

/* Will lock FIFO in read operation */
ssint tsl_read_message(void *msg);

ssint tsl_mem_read(u16 baseaddr, u16 offset, u8 *out_ptr);
ssint tsl_mem_write(u16 regaddr, u8 val);

#endif /* TSL_H_ */