/*
 * tsl.h
 *
 * Created: 6/14/2019 10:15:12 PM
 *  Author: A41450
 */ 

#ifndef TSL_H_
#define TSL_H_

#include "include/types.h"
#include "arch/tslapi.h"

// HW interface
typedef void (*cb_hal_assert_irq_t)(u8 assert, bool retrigger);
typedef void (*cb_hal_reset_t)(void);
typedef ssint (*cb_hal_load_cfg_t)(u8 *cfg, size_t size);
typedef ssint (*cb_hal_save_cfg_t)(const u8 *cfg, size_t size);

typedef struct hal_interface_info {
    const cb_hal_assert_irq_t assert_irq;
    const cb_hal_reset_t reset;
	const cb_hal_load_cfg_t load_cfg;
    const cb_hal_save_cfg_t save_cfg;
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
ssint tsl_start(void);
void tsl_pre_process(void);
void tsl_process(void);
void tsl_post_process(void);
u8 tsl_assert_irq(void);

ssint tsl_mem_read(u16 baseaddr, u16 offset, u8 *out_ptr);
ssint tsl_mem_write(u16 baseaddr, u16 offset, u8 val);
void tsl_end(void);

#endif /* TSL_H_ */