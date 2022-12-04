/*
 * tslink.h
 *
 * Created: 6/15/2019 2:37:52 PM
 *  Author: A41450
 */ 


#ifndef TSLINK_H_
#define TSLINK_H_


#include <qtouch/lib/tslib.h>

/*----------------------------------------------------------------------------
 *     Macro Declarations
 *----------------------------------------------------------------------------*/

#ifdef OBJECT_T15
#ifdef TOUCH_API_KEYS_H
#define TOUCH_API_BUTTON
#endif
#endif

#ifdef OBJECT_T9
#ifdef TOUCH_API_SCROLLER_H
#define TOUCH_API_SCROLLER
#endif
#if (defined(TOUCH_API_SURFACE_CS2T_H) || defined(TOUCH_API_SURFACE_CS_H))
#define TOUCH_API_SURFACE
#endif
#endif

#ifdef OBJECT_T126
#define TOUCH_API_AUTOSCAN
#endif

#ifndef POSITION_CHANGE
#ifdef SCROLLER_POSITION_CHANGE
#define POSITION_CHANGE SCROLLER_POSITION_CHANGE
#else
#define POSITION_CHANGE SURFACE_POSITION_CHANGE
#endif
#endif

#ifndef TOUCH_ACTIVE
#ifdef SCROLLER_POSITION_CHANGE
#define TOUCH_ACTIVE SCROLLER_POSITION_CHANGE
#else
#define TOUCH_ACTIVE SURFACE_POSITION_CHANGE
#endif
#endif

#ifndef SCR_RESOLUTION
#ifdef SCROLLER_RESOLUTION
#define SCR_RESOLUTION(m) SCROLLER_RESOLUTION(m)
#else
#define SCR_RESOLUTION(m) SURFACE_RESOLUTION(m)
#endif
#endif

#ifndef SCR_DEADBAND
#ifdef SCROLLER_DEADBAND
#define SCR_DEADBAND(m) SCROLLER_DEADBAND(m)
#else
#define SCR_DEADBAND(m) SURFACE_DEADBAND(m)
#endif
#endif

ssint tslink_config_op(u8 type, void *buf, u8 size, u8 index, u8 rw);
u8 tslink_read_group_config_byte(u8 type, u8 index);
u8 tslink_read_config_byte(u8 type);
void tslink_calibrate(void);
u8 tslink_get_chip_state(void);
u8 tslink_get_number_channel_nodes(void);
u8 tslink_get_number_key_sensors(void);
u8 tslink_get_number_slider_sensors(void);
bool tslink_sensor_state_is_suspend(uint8_t node);
ssint tslink_read_ref_signal_cap(u8 index, /* cap_sample_value_t **/ void *cval);
ssint tslink_read_button_state(u8 index);
ssint tslink_read_slider_state(u8 index, /*t9_point_status_t */ void *sts);
ssint tslink_read_surface_state(u8 id,/*t9_point_status_t */void *sts);
u8 tslink_t8_sensing_mode_translate(u8 mode, u8 rw);

u16 tslink_t6_get_sensor_base_ref(void);
bool tslink_touch_adc_init(void);
bool tslink_touch_adc_deinit(void);
void tslink_touch_suspend(bool suspend);
void tslink_touch_inject_event(void);
bool tslink_touch_state_idle(void);
bool tslink_touch_state_sleep(void);
u8	tslink_touch_sleep(void);
void tslink_touch_init(void);
void tslink_touch_process(void);
void tslink_touch_done(void);

#ifdef MPTT_FUSE_CHECK
ssint tslink_fuse_check(void);
const u8 *tslink_get_fuse_data(u8 *len_ptr);
#endif

#ifdef OBJECT_T37_DEBUG_PLATFORM_INFO
const u8 *tslink_get_signature_row_data(u8 *len_ptr);
#endif

uint8_t tslink_get_sensor_key_mapping(uint8_t sensor_node, uint8_t group);
uint8_t tslink_get_channel_node_mapping(uint8_t channel_node);

#endif /* TSLINK_H_ */