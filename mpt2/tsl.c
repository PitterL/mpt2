/*
 * trans.c
 *
 * Created: 6/14/2019 10:14:51 PM
 *  Author: A41450
 */ 

#include <string.h>
#include "include/types.h"
#include "arch/cpu.h"
//#include "arch/tslapi.h"
#include "arch/timer.h"
#include "arch/bus.h"
#include "objects/txx.h"
#include "mptt.h"
#include "interface.h"
#include "tsl.h"
#include "board.h"

qtouch_api_callback_t tsl_api_info =
{
	.sync = tsapi_config_op,
#ifdef OBJECT_T6
	.calibrate = tsapi_calibrate,
#endif
	.idle = tsapi_touch_state_idle,
	.suspend = tsapi_touch_suspend
};

tsl_interface_info_t interface_tsl = 
{
	.api = &tsl_api_info,
	.qtdef = &tsl_qtouch_def,
};

u8 tsl_assert_irq(void)
{
	return mpt_api_request_irq();
}

void init_maxtrix_node(qtouch_config_t *qdef)
{
	u8 sensor_channels = tsapi_read_config_byte(API_NUM_CHANNELS);

	if (!(qdef->matrix_nodes[NODE_X].size || qdef->matrix_nodes[NODE_Y].size)) {
		qdef->matrix_nodes[NODE_X].origin = 0;
		qdef->matrix_nodes[NODE_X].size = 4;	//4x mode
		qdef->matrix_nodes[NODE_Y].origin = 0;
		qdef->matrix_nodes[NODE_Y].size = sensor_channels >> 2;
	}
	
	qdef->maxtrix_channel_count = (u8)sensor_channels;
}

#ifdef TOUCH_API_BUTTON
void init_button_nodes(qtouch_config_t *qdef)
{
	qbutton_config_t *btn = &qdef->buttons[0];
	u8 i;
	
	for (i = 0; i < qdef->num_button; i++) {
		if (!btn[i].node.size) {
			if (i == 0) {	//All nodes assigned to button 0
				btn[i].node.origin = 0;
				btn[i].node.size = tsapi_read_config_byte(API_NUM_SENSORS);
			} else {
				break;
			}
		}
		qdef->num_button_channel_count += btn[i].node.size;
	}
	
	qdef->num_button = i;
}
#endif

#ifdef TOUCH_API_SCROLLER
void init_slider_nodes(qtouch_config_t *qdef)
{
	qsurface_config_t *sursld = &qdef->surface_sliders[0];
	u8 num_scrollers = tsapi_read_config_byte(API_NUM_SLIDERS);
	u8 val, i;
			
	for ( i = 0; i < num_scrollers && i < qdef->num_surfaces_slider; i++) {
		if (!(sursld[i].nodes[NODE_X].size || sursld[i].nodes[NODE_Y].size)) {
			sursld[i].nodes[NODE_X].origin = 0;
			sursld[i].nodes[NODE_X].size = 0;
			sursld[i].nodes[NODE_Y].origin = tsapi_read_group_config_byte(API_SLIDER_START_KEY, i);
			sursld[i].nodes[NODE_Y].size = tsapi_read_group_config_byte(API_SLIDER_NUM_KEYS, i);

            val = tsapi_read_group_config_byte(API_SLIDER_RESOL_DB, i);
			sursld[i].resolution_bit = ((SCR_RESOLUTION(val))/* - RESOL_2_BIT*/);
			sursld[i].resolution_max = (1 << sursld[i].resolution_bit) - 1;
		
			//position_hysteresis
			//contact_min_threshold
		}
		qdef->num_surfaces_slider_channel_count += sursld[i].nodes[NODE_Y].size;
	}
	
	qdef->num_slider = i;
}
#endif

