/*
 * mptt.h
 *
 * Created: 6/7/2019 9:48:20 PM
 *  Author: A41450
 */ 

#ifndef MPTT_H_
#define MPTT_H_

ssint mpt_api_chip_init(const void *tsl_ptr);
void mpt_api_chip_start(void);
ssint mpt_api_chip_test(void);
ssint mpt_api_mem_read(u16 baseaddr, u16 offset, u8 *out_ptr);
ssint mpt_api_mem_write(u16 baseaddr, u16 offset, u8 val);
void mpt_api_touch_init(void);
void mpt_api_pre_process(void);
void mpt_api_touch_process(void);
void mpt_api_touch_done(void);
ssint mpt_api_set_sensor_data(u8 sensor, /*const cap_sample_value_t * const*/ const void * cv, bool busy);
void mpt_api_set_button_status(u8 id, u8 status);
void mpt_api_set_pointer_location(u8 type, u8 id, u8 status, u16 x, u16 y);
void mpt_api_set_chip_status(u8 mask, u8 set);
void mpt_api_report_status(void);
u8 mpt_api_get_selftest_op(void);

/* Will lock FIFO get count operation */
u8 mpt_api_request_irq(void);

//void mpt_api_handle_command(void);
void mpt_api_writeback(void);

ssint mpt_api_config_op(u8 type, void *buf, u8 size, u8 index, u8 rw);
u8 mpt_api_read_config_byte(u8 type);
void mpt_api_calibrate(void);
bool mpt_api_touch_state_idle(void);
// void mpt_api_touch_suspend(bool suspend);
bool mpt_api_touch_state_sleep(void);
u8	mpt_api_touch_sleep(void);
void mpt_api_touch_inject_event(void);

u8 mpt_api_get_number_key_sensors(void);
ssint mpt_api_read_button_state(u8 index);
ssint mpt_api_read_ref_signal_cap(u8 key, /*cap_sample_value_t **/void *cv);
u8 mpt_api_get_chip_state(void);

#ifdef TOUCH_API_SCROLLER
u8 mpt_api_get_number_slider_sensors(void);
ssint mpt_api_read_slider_state(u8 index, /*t9_point_status_t */void *sts);
#endif

#ifdef TOUCH_API_SURFACE
ssint mpt_api_read_surface_state(u8 id, /*t9_point_status_t */void *sts);
#endif

#endif /* MPTT_H_ */