/*
 * liblink.h
 *
 * Created: 11/27/2022 5:53:02 PM
 *  Author: A41450
 */ 


#ifndef LIBLINK_H_
#define LIBLINK_H_

#include "../include/touch_api_ptc.h"

typedef uint16_t signal_t;
typedef uint16_t reference_t;
typedef int16_t delta_t;
typedef uint16_t capacitance_t;
typedef uint64_t node_mask_t;
#define MAXIMUM_ADC_CHANNEL_NODES 64

typedef qtm_acq_avr_da_node_config_t qtm_acq_node_config_t;

/* Surface Config */
#if defined(TOUCH_API_SURFACE_CS2T_H)
typedef qtm_surface_cs2t_data_t qtm_surface_csnt_data_t;
typedef qtm_surface_cs2t_control_t qtm_surface_csnt_control_t;
#elif defined(TOUCH_API_SURFACE_CS_H)
typedef qtm_surface_cs_data_t qtm_surface_csnt_data_t;
typedef qtm_surface_cs_control_t qtm_surface_csnt_control_t;
#endif

// Use 1/100 pf as unit
# define __CALCULATE_CAP_DIV_100(_v) ((((_v) >> 2) & 0x02) * 3 + (((_v) >> 4) & 0x0F) * 7 + (((_v) >> 8) & 0x0F) *68 + ((((_v) >> 12) & 0x03) + (((_v) >> 14) & 0x03)) * 675)
#define CALCULATE_CAP(_v) __CALCULATE_CAP_DIV_100(_v)
#define SENSOR_BASE_REF_VALUE 512

/*============================================================================
touch_ret_t qtm_init_acquisition_module(qtm_acquisition_control_t* qtm_acq_control_ptr);
------------------------------------------------------------------------------
Purpose: Initialize the PTC & Assign pins
Input  : pointer to acquisition set
Output : touch_ret_t: TOUCH_SUCCESS or INVALID_PARAM
Notes  : ptc_init_acquisition module must be called ONLY once with a pointer to each config set
============================================================================*/
inline touch_ret_t qtm_init_acquisition_module(qtm_acquisition_control_t *qtm_acq_control_ptr)
{
    return qtm_ptc_init_acquisition_module(qtm_acq_control_ptr);
}

/*============================================================================
touch_ret_t qtm_assign_signal_memory(uint16_t* qtm_signal_raw_data_ptr);
------------------------------------------------------------------------------
Purpose: Assign raw signals pointer to array defined in application code
Input  : pointer to raw data array
Output : touch_ret_t: TOUCH_SUCCESS
Notes  : none
============================================================================*/
inline touch_ret_t qtm_assign_signal_memory(signal_t *qtm_signal_raw_data_ptr)
{
    return qtm_ptc_qtlib_assign_signal_memory((uint16_t *)qtm_signal_raw_data_ptr);
}

/*============================================================================
touch_ret_t qtm_adc_start_measurement_seq(qtm_acquisition_control_t* qtm_acq_control_pointer, void
(*measure_complete_callback) (void));
------------------------------------------------------------------------------
Purpose:  Loads touch configurations for first channel and start,
Input  :  Node configurations pointer, measure complete callback pointer
Output : touch_ret_t:
Notes  :
============================================================================*/
inline touch_ret_t qtm_adc_start_measurement_seq(qtm_acquisition_control_t *qtm_acq_control_pointer,
    void (*measure_complete_callback)(void))
{
    return qtm_ptc_start_measurement_seq(qtm_acq_control_pointer, measure_complete_callback);
}


/*============================================================================
touch_ret_t qtm_adc_de_init(qtm_acquisition_control_t* qtm_acq_control_ptr)
------------------------------------------------------------------------------
Purpose: Clear CVD Pin registers, set TOUCH_STATE_NULL
Input  : none
Output : the lib state after deinit
Notes  : none
============================================================================*/
inline touch_ret_t qtm_adc_de_init(qtm_acquisition_control_t *qtm_acq_control_ptr)
{
   // FIXME: need to be achieved in lib

    return TOUCH_SUCCESS;
}


/*============================================================================
void touch_init_sensor(void)
------------------------------------------------------------------------------
Purpose: Mark the sensor init flag of touch processing
Input  : none
Output : none
Notes  :
============================================================================*/
void touch_init_sensor(void);

/*============================================================================
void touch_calibrate(void)
------------------------------------------------------------------------------
Purpose: Mark the calibration flag of touch processing
Input  : none
Output : none
Notes  :
============================================================================*/
void touch_calibrate(void);

/*============================================================================
USE_MPTT_WRAPPER
void touch_inject_event(void)
------------------------------------------------------------------------------
Purpose: Set a external flag that library could process parameter changed
Input  : None
Output : None
Notes  :
============================================================================*/
void touch_inject_event(void);


/*============================================================================
USE_MPTT_WRAPPER
uint8_t touch_sleep(void)
------------------------------------------------------------------------------
Purpose: Indicate whether the touch is busy now, outside could decide whether enter into sleep mode
Input  : none
Output : sleep_mode_t type
Notes  :
============================================================================*/

uint8_t touch_sleep(void);

/*============================================================================
USE_MPTT_WRAPPER
int8_t touch_state_idle(void)
------------------------------------------------------------------------------
Purpose: Indicate whether the touch is in idle mode
Input  : none
Output : Zero Idle, Other Busy
Notes  :
============================================================================*/
int8_t touch_state_idle(void);

/*============================================================================
USE_MPTT_WRAPPER
int8_t touch_state_sleep(void)
------------------------------------------------------------------------------
Purpose: Indicate whether the touch is in sleep mode
Input  : none
Output : Zero Sleep, Other none sleep
Notes  :
============================================================================*/
int8_t touch_state_sleep(void);

/*============================================================================
USE_MPTT_WRAPPER
void touch_suspend(uint8_t suspend)
------------------------------------------------------------------------------
Purpose: Set a suspend flag that prevent library process scanning
Input  : None zero, enter suspend; otherwise exit suspend
Output : None
Notes  :
============================================================================*/
void touch_suspend(uint8_t suspend);

#endif /* LIBLINK_H_ */