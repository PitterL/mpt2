
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

#ifndef __TSLIB__
#define __TSLIB__

#include "tswrapper.h"

/*----------------------------------------------------------------------------
 *     Global Varibles Declarations
 *----------------------------------------------------------------------------*/

/* Acquisition timer schedule interval */
/*  USE_MPTT_WRAPPER:
  set 3 varibles for the measurement period: active and idle by `measurement_period_store`
  `measurement_period_store`: current used measurement period
  `measurement_period_active_store`: active measurement period setting
  `measurement_period_idle_store`: idle measurement period setting(for low power mode)
  */
extern uint16_t measurement_period_store;
#define GET_MEASUREMENT_PERIOD()  (measurement_period_store)

/*
  USE_MPTT_WRAPPER:
  Active Acquisition Interval
  Range: [0~255], unit 1ms
  0: suspend
  1 ~ 255: actual value with 1ms unit
  */
extern uint8_t measurement_period_active_store;
#define GET_MEASUREMENT_PERIOD_ACTIVE()  (measurement_period_active_store)

/*  
  USE_MPTT_WRAPPER:
  Idle Acquisition Interval
  0: disable sleep mode  
  1 ~ 255: actual value with 1ms unit
*/
extern uint8_t measurement_period_idle_store;
#define GET_MEASUREMENT_PERIOD_IDLE()  (measurement_period_idle_store)

/*
  USE_MPTT_WRAPPER:
  The Active to Idle Timeout from last touch.
  Range: [0~255], unit 200ms
  0: means never enter into idle automatically
  1 ~ 255: actual value with 200ms unit
*/
extern uint8_t measurement_active_to_idle;
#define GET_MEASUREMENT_PERIOD_ACTIVE_TO_IDLE()  (measurement_active_to_idle)

/*
  USE_MPTT_WRAPPER:
  Idle Drift Interval
  0: disabled in sleep mode  
  1 ~ 255: actual value with 200ms unit. 
  Note: maximum value limited by watchdog window period
*/
extern uint8_t measurement_period_idle_drift;
#define GET_MEASUREMENT_PERIOD_IDLE_DRIFT()  (measurement_period_idle_drift)


/*----------------------------------------------------------------------------
 *     Structure Declarations
 *----------------------------------------------------------------------------*/

#define EXTRACT_N(_ptrcall, _n) (_ptrcall()[(_n)])
#define EXTRACT_I(_itemcall, _i) (_itemcall()._i)
#define EXTRACT_NI(_ptrcall, _n, _i) (_ptrcall()[(_n)]._i)

/* Acquisition module internal data - Size to largest acquisition set */
extern signal_t qtm_acq_signals_raw1[];
#define GET_ACQ_SIG_RAW_PTR() (qtm_acq_signals_raw1)
#define GET_ACQ_SIG_RAW_n(_n) EXTRACT_N(GET_ACQ_SIG_RAW_PTR, _n)

/* Acquisition set 1 - General settings */
extern qtm_acq_node_group_config_t qtm_acq_gen1;
#define GET_ACQ_NODE_GRP_CFG()  (qtm_acq_gen1)
#define GET_ACQ_NODE_GRP_CFG_i(_i)  EXTRACT_I(GET_ACQ_NODE_GRP_CFG, _i)

/* Node configurations */
extern qtm_acq_node_config_t qtm_seq_node_cfg1[];
#define GET_SEQ_NODE_CFG_PTR()  (qtm_seq_node_cfg1)
#define GET_SEQ_NODE_CFG_n(_n)  EXTRACT_N(GET_SEQ_NODE_CFG_PTR, _n)
#define GET_SEQ_NODE_CFG_ni(_n, _i)  EXTRACT_NI(GET_SEQ_NODE_CFG_PTR, _n, _i)

/* Node status, signal, calibration values */
extern qtm_acq_node_data_t qtm_node_stat1[];
#define GET_NODE_STAT_PTR() (qtm_node_stat1)
#define GET_NODE_STAT_n(_n) EXTRACT_N(GET_NODE_STAT_PTR, _n)
#define GET_NODE_STAT_ni(_n, _i) EXTRACT_NI(GET_NODE_STAT_PTR, _n, _i)

/* Node Container */
extern qtm_acquisition_control_t qtlib_acq_set1;
#define GET_ACQ_SET() (qtlib_acq_set1)
#define GET_ACQ_SET_i(_i) EXTRACT_I(GET_ACQ_SET, _i)

