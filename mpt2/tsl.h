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
	void (*fn_assert_irq)(u8 assert, bool retrigger);

	void (*fn_reset)(void);
	
	ssint (*fn_load_cfg)(u8 *cfg, size_t size);
	ssint (*fn_save_cfg)(const u8 *cfg, size_t size);

} hal_interface_info_t;

typedef struct tsl_interface_info {
	// HW interface
	const hal_interface_info_t *hal;

	// QTouch lib interface
	const qtouch_api_callback_t *api;
	
	// Touch default config
	const qtouch_config_t *qtdef;
}tsl_interface_info_t;

void tsl_init(const hal_interface_info_t *hal);
void tsl_start(void);
void tsl_pre_process(void);
void tsl_process(void);
void tsl_post_process(void);

ssint tsl_mem_read(u16 baseaddr, u16 offset, u8 *out_ptr);
ssint tsl_mem_write(u16 baseaddr, u16 offset, u8 val);
void tsl_end(void);

#endif /* TSL_H_ */