/*
------------------------------------------------------------------------------
Copyright (C) 2022 Microchip. All rights reserved.
------------------------------------------------------------------------------

============================================================================
Filename : qtm_acq_q10_0x001b_api.h
Project : QTouch Modular Library
Purpose : API for Acquisition module - Q10 selfcap
------------------------------------------------------------------------------
Version Number : 0.1
Last Updated : 04/09/2022
Updated By : Pitter
Revision 0.1: Initialized
============================================================================*/

#ifndef TOUCH_API_T1617_ACQ_4P_MODULE_H
#define TOUCH_API_T1617_ACQ_4P_MODULE_H

#include <stdint.h>
#include <arch/cpu.h>
#include "qtm_common_components_api.h"

/* Node Pin map */
#define _AN_MSK(_n) ((_n)& 0x7)
#define _AN_GRP(_g) (((_g) & 7) << 3)
#define ANA(_n)  (_AN_GRP(0) | _AN_MSK(_n))
#define ANB(_n)  (_AN_GRP(1) | _AN_MSK(_n))
#define ANC(_n)  (_AN_GRP(2) | _AN_MSK(_n))
#define AND(_n)  (_AN_GRP(3) | _AN_MSK(_n))
#define ANE(_n)  (_AN_GRP(4) | _AN_MSK(_n))
#define AN_NONE	(1 << 7)

#define TO_AN_PORT(_an)  (((_an) >> 3) & 0x7)
#define TO_AN_PIN(_an)  ((_an) & 0x7)
#define NODE_UNUSED(_an)	((_an) & AN_NONE)

/* Calibration auto-tuning options */
#define CAL_OPTION_MASK 0x03u

#define CAL_AUTO_TUNE_NONE 0u
#define CAL_AUTO_TUNE_RSEL 1u
#define CAL_AUTO_TUNE_PRSC 2u
#define CAL_AUTO_TUNE_CSD 3u

/* Timing auto-cal target */
#define CAL_CHRG_TIME_MASK 0x30u
#define CAL_CHRG_TIME_POS 4u

#define CAL_CHRG_2TAU 0u
#define CAL_CHRG_3TAU 1u
#define CAL_CHRG_4TAU 2u
#define CAL_CHRG_5TAU 3u

#define RSEL_MAX_OPTION RSEL_VAL_200
#define PRSC_MAX_OPTION PRSC_DIV_SEL_256

/* X line bit position */
#define X_NONE 0u
#define X(n) ((uint16_t)(1u << (n)))

/* Y line bit position */
#define Y(n) ((uint16_t)(1u << (n)))

/* Extract Analog / Digital Gain */
#define NODE_GAIN_ANA(m) (uint8_t)(((m)&0xF0u) >> 4u)
#define NODE_GAIN_DIG(m) (uint8_t)((m)&0x0Fu)

/* Combine Analog / Digital Gain */
#define NODE_GAIN(a, d) (uint8_t)(((a) << 4u) | (d))

/* Extract Delay / Prescaler */
/* Combine Delay / Prescaler */

/* Physical channel to nodes */
#define TO_CHANNLES(n) ((n) >> 2)
#define TO_CHANNLE_GROUPS(n) ((n) >> 2)
#define TO_NODES(c)	((c) << 2)

/* Auto scan trigger sources */
#define NODE_SCAN_4MS 1u
#define NODE_SCAN_8MS 2u
#define NODE_SCAN_16MS 3u
#define NODE_SCAN_32MS 4u
#define NODE_SCAN_64MS 5u
#define NODE_SCAN_128MS 6u
#define NODE_SCAN_256MS 7u
#define NODE_SCAN_512MS 8u
#define NODE_SCAN_1024MS 9u
#define NODE_SCAN_2048MS 10u
#define NODE_SCAN_4096MS 11u

/*	USE_MPTT_WRAPPER, 
	We use 32K RTC clock permanently, so the scanning rate will below */
typedef enum tag_node_sleep_scan_rate_t {
	NODE_SLEEP_SCAN_RATE_2MS,
	NODE_SLEEP_SCAN_RATE_4MS,
	NODE_SLEEP_SCAN_RATE_8MS,
	NODE_SLEEP_SCAN_RATE_16MS,
	NODE_SLEEP_SCAN_RATE_32MS,
	NODE_SLEEP_SCAN_RATE_64MS,
	NODE_SLEEP_SCAN_RATE_128MS,
	NODE_SLEEP_SCAN_RATE_256MS,
	NODE_SLEEP_SCAN_RATE_LEVELS
} node_sleep_scan_rate_t;