/* Low-power autoscan related parameters */
extern qtm_auto_scan_config_t auto_scan_setup;
#define GET_AUTO_SCAN_SETUP() (auto_scan_setup)
#define GET_AUTO_SCAN_SETUP_i(_i) EXTRACT_I(GET_AUTO_SCAN_SETUP, _i)

/**********************************************************/
/*********** Frequency Hop Module **********************/
/**********************************************************/

/* Buffer used with various noise filtering functions */
extern signal_t qtm_noise_filter_buffer[];
extern uint8_t  qtm_freq_hop_delay_selection[];


/* Configuration */
extern qtm_freq_hop_config_t qtm_freq_hop_config1;

/* Data */
extern qtm_freq_hop_data_t qtm_freq_hop_data1;
#define GET_FREQ_HOP_DATA_SET() (qtm_freq_hop_data1)
#define GET_FREQ_HOP_DATA_SET_i(_i) EXTRACT_I(GET_FREQ_HOP_DATA_SET, _i)

/* Freq Hop Container */
extern qtm_freq_hop_control_t qtlib_freq_hop_set1;
#define GET_FREQ_HOP_SET() (qtlib_freq_hop_set1)
#define GET_FREQ_HOP_SET_i(_i) EXTRACT_I(GET_FREQ_HOP_SET, _i)


/**********************************************************/
/*********************** Keys Module **********************/
/**********************************************************/

/* Keys set 1 - General settings */
extern qtm_touch_key_group_config_t qtm_key_grp_config_set1;
#define GET_KEY_GRP_CFG_SET() (qtm_key_grp_config_set1)
#define GET_KEY_GRP_CFG_SET_i(_i) EXTRACT_I(GET_KEY_GRP_CFG_SET, _i)

extern qtm_touch_key_group_data_t qtm_key_grp_data_set1;
#define GET_KEY_GRP_DATA_SET() (qtm_key_grp_data_set1)
#define GET_KEY_GRP_DATA_SET_i(_i) EXTRACT_I(GET_KEY_GRP_DATA_SET, _i)

/* Key data */
extern qtm_touch_key_data_t qtm_key_data_set1[];
#define GET_KEY_DATA_SET_PTR()  (qtm_key_data_set1)
#define GET_KEY_DATA_SET_n(_n)  EXTRACT_N(GET_KEY_DATA_SET_PTR, _n)
#define GET_KEY_DATA_SET_ni(_n, _i)  EXTRACT_NI(GET_KEY_DATA_SET_PTR, _n, _i)

/* Key Configurations */
extern qtm_touch_key_config_t qtm_key_configs_set1[];
#define GET_KEY_CFG_SET_PTR() (qtm_key_configs_set1)
#define GET_KEY_CFG_n(_n) EXTRACT_N(GET_KEY_CFG_SET_PTR, _n)
#define GET_KEY_CFG_ni(_n, _i) EXTRACT_NI(GET_KEY_CFG_SET_PTR, _n, _i)

/* Key Container */
extern qtm_touch_key_control_t qtlib_key_set1;
#define GET_KEY_SET()  (qtlib_key_set1)
#define GET_KEY_SET_i(_i)  EXTRACT_I(GET_KEY_SET, _i)


#ifdef TOUCH_API_SCROLLER_H

/* scroller Configurations */
extern qtm_scroller_config_t qtm_scroller_config1[];
#define GET_SCROLLER_CFG_PTR()  (qtm_scroller_config1)
#define GET_SCROLLER_CFG_n(_n)  EXTRACT_N(GET_SCROLLER_CFG_PTR, _n)
#define GET_SCROLLER_CFG_ni(_n, _i)  EXTRACT_NI(GET_SCROLLER_CFG_PTR, _n, _i)

/* Group Configuration */
extern qtm_scroller_group_config_t qtm_scroller_group_config1;
#define GET_SCROLLER_GRP_CFG()  (qtm_scroller_group_config1)
#define GET_SCROLLER_GRP_CFG_i(_i)  EXTRACT_I(GET_SCROLLER_GRP_CFG, _i)

/* Individual and Group Data */
extern qtm_scroller_data_t qtm_scroller_data1[];
#define GET_SCROLLER_DATA_PTR()  (qtm_scroller_data1)
#define GET_SCROLLER_DATA_n(_n)  EXTRACT_N(GET_SCROLLER_DATA_PTR, _n)
#define GET_SCROLLER_DATA_ni(_n, _i)  EXTRACT_NI(GET_SCROLLER_DATA_PTR, _n, _i)

/* Container */
extern qtm_scroller_control_t qtm_scroller_control1;
#define GET_SCROLLER_CTRL()  (qtm_scroller_control1)
#define GET_SCROLLER_CTRL_i(_i)  EXTRACT_I(GET_SCROLLER_CTRL, _i)