#ifdef TOUCH_API_SURFACE
void init_surface_node(qtouch_config_t *qdef)
{
	qsurface_config_t *sursld = &qdef->surface_sliders[qdef->num_slider];
    u8 val;

	if (!(sursld->nodes[NODE_X].size && sursld->nodes[NODE_Y].size)) {
		// For simpling the algorithm, we set v for x, h for y, but must care, v should start first, h follow up
		// Only one surface in atmel start code, so there without loop
		sursld->nodes[NODE_X].origin = tsapi_read_config_byte(API_SURFACE_CS_START_KEY_V);	//should start first
		sursld->nodes[NODE_X].size = tsapi_read_config_byte(API_SURFACE_CS_NUM_KEYS_V);
		sursld->nodes[NODE_Y].origin = tsapi_read_config_byte(API_SURFACE_CS_START_KEY_H);
		sursld->nodes[NODE_Y].size = tsapi_read_config_byte(API_SURFACE_CS_NUM_KEYS_H);
	}
	
	// Resolution
	val = tsapi_read_config_byte(API_SURFACE_CS_RESOL_DB);
	sursld->resolution_bit = ((SCR_RESOLUTION(val))/* - RESOL_2_BIT*/);
	sursld->resolution_max = (1 << sursld->resolution_bit) - 1;
	// Deadband percentage
	// sursld->deadband = qtm_surface_cs_config1.resol_deadband & 0xf;
	
	qdef->num_surfaces_slider_channel_count += sursld->nodes[NODE_X].size + sursld->nodes[NODE_Y].size;
	qdef->num_surfaces = 1;
}
#endif

void inlitialize_button_slider_surface_nodes(qtouch_config_t *qdef)
{
	init_maxtrix_node(qdef);

#ifdef TOUCH_API_BUTTON
	init_button_nodes(qdef);
#endif
	
#ifdef TOUCH_API_SCROLLER
	init_slider_nodes(qdef);
#endif

#ifdef TOUCH_API_SURFACE
	init_surface_node(qdef);
#endif
}

/*
 * \brief Touch software layer initialization
 * @hal: point to HW operation structure from interface.c
*/

void tsl_init(const hal_interface_info_t *hal)
{
	tsl_interface_info_t *tsl = &interface_tsl;
	qtouch_config_t *qdef = (qtouch_config_t *)tsl->qtdef;
	
	// Save HAL interface
	tsl->hal = hal;
	
	inlitialize_button_slider_surface_nodes(qdef);
	
	mpt_api_chip_init(tsl);
}

/**
 * \notice touch software layer active, 
	include register start, function start
 * @start: call chip start if set
 * @Return: Zero mean successful, other value mean something fault
 */
ssint tsl_start(ssint start)
{
	ssint result;

	if (start) {
		mpt_api_chip_start();
	}
	
	result = mpt_api_chip_test();
	if (result) {
		return result;
	}

    touch_start();

	return 0;
}

/**
 * \brief touch software layer pre-work before touch process, 
 */
void tsl_pre_process(void)
{
#ifndef OBJECT_T37_DEBUG_LOWPOWER_INFO
	if (tsapi_touch_state_sleep()) {
		return;
}
#endif
	
	mpt_api_pre_process();
}

void tch_ref_signal_update(void)
{
#if (defined(OBJECT_T37) || defined(OBJECT_T25) || defined(OBJECT_T109))
	u8 sensor_count = tsapi_get_number_key_sensors();
	u8 i;
	ssint result;
	cap_sample_value_t cvalue;
		
	for (i = 0; i < sensor_count; i++) {
		// Fetch `rsd` data
		result = tsapi_read_ref_signal_cap(i, &cvalue);
		if (result == 0) {
			// Dispatch `rsd` data
			mpt_api_set_sensor_data(i, &cvalue);
		}
	}
#endif
}

void tch_update_chip_state(void)
{
#ifdef OBJECT_T6
	u8 state = tsapi_get_chip_state();
	if (state) {
		mpt_api_set_chip_status(state, 1);
	} else {
		mpt_api_set_chip_status(MXT_T6_STATUS_RESET|MXT_T6_STATUS_CAL, 0);
	}
#endif
}

/**
 * \brief touch software layer work when each touch process, 
   there will handle the chip status and assert IRQ on data interface
 */