typedef enum tag_filter_level_t {
	FILTER_LEVEL_1,
	FILTER_LEVEL_2,
	FILTER_LEVEL_4,
	FILTER_LEVEL_8,
	FILTER_LEVEL_16,
	FILTER_LEVEL_32,
	FILTER_LEVEL_64,
	FILTER_LEVEL_128,
} filter_level_t;

/* Touch library GAIN setting */
typedef enum tag_gain_t { GAIN_1, GAIN_2, GAIN_4, GAIN_8, GAIN_16, GAIN_32, GAIN_64, GAIN_128 } gain_t;

/* PTC clock prescale setting.
 * For Example: if Generic clock input to PTC = 2MHz, then:
 * PRSC_DIV_SEL_2 sets PTC Clock to 1MHz
 * PRSC_DIV_SEL_4 sets PTC Clock to 500KHz
 *
 */
typedef enum tag_prsc_div_sel_t {
	PRSC_DIV_SEL_2 = ADC_ADCLK_ADCS_DIV_N(2),
	PRSC_DIV_SEL_4 = ADC_ADCLK_ADCS_DIV_N(4),
	PRSC_DIV_SEL_8 = ADC_ADCLK_ADCS_DIV_N(8),
	PRSC_DIV_SEL_16 = ADC_ADCLK_ADCS_DIV_N(16),
	PRSC_DIV_SEL_32 = ADC_ADCLK_ADCS_DIV_N(32),
	PRSC_DIV_SEL_48 = ADC_ADCLK_ADCS_DIV_N(48),
	PRSC_DIV_SEL_64 = ADC_ADCLK_ADCS_DIV_N(64),
	PRSC_DIV_SEL_128 = ADC_ADCLK_ADCS_DIV_N(128),
	PRSC_DIV_SEL_AUTO = ADC_ADCLK_ADCS_DIV_N(XTAL_FREQ_MHZ),
} prsc_div_sel_t;

/**
 * PTC series resistor setting. For Mutual cap mode, this series
 * resistor is switched internally on the Y-pin. For Self cap mode,
 * thes series resistor is switched internally on the Sensor pin.
 *
 * Example:
 * RSEL_VAL_0 sets internal series resistor to 0ohms.
 * RSEL_VAL_20 sets internal series resistor to 20Kohms.
 * RSEL_VAL_50 sets internal series resistor to 50Kohms.
 * RSEL_VAL_70 sets internal series resistor to 70Kohms.
 * RSEL_VAL_100 sets internal series resistor to 100Kohms.
 * RSEL_VAL_200 sets internal series resistor to 200Kohms.
 */
typedef enum tag_rsel_val_t {
	RSEL_VAL_0,
	RSEL_VAL_20,
	RSEL_VAL_50,
	RSEL_VAL_70,
	RSEL_VAL_100,
	RSEL_VAL_200
} rsel_val_t;

/**
 * PTC acquisition frequency delay setting.
 *
 * The PTC acquisition frequency is dependent on the Generic clock
 * input to PTC and PTC clock prescaler setting. This delay setting
 * inserts "n" PTC clock cycles between consecutive measurements on
 * a given sensor, thereby changing the PTC acquisition frequency.
 * FREQ_HOP_SEL_1 setting inserts 1 PTC clock cycle between consecutive
 * measurements. FREQ_HOP_SEL_14 setting inserts 14 PTC clock cycles.
 * Hence, higher delay setting will increase the total time taken for
 * capacitance measurement on a given sensor as compared to a lower
 * delay setting.
 *
 * A desired setting can be used to avoid noise around the same frequency
 * as the acquisition frequency.
 *
 */
typedef enum tag_freq_config_sel_t {
	FREQ_SEL_0 = CPU_100nS_COUNT(0),
	FREQ_SEL_1 = CPU_100nS_COUNT(1),
	FREQ_SEL_2 = CPU_100nS_COUNT(2),
	FREQ_SEL_3 = CPU_100nS_COUNT(3),
	FREQ_SEL_4 = CPU_100nS_COUNT(4),
	FREQ_SEL_5 = CPU_100nS_COUNT(5),
	FREQ_SEL_6 = CPU_100nS_COUNT(6),
	FREQ_SEL_7 = CPU_100nS_COUNT(7),
	FREQ_SEL_8 = CPU_100nS_COUNT(8),
	FREQ_SEL_9 = CPU_100nS_COUNT(9),
	FREQ_SEL_10 = CPU_100nS_COUNT(10),
	FREQ_SEL_11 = CPU_100nS_COUNT(11),
	FREQ_SEL_12 = CPU_100nS_COUNT(12),
	FREQ_SEL_13 = CPU_100nS_COUNT(13),
	FREQ_SEL_14 = CPU_100nS_COUNT(14),
	FREQ_SEL_15 = CPU_100nS_COUNT(15),
	FREQ_SEL_SPREAD
} freq_config_sel_t;

