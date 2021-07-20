/*
 * board.c
 *
 * Created: 7/12/2021 9:26:07 AM
 *  Author: A41450
 */ 

#include "include/types.h"
#include "arch/cpu.h"
#include "objects/txx.h"
#include "board.h"
#include "led.h"

#ifdef TOUCH_API_BUTTON
qbutton_config_t buttons_config[MXT_TOUCH_KEYARRAY_T15_INST] = {
	#ifdef EVK_QT1
	// Note QT1 need modify circle to support QT1
	{ .node = {	.origin = 0, .size = 2 } },	// Button
	{ .node = {	.origin = 2, .size = 4 } },	// Surface slider
	#endif
	#ifdef EVK_3217_QT7
	{ .node = {	.origin = 0, .size = 2 },
#ifdef OBJECT_T15_USE_STATE_CB
		.set_button_state = button_led_state_change
#endif
	},	// Button
	{ .node = {	.origin = 2, .size = 3 } },	// Surface slider
	#endif
	#ifdef EVK_3217_QT8
	{ .node = {	.origin = 0, .size = 5 } },	// Button
	{ .node = {	.origin = 5, .size = 5 } },
	#endif
	#ifdef EVK_WATER_SURFACE
	{ .node = {	.origin = 0, .size = 11 } },	// Surface slider
	{ .node = {	.origin = 11, .size = 2 } },	// Button
	#endif
#ifdef PROJECT_BX11
    { .node = {	.origin = 0, .size = 3 } },
	{ .node = {	.origin = 3, .size = 1 } },	
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
#ifdef OBJECT_T9_USE_STATE_CB
		.set_touch_state = slider_led_state_change
#endif
	}
};
#endif

qtouch_config_t tsl_qtouch_def = {
	#ifdef EVK_QT1
	.matrix_nodes = {{.origin = 0, .size = 2}, {.origin =  2, .size = 4}},
	#endif
	#ifdef EVK_3217_QT7
	.matrix_nodes = {{.origin = 0, .size = 2}, {.origin =  2, .size = 3}},
	#endif
	#ifdef EVK_3217_QT8
	.matrix_nodes = {{.origin = 0, .size = 2}, {.origin =  2, .size = 3}},
	#endif
	#ifdef EVK_WATER_SURFACE
	.matrix_nodes = {{.origin = 0, .size = 5}, {.origin =  5, .size = 8}},
	#endif
#ifdef PROJECT_BX11
	.matrix_nodes = {{.origin = 0, .size = 2}, {.origin =  2, .size = 2}},
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
	
	.chip_cb = {
	#ifdef OBJECT_T37_DEBUG_PLATFORM_INFO
		.get_sigrow = tsapi_get_signature_row_data,
	#endif
	}
};