void tsl_process(void)
{
#ifndef OBJECT_T37_DEBUG_LOWPOWER_INFO
	if (tsapi_touch_state_sleep()) {
		return;
	}
#endif

	tch_update_chip_state();
}

#ifdef TOUCH_API_BUTTON
void tch_button_press_report(void)
{
	u8 button_count = tsapi_get_number_key_sensors();
	u8 i;
	ssint result;
	
	for (i = 0; i < button_count; i++) {
		result = tsapi_read_button_state(i);
		if (result >= 0) {
			mpt_api_set_button_status(i, (u8)result);
		}
	}
}
#endif

#ifdef TOUCH_API_SCROLLER
void tch_slider_location_report(const qtouch_config_t *qdef)
{
	u8 slider_count = tsapi_get_number_slider_sensors();
	t9_point_status_t point;
	u8 i;
	ssint result;
	
	for ( i = 0; i < slider_count; i++ ) {
#define SLIDER_FINGER_ID 0
		result = tsapi_read_slider_state(i, &point);
		if (result == 0) {
			mpt_api_set_pointer_location(i, SLIDER_FINGER_ID, point.status,  point.pos.x, point.pos.y);
		}
	}
}
#endif

#ifdef TOUCH_API_SURFACE
void tch_surface_location_report(void)
{
	t9_point_status_t point;
	u8 i;
	ssint result;

#define SURFACE_INST_ID 0
#define SURFACE_FINGERS 1

	for (i = 0; i < SURFACE_FINGERS; i++) {
		result = tsapi_read_surface_state(i, &point);
		if (result == 0) {
			mpt_api_set_pointer_location(SURFACE_INST_ID, i, point.status,  point.pos.x, point.pos.y);
		}
	}
}
#endif

/**
 * \brief touch software layer post-work when touch process measure done, 
   there will handle the button/slider/surface pointer status
 */
void tsl_post_process(void)
{	
#ifdef TOUCH_API_SCROLLER
	tsl_interface_info_t *tsl = &interface_tsl;
	qtouch_config_t *qdef = (qtouch_config_t *)tsl->qtdef;
#endif

#ifndef OBJECT_T37_DEBUG_LOWPOWER_INFO
	if (tsapi_touch_state_sleep()) {
		return;
	}
#endif

#ifdef TOUCH_API_SCROLLER
	tch_slider_location_report(qdef);
#endif
#ifdef TOUCH_API_SURFACE
	tch_surface_location_report();
#endif
#ifdef TOUCH_API_BUTTON
	tch_button_press_report();
#endif

	if (mptt_get_bus_state() != BUS_READ) {	//the ref/signal is 16bits, data may crashed when bus is reading and update.
		tch_ref_signal_update();
	}
}

/*============================================================================
USE_MPTT_WRAPPER
bool tsl_inbusy(void)
------------------------------------------------------------------------------
Purpose: Indicate whether the touch is busy now, outside could decide whether enter into sleep mode
Input  : none
Output : Zero: idle, None: Busy
Notes  :
============================================================================*/
ssint tsl_sleep()
{	
	/* check chip status */
	if (tsapi_get_chip_state() == 0) {
		if (mpt_api_get_selftest_op() == 0) {
			/* execute touch processor sleep */
			if (tsapi_touch_sleep() == 0) {
				return 0;
			}
		}
	}
	
	return -2;
}

ssint tsl_mem_read(u16 baseaddr, u16 offset, u8 *out_ptr)
{
	return mpt_api_mem_read(baseaddr, offset, out_ptr);
}

ssint tsl_mem_write(u16 baseaddr, u16 offset, u8 val)
{
	return mpt_api_mem_write(baseaddr, offset, val);
}

void tsl_end(ssint write)
{
	if (write) {
		/* write back data to library */
		mpt_api_writeback();
		
		/* Note a external event to touch process handler*/
		tsapi_touch_inject_event();
	}

	/* Note irq state */
	tsl_assert_irq();
}