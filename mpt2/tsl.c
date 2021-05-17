/*
 * trans.c
 *
 * Created: 6/14/2019 10:14:51 PM
 *  Author: A41450
 */ 

#include <string.h>
#include "include/types.h"
#include "arch/cpu.h"
#include "arch/tslapi.h"
#include "arch/timer.h"
#include "arch/bus.h"
#include "objects/txx.h"
#include "mptt.h"
#include "interface.h"
#include "tsl.h"

#ifdef TOUCH_API_BUTTON
qbutton_config_t buttons_config[MXT_TOUCH_KEYARRAY_T15_INST] = {
#ifdef EVK_QT1
	// Note QT1 need modify circle to support QT1
	{ .node = {	.origin = 0, .size = 2 } },	// Button
	{ .node = {	.origin = 2, .size = 4 } },	// Surface slider
#endif
#ifdef EVK_QT7	
	{ .node = {	.origin = 0, .size = 2 } },	// Button
	{ .node = {	.origin = 2, .size = 3 } },	// Surface slider
#endif
#ifdef EVK_QT8	
	{ .node = {	.origin = 0, .size = 5 } },	// Button
	{ .node = {	.origin = 5, .size = 5 } },	
#endif
#ifdef EVK_WATER_SURFACE
	{ .node = {	.origin = 0, .size = 11 } },	// Surface slider
	{ .node = {	.origin = 11, .size = 2 } },	// Button
#endif
#ifdef EVK_3217_XPLAIN
    { .node = {	.origin = 0, .size = 2 } },
#endif
};
#endif

#if defined(OBJECT_T9)
// For simpling the algorithm, we set v for x, h for y, but must care, v should start first, h follow up
qsurface_config_t surfaces_sliders_config[MXT_TOUCH_MULTI_T9_INST] = {
	{
		/*.resolution_bit = 8,	*/
		/*.resolution_max = (1 << 8) -1,	*/
	}
};
#endif

qtouch_config_t tsl_qtouch_def = {
#ifdef EVK_QT1
	.matrix_nodes = {{.origin = 0, .size = 2}, {.origin =  2, .size = 4}},
#endif
#ifdef EVK_QT7
	.matrix_nodes = {{.origin = 0, .size = 2}, {.origin =  2, .size = 3}},
#endif
#ifdef EVK_WATER_SURFACE
	.matrix_nodes = {{.origin = 0, .size = 5}, {.origin =  5, .size = 8}},
#endif
#ifdef EVK_3217_XPLAIN
	.matrix_nodes = {{.origin = 0, .size = 2}, {.origin =  2, .size = 2}},
#endif
#ifdef TOUCH_API_BUTTON
	//If define num_button, should filled the buttons_config
	.buttons = &buttons_config[0],
	.num_button = ARRAY_SIZE(buttons_config),
	/*.num_buttons_channel_count,*/
#endif

#if defined(OBJECT_T9)
	//If define num_surfaces_slider, should filled the surfaces_sliders_config
	.surface_sliders = &surfaces_sliders_config[0],
	.num_surfaces_slider = ARRAY_SIZE(surfaces_sliders_config),
	/*.num_surfaces,*/
	/*.num_slider,*/
	/*.num_surfaces_slider_channel_count,*/
#endif

	.params = {
		.max_prsc_div = 0xff,
		.max_gain = GAIN_16,
		.max_filter_count = FILTER_LEVEL_64,
		.max_resl = RSEL_VAL_200,
	},
};

