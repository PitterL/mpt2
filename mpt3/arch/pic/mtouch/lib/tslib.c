/*============================================================================
Filename : tslib.c
Project : QTouch Modular Wrapper
Purpose : Wrap the Qtouch Library to Api call
------------------------------------------------------------------------------
Version Number : 0.1
Last Updated : 5/3/2022
Updated By : Pitter Liao
------------------------------------------------------------------------------
Copyright (c) 2022 Microchip. All rights reserved.
------------------------------------------------------------------------------
Revision 0.1 - Initialize
============================================================================*/

/*----------------------------------------------------------------------------
 *     Include files 
 *----------------------------------------------------------------------------*/
#include <mcc.h>
#include <arch/cpu.h>
#include "tslib.h"

/*----------------------------------------------------------------------------
 *     Global Variables
 *----------------------------------------------------------------------------*/

/*============================================================================
void touch_init(void)
------------------------------------------------------------------------------
Purpose: Initialization of touch library. PTC, timer, and
         datastreamer modules are initialized in this function.
Input  : none
Output : touch_ret_t
Notes  :
============================================================================*/
touch_ret_t tslib_init(void)
{
	uint16_t    channel_node, sensor_key;
	touch_ret_t touch_ret = TOUCH_SUCCESS;

	/* Init acquisition module */
	qtm_init_acquisition_module(&GET_ACQ_SET());

	/* Init pointers to DMA sequence memory */
	qtm_assign_signal_memory(GET_ACQ_SIG_RAW_PTR());

	/* Enable all sensor nodes */
	tslib_enable_all_sensors();

    /* Calibration sensor nodes */
    tslib_calibrate_all_sensors();

	/* Initialize keys and assign nodes */
    tslib_init_all_sensors();

	return (touch_ret);
}

/*============================================================================
void touch_acquisition_init(void)
------------------------------------------------------------------------------
Purpose: Initialization of touch acqusition. ADC registers and state only.
Input  : none
Output : touch_ret_t
Notes  :
============================================================================*/
touch_ret_t touch_adc_init(void)
{
    return qtm_init_acquisition_module(&GET_ACQ_SET());
}

/*============================================================================
void touch_acquisition_deinit(void)
------------------------------------------------------------------------------
Purpose: De-initialization of touch acqusition. ADC registers and state only.
Input  : none
Output : touch_ret_t
Notes  :
============================================================================*/
touch_ret_t touch_adc_deinit(void)
{
    return qtm_adc_de_init(&GET_ACQ_SET());
}

/* Measurement start - sequence or windowcomp */

/*============================================================================
touch_ret_t tslib_touch_start_measurement_seq(void (*measure_complete_callback)(void));
------------------------------------------------------------------------------
Purpose:  Loads touch configurations for first channel and start,
Input  :  the callback when measurement complete
Output : touch_ret_t:
Notes  :
*/
touch_ret_t tslib_touch_start_measurement_seq(void (*measure_complete_callback)(void))
{
    /* This function will execute the scanning return True if all sensors are completed scanning */
    return qtm_adc_start_measurement_seq(&GET_ACQ_SET(), measure_complete_callback);
}

/* Library prototypes */
/*============================================================================
touch_ret_t tslib_acquisition_process(void)
------------------------------------------------------------------------------
Purpose: Signal capture and processing
Input  : (Measured signals, config)
Output : TOUCH_SUCCESS or TOUCH_CAL_ERROR
Notes  : none
============================================================================*/
touch_ret_t tslib_acquisition_process(void)
{
    return qtm_acquisition_process();
}

/*============================================================================
touch_ret_t autoscan_sensor_node(qtm_auto_scan_config_t* qtm_auto_scan_config_ptr, void (*auto_scan_callback)(void))
------------------------------------------------------------------------------
Purpose: Configures the ADC for sleep mode measurement of a single node, with window comparator wake
Input  : Acquisition set, channel number, threshold, scan trigger; if the callback is set, using hardware mode, otherwise using software mode
Output : touch_ret_t
Notes  : none
============================================================================*/
touch_ret_t tslib_autoscan_sensor_node(qtm_auto_scan_config_t* qtm_auto_scan_config_ptr, void (*auto_scan_callback)(void))
{
    return qtm_autoscan_sensor_node(qtm_auto_scan_config_ptr, auto_scan_callback);
}

/*============================================================================
touch_ret_t tslib_autoscan_node_cancel(void)
------------------------------------------------------------------------------
Purpose: Cancel auto-scan config
Input  : None
Output : touch_ret_t
Notes  : none
============================================================================*/
touch_ret_t tslib_autoscan_node_cancel(void)
{
    return qtm_autoscan_node_cancel();
}