/*----------------------------------------------------------------------------
 * Structure Declarations
 *----------------------------------------------------------------------------*/

/* Node configuration
- v1:
- v2: Mega328PB => CSD, Up to 8 X lines, up to 32 Y lines
- v3: Tiny1617 => 8PTC pins (Selectable X or Y), Driven shield
- v4: Q10 => selfcap node with multi-shield
*/
enum { NODE_SHIELD, NODE_SENSOR, NUM_NODE_TYPE };
typedef struct {
	uint8_t nodes[NUM_NODE_TYPE];    /* Selects the sensor pins for node group */
	uint8_t  node_precharge;    /* Precharge time */
	uint8_t  node_csd;          /* Charge acquisition time */
	uint8_t  node_prescale;    /* Prescale */
	uint8_t  node_gain;         /* Bits 3:0 = Digital gain */
	uint8_t  node_oversampling; /* Accumulator setting */
} qtm_acq_node_config_t;

/* Node run-time data - Defined in common api as it will be used with all acquisition modules */

/* Node group configuration */
typedef struct qtm_acq_node_group_config_type {
	uint8_t  num_channel_nodes;    /* Number of channel nodes actually */
	uint8_t  acq_sensor_type;     /* Self or mutual sensors */
	uint8_t  calib_option_select; /* Hardware tuning: XX | TT 3/4/5 Tau | X | XX None/RSEL/PRSC/CSD */
	uint8_t  freq_option_select;  /* SDS or ASDV setting */
} qtm_acq_node_group_config_t;

/* Container structure for sensor group */
typedef struct {
	qtm_acq_node_group_config_t *qtm_acq_node_group_config;
	qtm_acq_node_config_t *  qtm_acq_node_config;
	qtm_acq_node_data_t *        qtm_acq_node_data;
	signal_t *qtm_signal_raw_data;
} qtm_acquisition_control_t;

typedef struct 
{
  qtm_acquisition_control_t* qtm_acq_control;
  union {
  	uint8_t auto_scan_node_number;	/* Evsys sleep node number assigned */
		uint8_t auto_scan_node_mask;	/* Soft sleep node number mask */
  };
  uint8_t auto_scan_node_threshold;
  uint8_t auto_scan_trigger;  
}qtm_auto_scan_config_t;

/* Autoscan signal buffer definition */
enum { QTM_AUTOSCAN_SIGNAL = 0, QTM_AUTOSCAN_DRIFT_TARGET_SIGNAL, NUM_QTM_AUTOSCAN_BUFFER};

/*----------------------------------------------------------------------------
 * prototypes
 *----------------------------------------------------------------------------*/

/* Library prototypes */

/*============================================================================
touch_ret_t qtm_acquisition_process(void)
------------------------------------------------------------------------------
Purpose: Signal capture and processing
Input  : (Measured signals, config)
Output : TOUCH_SUCCESS or TOUCH_CAL_ERROR
Notes  : none
============================================================================*/
touch_ret_t qtm_acquisition_process(void);

/*============================================================================
touch_ret_t qtm_init_acquisition_module(qtm_acquisition_control_t* qtm_acq_control_ptr);
------------------------------------------------------------------------------
Purpose: Initialize the PTC & Assign pins
Input  : pointer to acquisition set
Output : touch_ret_t: TOUCH_SUCCESS or INVALID_PARAM
Notes  : adc_init_acquisition module must be called ONLY once with a pointer to each config set
============================================================================*/
touch_ret_t qtm_init_acquisition_module(qtm_acquisition_control_t *qtm_acq_control_ptr);

/*============================================================================
touch_ret_t qtm_assign_signal_memory(signal_t* qtm_signal_raw_data_ptr)
------------------------------------------------------------------------------
Purpose: Assign raw signals pointer to array defined in application code
Input  : pointer to raw data array, autoscan data array 
Output : touch_ret_t: TOUCH_SUCCESS or INVALID_POINTER
Notes  : none
============================================================================*/
touch_ret_t qtm_assign_signal_memory(signal_t* qtm_signal_raw_data_ptr);