#endif

/* Surface Config */
#if (defined(TOUCH_API_SURFACE_CS2T_H) || defined(TOUCH_API_SURFACE_CS_H))

extern qtm_surface_cs_config_t qtm_surface_cs_config1;
#define GET_SURFACE_CS_CFG()  (qtm_surface_cs_config1)
#define GET_SURFACE_CS_CFG_i(_i)  EXTRACT_I(GET_SURFACE_CS_CFG, _i)

/* Surface Data */
extern qtm_surface_csnt_data_t qtm_surface_cs_data1;
#define GET_SURFACE_CS_DATA()  (qtm_surface_cs_data1)
#define GET_SURFACE_CS_DATA_i(_i)  EXTRACT_I(GET_SURFACE_CS_DATA, _i)

/* Contact Data */
qtm_surface_contact_data_t qtm_surface_contacts[2];
#define GET_SURFACE_CONTACT_DATA_PTR()  (qtm_surface_contacts)
#define GET_SURFACE_CONTACT_DATA_n(_n)  EXTRACT_N(GET_SURFACE_CONTACT_DATA_PTR, _n)
#define GET_SURFACE_CONTACT_DATA_ni(_n, _i)  EXTRACT_NI(GET_SURFACE_CONTACT_DATA_PTR, _n, _i)

/* Container */
extern qtm_surface_csnt_control_t qtm_surface_cs_control1;
#define GET_SURFACE_CS_CTRL()  (qtm_surface_cs_control1)
#define GET_SURFACE_CS_CTRL_i(_i)  EXTRACT_I(qtm_surface_cs_control1, _i)

#endif

/*----------------------------------------------------------------------------
 *     Function Declarations
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
touch_ret_t tslib_init(void);

/*============================================================================
void touch_adc_init(void)
------------------------------------------------------------------------------
Purpose: Initialization of touch acqusition. ADC registers and state only.
Input  : none
Output : touch_ret_t
Notes  :
============================================================================*/
touch_ret_t touch_adc_init(void);

/*============================================================================
void touch_adc_deinit(void)
------------------------------------------------------------------------------
Purpose: De-initialization of touch acqusition. ADC registers and state only.
Input  : none
Output : touch_ret_t
Notes  :
============================================================================*/
touch_ret_t touch_adc_deinit(void);

/* Measurement start - sequence or windowcomp */

/*============================================================================
touch_ret_t tslib_touch_start_measurement_seq(void (*measure_complete_callback)(void));
------------------------------------------------------------------------------
Purpose:  Loads touch configurations for first channel and start,
Input  :  the callback when measurement complete
Output : touch_ret_t:
Notes  :
*/
touch_ret_t tslib_touch_start_measurement_seq(void (*measure_complete_callback)(void));

/* Library prototypes */
/*============================================================================
touch_ret_t tslib_acquisition_process(void)
------------------------------------------------------------------------------
Purpose: Signal capture and processing
Input  : (Measured signals, config)
Output : TOUCH_SUCCESS or TOUCH_CAL_ERROR
Notes  : none
============================================================================*/
touch_ret_t tslib_acquisition_process(void);

/*============================================================================
touch_ret_t autoscan_sensor_node(qtm_auto_scan_config_t* qtm_auto_scan_config_ptr, void (*auto_scan_callback)(void))
------------------------------------------------------------------------------
Purpose: Configures the ADC for sleep mode measurement of a single node, with window comparator wake
Input  : Acquisition set, channel number, threshold, scan trigger; if the callback is set, using hardware mode, otherwise using software mode
Output : touch_ret_t
Notes  : none
============================================================================*/
touch_ret_t tslib_autoscan_sensor_node(qtm_auto_scan_config_t* qtm_auto_scan_config_ptr, void (*auto_scan_callback)(void));

/*============================================================================
touch_ret_t tslib_autoscan_node_cancel(void)
------------------------------------------------------------------------------
Purpose: Cancel auto-scan config
Input  : None
Output : touch_ret_t
Notes  : none
============================================================================*/
touch_ret_t tslib_autoscan_node_cancel(void);

/*============================================================================
touch_ret_t tslib_done(void)
------------------------------------------------------------------------------
Purpose: Resource handle called after touch processed
Input  : (Measured signals, config)
Output : TOUCH_SUCCESS
Notes  : none
============================================================================*/
touch_ret_t tslib_done(void);