/*============================================================================
touch_ret_t tslib_done(void)
------------------------------------------------------------------------------
Purpose: Resource handle called after touch processed
Input  : (Measured signals, config)
Output : TOUCH_SUCCESS
Notes  : none
============================================================================*/
touch_ret_t tslib_done(void)
{   
    return TOUCH_SUCCESS;
}

/*============================================================================
touch_ret_t tslib_freq_hop(void);
------------------------------------------------------------------------------
Purpose: Runs freq hop process
Input  : None
Output : touch_ret_t
Notes  : none
============================================================================*/
touch_ret_t tslib_freq_hop(void)
{
    return qtm_freq_hop(&GET_FREQ_HOP_SET());
}

/*============================================================================
touch_ret_t tslib_key_sensors_process(void)
------------------------------------------------------------------------------
Purpose: Sensor key post-processing (touch detect state machine)
Input  : None
Output : TOUCH_SUCCESS
Notes  : none
============================================================================*/
touch_ret_t tslib_key_sensors_process(void)
{
    return qtm_key_sensors_process(&GET_KEY_SET());
}

/*============================================================================
void tslib_key_sensors_update_timer(uint16_t interval)
------------------------------------------------------------------------------
Purpose: Sensor key time updating
Input  : The time interval past
Output : none
Notes  : none
============================================================================*/
void tslib_key_sensors_update_timer(uint16_t interval)
{
    return qtm_update_qtlib_timer(interval);
}

/*============================================================================
touch_ret_t tslib_key_sensors_reburst(void)
------------------------------------------------------------------------------
Purpose: Check whether key sensor should process reburst
Input  : None
Output : TOUCH_KEY_REBURST
Notes  : none
============================================================================*/
touch_ret_t tslib_key_sensors_reburst(void)
{
    if ((0u != (GET_KEY_GRP_DATA_SET_i(qtm_keys_status) & QTM_KEY_REBURST/*0x80u*/))) {
        return TOUCH_KEY_REBURST;
    } else {
        return TOUCH_SUCCESS;
    }
}

/*============================================================================
touch_ret_t tslib_key_sensors_active(void)
------------------------------------------------------------------------------
Purpose: Check whether key sensor should process reburst
Input  : None
Output : TOUCH_KEY_ACTIVE
Notes  : none
============================================================================*/
touch_ret_t tslib_key_sensors_active(void)
{
    if (0u != (GET_KEY_GRP_DATA_SET_i(qtm_keys_status) & QTM_KEY_DETECT/*0x01u*/)) {
        return TOUCH_KEY_ACTIVE;
    } else {
        return TOUCH_SUCCESS;
    }
}

/*============================================================================
touch_ret_t tslib_key_suspend(uint8_t which_sensor_key)
------------------------------------------------------------------------------
Purpose: Suspends acquisition measurements for the key
Input  : Which key number
Output : TOUCH_SUCCESS
Notes  : none
============================================================================*/
touch_ret_t tslib_key_suspend(uint8_t which_sensor_key)
{
    return qtm_key_suspend(which_sensor_key, &GET_KEY_SET());
}

/*============================================================================
touch_ret_t tslib_key_resume(uint8_t which_sensor_key)
------------------------------------------------------------------------------
Purpose: Resumes acquisition measurements for the key
Input  : Which key number
Output : TOUCH_SUCCESS
Notes  : none
============================================================================*/
touch_ret_t tslib_key_resume(uint8_t which_sensor_key)
{
    return qtm_key_resume(which_sensor_key, &GET_KEY_SET());
}

/*============================================================================
uint8_t tslib_num_key_sensors(void)
------------------------------------------------------------------------------
Purpose: number of sensors for keys
Input  : None
Output : int8_t
Notes  : none
============================================================================*/
uint8_t tslib_num_key_sensors(void)
{
    return (uint8_t)GET_KEY_GRP_CFG_SET_i(num_key_sensors);
}

/*============================================================================
uint8_t tslib_num_channel_nodes(void)
------------------------------------------------------------------------------
Purpose: number of channels for nodes
Input  : None
Output : int8_t
Notes  : none
============================================================================*/
uint8_t tslib_num_channel_nodes(void)
{
    return GET_ACQ_SET_i(qtm_acq_node_group_config)->num_channel_nodes;
}

/*============================================================================
uint8_t tslib_get_sensor_key_mapping(uint8_t sensor_key, uint8_t group)
------------------------------------------------------------------------------
Purpose: Key sensor to channel node id/group mapping
Input  : which_sensor_key, the the sensor group
Output : int8_t
Notes  : none
============================================================================*/
uint8_t tslib_get_sensor_key_mapping(uint8_t which_sensor_key, uint8_t group)
{	
    /* group is unused for 1x, every  */
	return which_sensor_key;
}