qtouch_api_callback_t tsl_api_info =
{
	.sync = tsapi_config_op,
#ifdef OBJECT_T6
	.calibrate = tsapi_calibrate,
#endif
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
	u8 sensor_count = tsapi_read_config_byte(API_NUM_CHANNELS);

	if (!(qdef->matrix_nodes[NODE_X].size || qdef->matrix_nodes[NODE_Y].size)) {
		qdef->matrix_nodes[NODE_X].origin = 0;
		qdef->matrix_nodes[NODE_X].size = (sensor_count + 1) >> 1;
		qdef->matrix_nodes[NODE_Y].origin = qdef->matrix_nodes[NODE_X].size;
		qdef->matrix_nodes[NODE_Y].size = sensor_count - qdef->matrix_nodes[NODE_X].size;
	}
	
	qdef->maxtrix_channel_count = (u8)sensor_count;
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
 * @Return: Zero mean successful, other value mean something fault
 */
ssint tsl_start(void)
{
	ssint result;

	result = mpt_api_chip_start();
	if (result)
		return result;    

    touch_start();

	return 0;
}

/**
 * \brief touch software layer pre-work before touch process, 
 */
void tsl_pre_process(void)
{
	mpt_api_pre_process();
}

void tch_ref_signal_update(void)
{
#if (defined(OBJECT_T37) || defined(OBJECT_T25) || defined(OBJECT_T109))
	u8 sensor_count = tsapi_read_config_byte(API_NUM_CHANNELS);
	u8 i;
	ssint result;
	cap_sample_value_t cvalue;
	
	for (i = 0; i < sensor_count; i++) {
		result = tsapi_read_ref_signal_cap(i, &cvalue);
		if (result == 0)
			mpt_api_set_sensor_data(i, cvalue.reference, cvalue.signal, cvalue.cccap, cvalue.comcap);
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
		mpt_api_set_chip_status(MXT_T6_STATUS_RESET|MXT_T6_STATUS_CAL|MXT_T6_STATUS_SIGERR, 0);
	}
#endif
}

/**
 * \brief touch software layer work when each touch process, 
   there will handle the chip status and assert IRQ on data interface
 */
void tsl_process(void)
{
	tch_update_chip_state();
}

#ifdef TOUCH_API_BUTTON
void tch_button_press_report(void)
{
	u8 button_count = tsapi_read_config_byte(API_NUM_SENSORS);
	u8 i;
	ssint result;
	
	for (i = 0; i < button_count; i++) {	
		result = tsapi_read_button_state(i);
		if (result >= 0)
			mpt_api_set_button_status(i, (u8)result);
	}
}
#endif

#ifdef TOUCH_API_SCROLLER
void tch_slider_location_report(const qtouch_config_t *qdef)
{
	u8 slider_count = tsapi_read_config_byte(API_NUM_SLIDERS);
	t9_point_status_t point;
	u8 i;
	ssint result;
	
	for ( i = 0; i < slider_count; i++ ) {
		#define SLIDER_FINGER_ID 0
		result = tsapi_read_slider_state(i, &point);
		if (result == 0)
			mpt_api_set_pointer_location(i, SLIDER_FINGER_ID, point.status,  point.pos.x, point.pos.y);
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
		if (result == 0)	
			mpt_api_set_pointer_location(SURFACE_INST_ID, i, point.status,  point.pos.x, point.pos.y);
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

#ifdef TOUCH_API_SCROLLER
	tch_slider_location_report(qdef);
#endif
#ifdef TOUCH_API_SURFACE
	tch_surface_location_report();
#endif
#ifdef TOUCH_API_BUTTON
	tch_button_press_report();
#endif

	if (mptt_get_bus_state() != BUS_READ)	//the ref/signal is 16bits, data may crashed when bus is reading and update.
		tch_ref_signal_update();
}

void tsl_suspend(uint8_t suspend)
{
#ifdef OBJECT_T25
	touch_suspend(suspend);
#endif
}

ssint tsl_mem_read(u16 baseaddr, u16 offset, u8 *out_ptr)
{
	return mpt_api_mem_read(baseaddr, offset, out_ptr);
}

ssint tsl_mem_write(u16 baseaddr, u16 offset, u8 val)
{
	return mpt_api_mem_write(baseaddr, offset, val);
}

void tsl_end(void)
{
	mpt_api_writeback();
	
	tsl_assert_irq();
}