/*============================================================================
touch_ret_t tslib_freq_hop(void);
------------------------------------------------------------------------------
Purpose: Runs freq hop process
Input  : None
Output : touch_ret_t
Notes  : none
============================================================================*/
touch_ret_t tslib_freq_hop(void);

/*============================================================================
touch_ret_t tslib_key_sensors_process(void)
------------------------------------------------------------------------------
Purpose: Sensor key post-processing (touch detect state machine)
Input  : None
Output : TOUCH_SUCCESS
Notes  : none
============================================================================*/
touch_ret_t tslib_key_sensors_process(void);

/*============================================================================
void tslib_key_sensors_update_timer(uint16_t interval)
------------------------------------------------------------------------------
Purpose: Sensor key time updating
Input  : The time interval past
Output : none
Notes  : none
============================================================================*/
void tslib_key_sensors_update_timer(uint16_t interval);

/*============================================================================
touch_ret_t tslib_key_sensors_reburst(void)
------------------------------------------------------------------------------
Purpose: Check whether key sensor should process reburst
Input  : None
Output : TOUCH_KEY_REBURST
Notes  : none
============================================================================*/
touch_ret_t tslib_key_sensors_reburst(void);

/*============================================================================
touch_ret_t tslib_key_sensors_active(void)
------------------------------------------------------------------------------
Purpose: Check whether key sensor should process reburst
Input  : None
Output : TOUCH_KEY_ACTIVE
Notes  : none
============================================================================*/
touch_ret_t tslib_key_sensors_active(void);

/*============================================================================
touch_ret_t tslib_key_suspend(uint8_t which_sensor_key)
------------------------------------------------------------------------------
Purpose: Suspends acquisition measurements for the key
Input  : Which key number
Output : TOUCH_SUCCESS
Notes  : none
============================================================================*/
touch_ret_t tslib_key_suspend(uint8_t which_sensor_key);

/*============================================================================
touch_ret_t tslib_key_resume(uint8_t which_sensor_key)
------------------------------------------------------------------------------
Purpose: Resumes acquisition measurements for the key
Input  : Which key number
Output : TOUCH_SUCCESS
Notes  : none
============================================================================*/
touch_ret_t tslib_key_resume(uint8_t which_sensor_key);

/*============================================================================
uint8_t tslib_num_key_sensors(void)
------------------------------------------------------------------------------
Purpose: number of sensors for keys
Input  : None
Output : int8_t
Notes  : none
============================================================================*/
uint8_t tslib_num_key_sensors(void);

/*============================================================================
uint8_t tslib_num_channel_nodes(void)
------------------------------------------------------------------------------
Purpose: number of channels for nodes
Input  : None
Output : int8_t
Notes  : none
============================================================================*/
uint8_t tslib_num_channel_nodes(void);

/*============================================================================
uint8_t tslib_get_sensor_key_mapping(uint8_t sensor_key, uint8_t group)
------------------------------------------------------------------------------
Purpose: Key sensor to channel node id/group mapping
Input  : None
Output : int8_t
Notes  : none
============================================================================*/
uint8_t tslib_get_sensor_key_mapping(uint8_t which_sensor_key, uint8_t group);

/*============================================================================
uint8_t tslib_get_channel_node_mapping(uint8_t channel_node)
------------------------------------------------------------------------------
Purpose: Channel node to key sensor mapping
Input  : None
Output : int8_t
Notes  : none
============================================================================*/
uint8_t tslib_get_channel_node_mapping(uint8_t which_channel_node);

/*============================================================================
touch_ret_t tslib_get_sensor_state(uint8_t which_sensor_key)
------------------------------------------------------------------------------
Purpose: Get key sensor state
Input  : Which key number
Output : int8_t
Notes  : none
============================================================================*/
uint8_t tslib_get_sensor_state(uint8_t which_sensor_key);

/*============================================================================
signal_t tslib_get_channel_node_signal(uint8_t channel_node)
------------------------------------------------------------------------------
Purpose: Get channel node signal
Input  : Which channel
Output : int16_t
Notes  : none
============================================================================*/
signal_t tslib_get_channel_node_signal(uint8_t channel_node);

/*============================================================================
void tslib_update_channel_node_signal(uint8_t channel_node, signal_t new_signal)
------------------------------------------------------------------------------
Purpose: Update channel node signal
Input  : Which channel
Output : int16_t
Notes  : none
============================================================================*/
void tslib_update_channel_node_signal(uint8_t channel_node, signal_t new_signal);