/*============================================================================
uint8_t tslib_get_channel_node_mapping(uint8_t channel_node)
------------------------------------------------------------------------------
Purpose: Channel node to key sensor mapping
Input  : None
Output : int8_t
Notes  : none
============================================================================*/
uint8_t tslib_get_channel_node_mapping(uint8_t which_channel_node)
{
	return which_channel_node;
}

/*============================================================================
uint8_t tslib_get_sensor_state(uint8_t which_sensor_key)
------------------------------------------------------------------------------
Purpose: Get key sensor state
Input  : Which key number
Output : int8_t
Notes  : none
============================================================================*/
uint8_t tslib_get_sensor_state(uint8_t which_sensor_key)
{
    return (GET_KEY_DATA_SET_ni(which_sensor_key ,sensor_state) & ~KEY_TOUCHED_MASK);
}

/*============================================================================
signal_t tslib_get_channel_node_signal(uint8_t channel_node)
------------------------------------------------------------------------------
Purpose: Get channel node signal
Input  : Which channel
Output : int16_t
Notes  : none
============================================================================*/
signal_t tslib_get_channel_node_signal(uint8_t channel_node)
{ 
    /* Return the channel node signal */
    return GET_ACQ_SET_i(qtm_acq_node_data)[channel_node].node_acq_signals;
}

/*============================================================================
void tslib_update_channel_node_signal(uint8_t channel_node, signal_t new_signal)
------------------------------------------------------------------------------
Purpose: Update channel node signal
Input  : Which channel
Output : int16_t
Notes  : none
============================================================================*/
void tslib_update_channel_node_signal(uint8_t channel_node, signal_t new_signal)
{
	GET_ACQ_SET_i(qtm_acq_node_data)[channel_node].node_acq_signals = new_signal;
}

/*============================================================================
signal_t tslib_get_sensor_key_signal(uint8_t sensor_key)
------------------------------------------------------------------------------
Purpose: Get key sensor signal
Input  : Which key number
Output : int16_t
Notes  : none
============================================================================*/
signal_t tslib_get_sensor_key_signal(uint8_t sensor_key)
{ 
    return GET_KEY_DATA_SET_ni(sensor_key ,node_data_struct_ptr)->node_acq_signals;
}

/*============================================================================
reference_t tslib_get_sensor_key_reference(uint8_t sensor_key)
------------------------------------------------------------------------------
Purpose: Get key sensor reference
Input  : Which key number
Output : int16_t
Notes  : none
============================================================================*/
reference_t tslib_get_sensor_key_reference(uint8_t sensor_key)
{
    return GET_KEY_DATA_SET_ni(sensor_key ,channel_reference);
}

/*============================================================================
void update_sensor_key_reference(uint8_t sensor_key, reference_t new_reference)
------------------------------------------------------------------------------
Purpose: Update key sensor reference
Input  : Which key number
Output : int16_t
Notes  : none
============================================================================*/
void update_sensor_key_reference(uint8_t sensor_key, reference_t new_reference)
{
	GET_KEY_DATA_SET_ni(sensor_key ,channel_reference) = new_reference;
}

/*============================================================================
delta_t tslib_get_sensor_key_delta(uint8_t sensor_key)
------------------------------------------------------------------------------
Purpose: Get key sensor delta
Input  : Which key number
Output : int16_t
Notes  : none
============================================================================*/
delta_t tslib_get_sensor_key_delta(uint8_t sensor_key)
{
	return tslib_get_sensor_key_signal(sensor_key) - tslib_get_sensor_key_reference(sensor_key);
}

/*============================================================================
capacitance_t tslib_get_sensor_ccomp_val(uint8_t sensor_key)
------------------------------------------------------------------------------
Purpose: Get key sensor ccomp value
Input  : Which key number
Output : int16_t
Notes  : none
============================================================================*/
capacitance_t tslib_get_sensor_ccomp_val(uint8_t sensor_key)
{
    return GET_KEY_DATA_SET_ni(sensor_key ,node_data_struct_ptr)->node_comp_caps;
}

/*============================================================================
void tslib_update_sensor_ccomp_val(uint8_t sensor_key, capacitance_t new_ccomp_value)
------------------------------------------------------------------------------
Purpose: Update key sensor ccomp value
Input  : Which key number
Output : int16_t
Notes  : none
============================================================================*/
void tslib_update_sensor_ccomp_val(uint8_t sensor_key, capacitance_t new_ccomp_value)
{
	GET_KEY_DATA_SET_ni(sensor_key ,node_data_struct_ptr)->node_comp_caps = new_ccomp_value;
}