/* Scan configuration */

/*============================================================================
touch_ret_t enable_sensor_node(qtm_acquisition_control_t* qtm_acq_control_ptr, uint16_t qtm_which_node_number)
------------------------------------------------------------------------------
Purpose:  Enables a sensor node for measurement
Input  :  Node configurations pointer, node (channel) number
Output : touch_ret_t:
Notes  :
============================================================================*/
touch_ret_t qtm_enable_sensor_node(qtm_acquisition_control_t *qtm_acq_control_ptr, uint16_t qtm_which_node_number);

/*============================================================================
touch_ret_t calibrate_sensor_node(qtm_acquisition_control_t* qtm_acq_control_ptr, uint16_t qtm_which_node_number)
------------------------------------------------------------------------------
Purpose:  Marks a sensor node for calibration
Input  :  Node configurations pointer, node (channel) number
Output : touch_ret_t:
Notes  :
============================================================================*/
touch_ret_t qtm_calibrate_sensor_node(qtm_acquisition_control_t *qtm_acq_control_ptr, uint16_t qtm_which_node_number);

/* Measurement start - sequence or windowcomp */

/*============================================================================
touch_ret_t qtm_adc_start_measurement_seq(qtm_acquisition_control_t* qtm_acq_control_pointer, void
(*measure_complete_callback) (void));
------------------------------------------------------------------------------
Purpose:  Loads touch configurations for first channel and start,
Input  :  Node configurations pointer, measure complete callback pointer
Output : touch_ret_t:
Notes  :
============================================================================*/
touch_ret_t qtm_adc_start_measurement_seq(qtm_acquisition_control_t *qtm_acq_control_pointer,
                                          void (*measure_complete_callback)(void));

/*============================================================================
touch_ret_t autoscan_sensor_node(qtm_auto_scan_config_t* qtm_auto_scan_config_ptr, void (*auto_scan_callback)(void))
------------------------------------------------------------------------------
Purpose: Configures the PTC for sleep mode measurement of a single node, with window comparator wake
Input  : Acquisition set, channel number, threshold, scan trigger
Output : touch_ret_t
Notes  : none
============================================================================*/
touch_ret_t qtm_autoscan_sensor_node(qtm_auto_scan_config_t *qtm_auto_scan_config_ptr,
                                     void (*auto_scan_callback)(void));

/*============================================================================
touch_ret_t autoscan_node_cancel(void)
------------------------------------------------------------------------------
Purpose: Cancel auto-scan config
Input  : None
Output : touch_ret_t
Notes  : none
============================================================================*/
touch_ret_t qtm_autoscan_node_cancel(void);

/*============================================================================
touch_ret_t qtm_adc_de_init(void)
------------------------------------------------------------------------------
Purpose: Clear PTC Pin registers, set TOUCH_STATE_NULL
Input  : none
Output : the state after deinit
Notes  : none
============================================================================*/
touch_ret_t qtm_adc_de_init(qtm_acquisition_control_t* qtm_acq_control_ptr);

/*============================================================================
uint16_t T1617_ACQ_4P_MODULE_get_id(void);
------------------------------------------------------------------------------
Purpose: Check module ID
Input  :
Output : 16-bit ID for the module
Notes  : none
============================================================================*/
uint16_t qtm_acq_module_get_id(void);

/*============================================================================
uint8_t T1617_ACQ_4P_MODULE_get_version(void);
------------------------------------------------------------------------------
Purpose: Check module verison
Input  :
Output : 8-bit Firmware version (4-bit Major / 4-bit Minor) for the module
Notes  : none
============================================================================*/
uint8_t qtm_acq_module_get_version(void);

/*============================================================================
void qtm_t1617_adc_handler_eoc(void)
------------------------------------------------------------------------------
Purpose:  Captures  the  measurement,  starts  the  next  or  End  Of  Sequence  handler
Input    :  none
Output  :  none
Notes    :  none
============================================================================*/
void qtm_adc_handler_eoc(void);

/*============================================================================
void qtm_t1617_adc_handler_wcomp(void)
------------------------------------------------------------------------------
Purpose:  Captures  the  measurement and calls the user callback
Input    :  none
Output  :  none
Notes    :  none
============================================================================*/
void qtm_adc_handler_wcomp(void);

#endif /* TOUCH_API_PTC_H */