/*============================================================================
signal_t tslib_get_sensor_key_signal(uint8_t sensor_key)
------------------------------------------------------------------------------
Purpose: Get key sensor signal
Input  : Which key number
Output : int16_t
Notes  : none
============================================================================*/
signal_t tslib_get_sensor_key_signal(uint8_t sensor_key);

/*============================================================================
reference_t tslib_get_sensor_key_reference(uint8_t sensor_key)
------------------------------------------------------------------------------
Purpose: Get key sensor reference
Input  : Which key number
Output : int16_t
Notes  : none
============================================================================*/
reference_t tslib_get_sensor_key_reference(uint8_t sensor_key);

/*============================================================================
void update_sensor_key_reference(uint8_t sensor_key, reference_t new_reference);
------------------------------------------------------------------------------
Purpose: Update key sensor reference
Input  : Which key number
Output : int16_t
Notes  : none
============================================================================*/
void update_sensor_key_reference(uint8_t sensor_key, reference_t new_reference);

/*============================================================================
delta_t tslib_get_sensor_key_delta(uint8_t sensor_key)
------------------------------------------------------------------------------
Purpose: Get key sensor delta
Input  : Which key number
Output : int16_t
Notes  : none
============================================================================*/
delta_t tslib_get_sensor_key_delta(uint8_t sensor_key);

/*============================================================================
capacitance_t tslib_get_sensor_ccomp_val(uint8_t sensor_key)
------------------------------------------------------------------------------
Purpose: Get key sensor ccomp value
Input  : Which key number
Output : int16_t
Notes  : none
============================================================================*/
capacitance_t tslib_get_sensor_ccomp_val(uint8_t sensor_key);

/*============================================================================
void tslib_update_sensor_ccomp_val(uint8_t sensor_key, capacitance_t new_ccomp_value)
------------------------------------------------------------------------------
Purpose: Update key sensor ccomp value
Input  : Which key number
Output : int16_t
Notes  : none
============================================================================*/
void tslib_update_sensor_ccomp_val(uint8_t sensor_key, capacitance_t new_ccomp_value);

/*============================================================================
void tslib_calibrate_sensor_key(uint8_t sensor_key)
------------------------------------------------------------------------------
Purpose: Calibrate the key sensor
Input  : Which key number
Output : int16_t
Notes  : none
============================================================================*/
void tslib_calibrate_sensor_key(uint8_t sensor_key);

/*============================================================================
void tslib_calibrate_all_nodes(void)
------------------------------------------------------------------------------
Purpose: Calibrate all sensor nodes
Input  : none
Output : int16_t
Notes  : none
============================================================================*/
void tslib_calibrate_all_sensors(void);

/*============================================================================
void tslib_enable_all_sensors(void)
------------------------------------------------------------------------------
Purpose: enable all sensor nodes
Input  : none
Output : none
Notes  : none
============================================================================*/
void tslib_enable_all_sensors(void);

/*============================================================================
void tslib_init_all_sensors(void)
------------------------------------------------------------------------------
Purpose: init all sensor nodes
Input  : none
Output : int16_t
Notes  : none
============================================================================*/
void tslib_init_all_sensors(void);

/*============================================================================
void tslib_init_slider_surfaces(void)
------------------------------------------------------------------------------
Purpose: init all slider surfaces
Input  : none
Output : int16_t
Notes  : none
============================================================================*/
void tslib_init_slider_surfaces(void);

/*============================================================================
void tslib_set_adc_isr_handler(void (* handler)(void))
------------------------------------------------------------------------------
Purpose: Set interrupt handler for ADC / PTC EOC Interrupt
Input  : isr handelr
Output : none
Notes  : none
============================================================================*/
void tslib_set_adc_isr_handler(void (* handler)(void));

/*============================================================================
void tslib_adc_handler_eoc(void)
------------------------------------------------------------------------------
Purpose: The library interrupt handler for ADC / PTC EOC Interrupt
Input  : none
Output : none
Notes  : none
============================================================================*/
void tslib_adc_handler_eoc(void);

/*============================================================================
void tslib_set_wcomp_isr_handler(void (* handler)(void))
------------------------------------------------------------------------------
Purpose: Set interrupt handler for ADC / WCOMP Interrupt
Input  : isr handelr
Output : none
Notes  : none
============================================================================*/
void tslib_set_wcomp_isr_handler(void (* handler)(void));

/*============================================================================
void tslib_adc_handler_wcomp(void)
------------------------------------------------------------------------------
Purpose: The library interrupt handler for ADC / WCOMP Interrupt
Input  : none
Output : none
Notes  : none
============================================================================*/
void tslib_adc_handler_wcomp(void);

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
void datastreamer_output(void);

#endif