/*============================================================================
void tslib_calibrate_sensor_key(uint8_t sensor_key)
------------------------------------------------------------------------------
Purpose: Calibrate the key sensor
Input  : Which key number
Output : int16_t
Notes  : none
============================================================================*/
void tslib_calibrate_sensor_key(uint8_t sensor_key)
{
    uint8_t channel_node = tslib_get_sensor_key_mapping(sensor_key, 0);

    qtm_calibrate_sensor_node(&GET_ACQ_SET(), channel_node);
}

/*============================================================================
void tslib_calibrate_all_nodes(void)
------------------------------------------------------------------------------
Purpose: Calibrate all sensor nodes
Input  : none
Output : int16_t
Notes  : none
============================================================================*/
void tslib_calibrate_all_sensors(void)
{
    uint8_t num_channel_nodes = tslib_num_channel_nodes();
	uint8_t i;

	for ( i = 0; i < num_channel_nodes; i++) {
		/* Calibrate Node */
		qtm_calibrate_sensor_node(&GET_ACQ_SET(), i);
	}
}

/*============================================================================
void tslib_enable_all_sensors(void)
------------------------------------------------------------------------------
Purpose: enable all sensor nodes
Input  : none
Output : none
Notes  : none
============================================================================*/
void tslib_enable_all_sensors(void)
{
    uint8_t num_channel_nodes = tslib_num_channel_nodes();
	uint8_t i;

	for ( i = 0; i < num_channel_nodes; i++) {
        qtm_enable_sensor_node(&GET_ACQ_SET(), i);
    }
}

/*============================================================================
void tslib_init_all_sensors(void)
------------------------------------------------------------------------------
Purpose: init all sensor nodes
Input  : none
Output : int16_t
Notes  : none
============================================================================*/
void tslib_init_all_sensors(void)
{
    uint8_t num_key_sensors = tslib_num_key_sensors();
	uint8_t i;

    for ( i = 0; i < num_key_sensors; i++ ) {
        qtm_init_sensor_key(
            &GET_KEY_SET(), i, GET_ACQ_SET_i(qtm_acq_node_data) + i);
    }
}

/*============================================================================
void tslib_set_adc_isr_handler(void (* handler)(void))
------------------------------------------------------------------------------
Purpose: Set interrupt handler for ADC / PTC EOC Interrupt
Input  : isr handelr
Output : none
Notes  : none
============================================================================*/
void tslib_set_adc_isr_handler(void (* handler)(void))
{
	ADCC_SetADIInterruptHandler(handler);
}

/*============================================================================
void tslib_adc_handler_eoc(void)
------------------------------------------------------------------------------
Purpose: The library interrupt handler for ADC / PTC EOC Interrupt
Input  : none
Output : none
Notes  : none
============================================================================*/
void tslib_adc_handler_eoc(void)
{
	qtm_adc_handler_eoc();
}

/*============================================================================
void tslib_set_wcomp_isr_handler(void (* handler)(void))
------------------------------------------------------------------------------
Purpose: Set interrupt handler for ADC / WCOMP Interrupt
Input  : isr handelr
Output : none
Notes  : none
============================================================================*/
void tslib_set_wcomp_isr_handler(void (* handler)(void))
{
	ADCC_SetADTIInterruptHandler(handler);
}

/*============================================================================
void tslib_adc_handler_wcomp(void)
------------------------------------------------------------------------------
Purpose: The library interrupt handler for ADC / WCOMP Interrupt
Input  : none
Output : none
Notes  : none
============================================================================*/
void tslib_adc_handler_wcomp(void)
{
	qtm_adc_handler_wcomp();
}

/*============================================================================
void tslib_datastreamer_init(void)
------------------------------------------------------------------------------
Purpose: Initialize datastreamer frame based on the configured modules, Tranmits
         the frame as single packet through UART port.
Input  : none
Output : none
Notes  : The data visualizer scripts that are generated in the project should be
         set on the data visualizer software.
============================================================================*/
#ifdef DEF_TOUCH_DATA_STREAMER_ENABLE
void tslib_datastreamer_init(void)
{
  MTOUCH_DataStreamer_Initialize();
}
#endif

/*============================================================================
void datastreamer_output(void)
------------------------------------------------------------------------------
Purpose: Forms the datastreamer frame based on the configured modules, Tranmits
         the frame as single packet through UART port.
Input  : none
Output : none
Notes  : The data visualizer scripts that are generated in the project should be
         set on the data visualizer software.
============================================================================*/
#ifdef DEF_TOUCH_DATA_STREAMER_ENABLE
void tslib_datastreamer_output(void)
{
    MTOUCH_DataStreamer_Service();
}
#endif