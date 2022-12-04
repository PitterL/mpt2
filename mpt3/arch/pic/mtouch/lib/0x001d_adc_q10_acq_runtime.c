/*============================================================================
Filename : 0x001d_adc_q10_acq_runtime.c
Project : QTouch Modular Library
Purpose : Acquisition module - Q10 and compatible
------------------------------------------------------------------------------
Version Number : 0.1
Last Updated : 4/10/2022
Updated By : Pitter Liao
------------------------------------------------------------------------------
Copyright (c) 2022 Microchip. All rights reserved.
------------------------------------------------------------------------------
Revision 0.1 - Initialize, from `ptc_driver_t1617_4p_runtime.c` v2.1
============================================================================*/

/* Include files */
#include <arch/utils/compiler.h>

/* Header file for internal declarations */
#include "0x001d_adc_q10_acq_runtime.h"

/* Definition */
#define QTM_ACQ_SEQUENTIAL 0u
#define QTM_ACQ_WINDOWCOMP 1u

/* Variables */

/* Container for int cap gain settings */

/* Measurement channel */
uint8_t current_measure_channel = 0u;

/* Library state */
static uint8_t touch_seq_lib_state = TOUCH_STATE_NULL;

/* Acquisition settings pointer */
qtm_acquisition_control_t *qtm_acquisition_control_working_set_ptr;

/* Raw data pointer */
signal_t (*qtm_raw_data_measurements_ptr);

/* Track between sequential and event system measurements */
uint8_t qtm_which_mode_current = QTM_ACQ_SEQUENTIAL;

/*----------------------------------------------------------------------------
Prototypes
----------------------------------------------------------------------------*/
/*============================================================================
static signal_t get_adc_result(void);
------------------------------------------------------------------------------
Purpose:  Captures the CVD result register
Input  :
Output :
Notes  :
============================================================================*/
static signal_t get_adc_result(void);

/*============================================================================
static void adc_polarity_ctrl_set(uint16_t used_adc_pins, uint16_t inverted_adc_pins);
------------------------------------------------------------------------------
Purpose:  Configures the POLARITY bit for all used XY lines
Input  :  USED and INVERTED pins - Bitfield
Output :  -
Notes  :  -
============================================================================*/
static void adc_polarity_ctrl_set(uint16_t used_adc_pins, uint16_t inverted_adc_pins);

/*============================================================================
static void (*adc_seq_measure_complete_pointer) (void);
------------------------------------------------------------------------------
Purpose: Measure complete callback pointer
Input  : Pointer to user callback function
Output : none
Notes  : none
============================================================================*/
static void (*adc_seq_measure_complete_pointer)(void);

/*============================================================================
static void (*adc_wcomp_happened_pointer) (void);
------------------------------------------------------------------------------
Purpose: Window comp callback pointer
Input  : Pointer to user callback function
Output : none
Notes  : none
============================================================================*/
static void (*adc_wcomp_happened_pointer)(void);

/*============================================================================
static uint8_t comp_cap_delta_c_calc(uint16_t* cccal_current_val, uint16_t cal_target_diff, uint8_t delta_pol);
------------------------------------------------------------------------------
Purpose: Calculate actual sensor node capacitance
Input  : Current comp cap setting, signal miss, polarity
Output : The new cap value
Notes  : none
============================================================================*/
static uint8_t comp_cap_delta_c_calc(uint8_t cccal_current_val, uint16_t cal_target_diff, uint8_t delta_pol);

/*============================================================================
static uint8_t select_next_channel_to_measure(void);
------------------------------------------------------------------------------
Purpose: Select the next channel for measuring
Input  : none
Output : current_measure_channel set to next channel number
Notes  : none
============================================================================*/
static uint8_t select_next_channel_to_measure(void);

/*============================================================================
static void qtm_measure_node(uint8_t channel_number);
------------------------------------------------------------------------------
Purpose: Configures the CVD for the selected channel + starts measurement
Input  : channel number
Output : none
Notes  : none
============================================================================*/
static void qtm_measure_node(uint8_t channel_number);

/*============================================================================
static void adc_isr_data_capture(void)
------------------------------------------------------------------------------
Purpose: Captures the measurement, starts the next or End Of Sequence handler
Input  : none
Output : none
Notes  : none
============================================================================*/
static void adc_isr_data_capture(void);

/*============================================================================
static void qtm_invert_io_ctrl(uint8_t which_adc_pin, uint8_t invert_enabled);
------------------------------------------------------------------------------
Purpose: Sets or clears 'INVERT' for the selected CVD pin
Input  : pin number, invert or  not
Output : none
Notes  : none
============================================================================*/
static void qtm_invert_io_ctrl(uint8_t which_adc_pin, uint8_t invert_enabled);

/*============================================================================
static uint16_t qtm_get_x_mask(uint8_t which_node_group);
------------------------------------------------------------------------------
Purpose: Gets a mask of all 4 X configs lines for the group
Input  : Group number
Output : X Line mask
Notes  : none
============================================================================*/
static uint16_t qtm_get_x_mask(uint8_t which_node_group);

/*----------------------------------------------------------------------------
Functions
----------------------------------------------------------------------------*/

/*============================================================================
static uint16_t qtm_adc_init(void);
------------------------------------------------------------------------------
Purpose: Initialize the ADC controller
Input  : None
Output : None
Notes  : none
============================================================================*/
static void qtm_adc_init(void)
{
  // 0xC4: ADGO stop; ADFM right; ADON enabled; ADCONT Disabled; ADCS FOSC/ADCLK;
  // Note the silicon has bugs in non-continous mode, the CNT won't increase, and the ADGO won't clear in average burst mode
  //  PIC18F26/45/46Q10 Silicon A5/B0
  //  PIC18F27/47Q10 Silicon A2
  ADCON0 = /* ADCON0_ADON_ENABLE
    |  */ /* ADCON0_ADCONT_TRIGGER_CONTINOUS |*/ ADCON0_ADCS_CLK_FOSC | ADCON0_ADFM_RIGHT_JUSTIFIED;

  // 0x41: ADDSEN enabled; ADGPOL digital_low; ADIPEN enabled; ADPPOL VDD;
  ADCON1 = ADCON1_ADPPOL_VSS | ADCON1_ADIPEN_INVERT | ADCON1_ADGPOL_LOW | ADCON1_ADDSEN_DOUBLE;
  // 0x8B: ADCRS 0; ADMD Average Burst mode; ADACLR enabled; ADPSIS ADRES;
  ADCON2 = ADCON2_ADPSIS_ADFLTR | ADCON2_ADCRS(0) | ADCON2_ADACLR_CLEAR | ADCON2_ADMD_BURST_AVG_MODE;

  // 0x0F: ADCALC First derivative of Single measurement; ADTMD enabled; ADSOI ADGO is cleared until ADTIF is triggered
  ADCON3 = ADCON3_ADCALC_RESULT_VS_PREV | ADCON3_ADSOI_ADGO_STOP_ON_IRQ | ADCON3_ADTMD_IRQ_ANY;

  // ADACT
  ADACT = ADC_ADACT_DISABLED;
  // ADCCS
  // ADCLK = ADC_ADCLK_ADCS_DIV_N(XTAL_FREQ_MHZ);
  // ADREF 0x0: ADNREF VSS; ADPREF VDD;
  ADREF = ADREF_ADPREF_VDD | ADREF_ADNREF_VSS;
  // ADACQ
  // ADACQ = 200;
  // ADCAP
  // ADCAP = 0;
  // ADPRE
  // ADPRE = 1;
  // ADPCH
  // ADPCH = 0;
  // ADLTHL 0;
  ADLTHL = 0x00;
  // ADLTHH 0;
  ADLTHH = 0x00;
  // ADUTHL 0;
  ADUTHL = 0x00;
  // ADUTHH 0;
  ADUTHH = 0x00;
  // ADSTPTL 0;
  ADSTPTL = 0x00;
  // ADSTPTH 0;
  ADSTPTH = 0x00;
}

/*============================================================================
static uint16_t qtm_get_x_mask(uint8_t which_node_group);
------------------------------------------------------------------------------
Purpose: Gets a mask of all X configs lines for the group
Input  : Group number
Output : X Line mask
Notes  : none
============================================================================*/
static uint16_t qtm_get_x_mask(uint8_t which_node_group)
{
  return 0;
}

/*============================================================================
static void adc_pin_ctrl_set_low(uint8_t node, bool low)
------------------------------------------------------------------------------
Purpose: Sets or clears low level mode for the selected CVD pin
Input  : pin number, set low or not
Output : none
Notes  : none
============================================================================*/
static void adc_pin_ctrl_set_low(uint8_t node, bool low)
{
  uint8_t port, pin, mask;
  volatile uint8_t *RnTRISA;
  volatile uint8_t *RnLATA;

  port = TO_AN_PORT(node);
  pin = TO_AN_PIN(node);
  mask = (uint8_t)(1 << pin);

  // Tri-State Control Register: 1 Port output driver is disabled; 0 Port output driver is enabled
  RnTRISA = &TRISA + port;
  // Output Latch Register
  RnLATA = &LATA + port;

  if (low)
  {
    // set output Low
    *RnTRISA &= ~mask;
    *RnLATA |= mask;
  }
  else
  {
    // set input as OD
    *RnTRISA |= mask;
  }
}

/*============================================================================
static void adc_pin_ctrl_set_ds(uint8_t node, bool ds, PPS_OP_T ppsop)
------------------------------------------------------------------------------
Purpose:  Configures the driven shield for target XY line
Input  :  USED and INVERTED pins - Bitfield
Output :  -
Notes  :  -
============================================================================*/
typedef enum
{
  PPS_UNLOCK,
  PPS_LOCK,
  PPS_NOT,
  NUM_PPS_OP
} PPS_OP_T;
static void adc_pin_ctrl_set_ds(uint8_t node, bool ds, PPS_OP_T ppsop)
{
  volatile uint8_t *RxPPSAddr;

  if (ppsop == PPS_UNLOCK)
  {
    PPSLOCK = PPSLOCK_SEQ0;
    PPSLOCK = PPSLOCK_SEQ1;
    PPSLOCKbits.PPSLOCKED = 0; // unlock PPS
  }

  if (!NODE_UNUSED(node))
  {
    RxPPSAddr = &RA0PPS + node;
    if (ds)
    {
      *RxPPSAddr = RxyOUT_ADGRDA; /* ADCC:ADGRDA */
    }
    else
    {
      *RxPPSAddr = RxyOUT_LATxy;
    }
  }

  if (ppsop == PPS_LOCK)
  {
    PPSLOCK = PPSLOCK_SEQ0;
    PPSLOCK = PPSLOCK_SEQ1;
    PPSLOCKbits.PPSLOCKED = 1; // lock PPS
  }
}

/*============================================================================
touch_ret_t qtm_ds_pin_ctrl_set(qtm_acquisition_control_t* qtm_acq_control_ptr, uint8_t group, bool set)
------------------------------------------------------------------------------
Purpose: Initialize the pin to DS
Input  : @qtm_acq_control_ptr: pointer to acquisition set,
         @group: channel group (if larger than max value, will set all pin),
         @flag: indicate to process sensor node and shield node
Output : touch_ret_t: TOUCH_SUCCESS or INVALID_PARAM
Notes  : adc_init_acquisition module must be called ONLY once with a pointer to each config set
============================================================================*/
#define F_PIN_SET_SHIELD (uint8_t)(1 << 0)
#define F_PIN_SET_SENSOR (uint8_t)(1 << 1)
touch_ret_t qtm_ds_pin_ctrl_set(qtm_acquisition_control_t *qtm_acq_control_ptr, uint8_t group, uint8_t flag)
{
  touch_ret_t touch_return_param = TOUCH_SUCCESS;
  uint8_t i, j, st, end;
  uint8_t node;
  bool set;
  PPS_OP_T ppsop;

  if (qtm_acq_control_ptr == NULL_POINTER)
  {

    touch_return_param = TOUCH_INVALID_POINTER;
  }
  else
  {

    if (group >= qtm_acq_control_ptr->qtm_acq_node_group_config->num_channel_nodes)
    {
      /* all groups need to be set */
      st = 0;
      end = qtm_acq_control_ptr->qtm_acq_node_group_config->num_channel_nodes - 1;
    }
    else
    {
      st = group;
      end = group;
    }

    for (i = st; i <= end; i++)
    {
      for (j = 0; j < NUM_NODE_TYPE; j++)
      {
        node = qtm_acq_control_ptr->qtm_acq_node_config[i].nodes[j];
        if (NODE_UNUSED(node))
        {
          // Invalid node
        }
        else
        {
          set = (j == NODE_SENSOR) ? (flag & F_PIN_SET_SENSOR) : (flag & F_PIN_SET_SHIELD);
          if (qtm_acq_control_ptr->qtm_acq_node_group_config->acq_sensor_type == NODE_SELFCAP)
          {
            adc_pin_ctrl_set_low(node, set);
          }
          else if (qtm_acq_control_ptr->qtm_acq_node_group_config->acq_sensor_type == NODE_SELFCAP_SHIELD)
          {
            if (i == st && j == 0)
            { // first
              ppsop = PPS_UNLOCK;
            }
            else if (i == end && j + 1 == NUM_NODE_TYPE)
            { // last
              ppsop = PPS_LOCK;
            }
            else
            {
              ppsop = PPS_NOT;
            }
            adc_pin_ctrl_set_ds(node, set, ppsop);
          }
          else
          {
            // Other sensing mode unsupported
            touch_return_param = TOUCH_INVALID_INPUT_PARAM;
          }
        }
      }
    }
  }

  return touch_return_param;
}

/**
 * @brief next comp value by adppol set
 *
 * @return int8_t: 1 inc, -1 dec
 */
static int8_t next_comp(void)
{
  return ADCON1bits.ADPPOL ^ ADCON1bits.ADDSEN ? 1 : -1;
}

/*============================================================================
touch_ret_t qtm_acquisition_process(void)
------------------------------------------------------------------------------
Purpose: Signal capture and processing
Input  : (Measured signals, config)
Output : TOUCH_SUCCESS or TOUCH_CAL_ERROR
Notes  : none
============================================================================*/
#ifdef DEBUG_CALIBRATION
#define NUM_DBG_CAL_CHANNELS 5
#define DEPTH_DBG_CAL_STACKS 40
int16_t cache_signal[NUM_DBG_CAL_CHANNELS][DEPTH_DBG_CAL_STACKS];
uint8_t cache_cap[NUM_DBG_CAL_CHANNELS][DEPTH_DBG_CAL_STACKS];
uint8_t tick[NUM_DBG_CAL_CHANNELS];
#endif

touch_ret_t qtm_acquisition_process(void)
{
  touch_ret_t touch_return = TOUCH_SUCCESS;
  /* Called from Result complete */
  uint16_t measured_nodes;
  signal_t measured_signal, measured_signal_old, cal_target;
  delta_t cal_sig_delta, cal_sig_delta_old, cal_sig_delta_diff;
  uint8_t node_state;
  uint8_t scale_down, temp_var_mask;

  uint8_t comp_cap_test_val;

  if (TOUCH_STATE_NULL == touch_seq_lib_state)
  {
    touch_return = TOUCH_INVALID_LIB_STATE;
  }
  else
  {
    for (measured_nodes = 0u; measured_nodes < (qtm_acquisition_control_working_set_ptr->qtm_acq_node_group_config->num_channel_nodes); measured_nodes++)
    {
      measured_signal = (int16_t)(qtm_raw_data_measurements_ptr[measured_nodes]);
      node_state = (qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[measured_nodes].node_acq_status & NODE_STATUS_MASK);

      switch (node_state)
      {
      case (NODE_MEASURE << NODE_STATUS_POS):
      {
        temp_var_mask = NODE_GAIN_DIG(qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[measured_nodes].node_gain);
        scale_down = qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[measured_nodes].node_oversampling;
        if (scale_down > temp_var_mask)
        { // Negative
          scale_down -= temp_var_mask;
        }
        else
        {
          /* Scale down according to digital gain & accumulation settings */
          scale_down = 0;
        }
        measured_signal >>= scale_down;
      }
      break;
      case (NODE_CC_CAL << NODE_STATUS_POS):
      {
        scale_down = qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[measured_nodes].node_oversampling;
        measured_signal >>= scale_down;

        /* last measured value */
        measured_signal_old = (int16_t)qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[measured_nodes].node_acq_signals;

        /* Comp cap settings used for this measurement */
        comp_cap_test_val = (uint8_t)qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[measured_nodes].node_comp_caps;

        /* Calculate Delta-C */
        cal_target = ADCON1bits.ADDSEN ? 0 : ADC_MID_READ;
        cal_sig_delta = (int16_t)(measured_signal - cal_target);
        if (measured_signal_old)
        {
          cal_sig_delta_old = (int16_t)(measured_signal_old - cal_target);
        }
        else
        {
          cal_sig_delta_old = 0;

#ifdef DEBUG_CALIBRATION
          if (measured_nodes == 0)
          {
            memset(tick, 0, sizeof(tick));
            memset(cache_cap, 0, sizeof(cache_cap));
            memset(cache_signal, 0, sizeof(cache_signal));
          }
#endif
        }

#ifdef DEBUG_CALIBRATION
        if (measured_nodes < NUM_DBG_CAL_CHANNELS && tick[measured_nodes] < DEPTH_DBG_CAL_STACKS)
        {
          cache_cap[measured_nodes][tick[measured_nodes]] = comp_cap_test_val;
          cache_signal[measured_nodes][tick[measured_nodes]] = measured_signal;
          tick[measured_nodes]++;
        }
#endif

        cal_sig_delta_diff = (int16_t)(measured_signal - measured_signal_old);

        /* Compare compensation result */
        if (cal_sig_delta_old && cal_sig_delta_diff < CC_CAL_DIFF_PRECISION && cal_sig_delta_diff > -CC_CAL_DIFF_PRECISION)
        {
          /* Diff is little that means calibration end at last round */
          node_state = NODE_ENABLED;
        }
        else if (cal_sig_delta < CC_CAL_PRECISION && cal_sig_delta > -CC_CAL_PRECISION)
        {
          // Best Matched
          node_state = NODE_ENABLED;
        }
        else if (cal_sig_delta_old && (cal_sig_delta ^ cal_sig_delta_old) < 0)
        {
          // Sign is reversed
          if (cal_sig_delta_old < 0)
          {
            if (-cal_sig_delta_old < cal_sig_delta)
            {
              // Using old value
              comp_cap_test_val += next_comp();
            }
          }
          else
          {
            if (cal_sig_delta_old < -cal_sig_delta)
            {
              // Using old value
              comp_cap_test_val -= next_comp();
            }
          }

          node_state = NODE_ENABLED;
        }
        else if (cal_sig_delta > 0)
        {
          /*
          do {
            cal_sig_delta -= CROUGH_BITVAL;
            comp_cap_test_val++;
          }
          while (cal_sig_delta > CROUGH_BITVAL && comp_cap_test_val < CROUGH_MAX_MUTL);
          */
          comp_cap_test_val += next_comp();
        }
        else if (cal_sig_delta < 0)
        {
          /*
            do {
              cal_sig_delta += CROUGH_BITVAL;
              comp_cap_test_val--;
            }
            while (cal_sig_delta < -CROUGH_BITVAL && comp_cap_test_val > 0);
          */
          comp_cap_test_val -= next_comp();
        }

        if ((int8_t)comp_cap_test_val < 0)
        {
          /* minimum cap value */
          comp_cap_test_val = 0;
          node_state = /* NODE_CAL_ERROR| */ NODE_ENABLED;
        }
        else if (comp_cap_test_val >= CROUGH_MAX_MUTL)
        {
          /* Can't calibrate CCC, use Max */
          comp_cap_test_val = CROUGH_MAX_MUTL;
          node_state = NODE_CAL_ERROR | NODE_ENABLED;
        }
        else 
        {

        }

        /* Latest measurement within target ? */
        if (!(node_state & (NODE_CC_CAL << NODE_STATUS_POS)))
        {
          /* Calibration auto-tune of hardware config already completed */
          qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[measured_nodes].node_acq_status = node_state;
          if (NODE_CAL_ERROR == (node_state & NODE_CAL_ERROR))
          {
            /* No Tau cal for NODE_CAL_ERROR */
            touch_return = TOUCH_LIB_NODE_CAL_ERROR; // Overwrite other ret value
          }
          else
          {
            /* No error */
          }
        }
        else
        {
          if (touch_return == TOUCH_SUCCESS)
          {
            touch_return = TOUCH_LIB_NODE_CAL_IN_PROCESS;
          }
        }

        /* Write result to node status array */
        qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[measured_nodes].node_comp_caps = (capacitance_t)(comp_cap_test_val);
      }
      break;
      default:
      {
        /* Shouldn't be here */
      }
      break;
      }
      /* Save measured result */
      qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[measured_nodes].node_acq_signals = (signal_t)measured_signal;
    }
    /* Allow next acquisition start */
    touch_seq_lib_state = TOUCH_STATE_READY;
  } /* Not NULL */

  return touch_return;
}

/*============================================================================
touch_ret_t qtm_init_acquisition_module(qtm_acquisition_control_t* qtm_acq_control_ptr)
------------------------------------------------------------------------------
Purpose: Initialize the CVD & Assign pins
Input  : pointer to acquisition set
Output : touch_ret_t: TOUCH_SUCCESS or INVALID_PARAM
Notes  : adc_init_acquisition module must be called ONLY once with a pointer to each config set
============================================================================*/
touch_ret_t qtm_init_acquisition_module(qtm_acquisition_control_t *qtm_acq_control_ptr)
{
  touch_ret_t touch_return_param = TOUCH_SUCCESS;

  if (qtm_acq_control_ptr == NULL_POINTER)
  {
    touch_return_param = TOUCH_INVALID_POINTER;
  }
  else
  {
    if (touch_seq_lib_state == TOUCH_STATE_NULL)
    {
      /* All nodes configured as low or shield */
      qtm_ds_pin_ctrl_set(qtm_acq_control_ptr, qtm_acq_control_ptr->qtm_acq_node_group_config->num_channel_nodes, F_PIN_SET_SENSOR | F_PIN_SET_SHIELD);

      // initialization for ADCC for CVD mode
      qtm_adc_init();

      touch_seq_lib_state = TOUCH_STATE_INIT;
    }
  }

  return touch_return_param;
} /* adc_init_settings(...) */

/*============================================================================
touch_ret_t qtm_assign_signal_memory(signal_t* qtm_signal_raw_data_ptr)
------------------------------------------------------------------------------
Purpose: Assign raw signals pointer to array defined in application code
Input  : pointer to raw data array
Output : touch_ret_t: TOUCH_SUCCESS or INVALID_POINTER
Notes  : none
============================================================================*/
touch_ret_t qtm_assign_signal_memory(signal_t *qtm_signal_raw_data_ptr)
{
  touch_ret_t touch_return_this = TOUCH_SUCCESS;
  if (qtm_signal_raw_data_ptr == NULL)
  {
    touch_return_this = TOUCH_INVALID_POINTER;
  }
  else
  {
    qtm_raw_data_measurements_ptr = qtm_signal_raw_data_ptr;
  }

  return touch_return_this;
}

/*============================================================================
static signal_t get_adc_result(void)
------------------------------------------------------------------------------
Purpose: Captures CVD RESULT register
Input  : -
Output : RESULT
Notes  : none
============================================================================*/
static signal_t get_adc_result(void)
{
  signal_t touch_measurement_capture = (signal_t)ADACC;

  return touch_measurement_capture;
}

/*============================================================================
touch_ret_t qtm_adc_start_measurement_seq(qtm_acquisition_control_t* qtm_acq_control_pointer, void (*measure_complete_callback) (void))
------------------------------------------------------------------------------
Purpose:  Loads touch configurations for first channel and start,
Input  :  Node configurations pointer, measure complete callback pointer
Output : touch_ret_t:
Notes  :
============================================================================*/
touch_ret_t qtm_adc_start_measurement_seq(qtm_acquisition_control_t *qtm_acq_control_pointer, void (*measure_complete_callback)(void))
{
  touch_ret_t param_ok_status = TOUCH_SUCCESS;
  uint8_t sequence_complete = 0u;

  if (measure_complete_callback == NULL)
  {
    param_ok_status = TOUCH_INVALID_POINTER;
  }
  else if (qtm_acq_control_pointer == NULL)
  {
    param_ok_status = TOUCH_INVALID_POINTER;
  }
  else if (touch_seq_lib_state == TOUCH_STATE_NULL)
  {
    param_ok_status = TOUCH_INVALID_LIB_STATE;
  }
  else if (touch_seq_lib_state == TOUCH_STATE_BUSY)
  {
    param_ok_status = TOUCH_ACQ_INCOMPLETE;
  }
  else
  {
    /* Attach pointers */
    qtm_acquisition_control_working_set_ptr = qtm_acq_control_pointer;
    adc_seq_measure_complete_pointer = measure_complete_callback;
    qtm_which_mode_current = QTM_ACQ_SEQUENTIAL;

    /* IO Pad drivers */

    /* Measurement type */
    if (qtm_acq_control_pointer->qtm_acq_node_group_config->acq_sensor_type == NODE_MUTUAL)
    {
      /* Mutual */
    }
    else if (qtm_acq_control_pointer->qtm_acq_node_group_config->acq_sensor_type == NODE_SELFCAP)
    {
      /* Self / No Shield */
    }
    else if (qtm_acq_control_pointer->qtm_acq_node_group_config->acq_sensor_type == NODE_SELFCAP_SHIELD)
    {
      /* Self / Shield */
    }
    else
    {
      /* Neither Mutual or Self configured - invalid */
      param_ok_status = TOUCH_INVALID_INPUT_PARAM;
    }

    if (param_ok_status == TOUCH_SUCCESS)
    {
      qtm_autoscan_node_cancel();

      /* ISR @ EOC */
      // Clear the ADC Threshold interrupt flag
      PIR1bits.ADTIF = 0;
      // PIR1bits.ADIF = 0;
      // Enabling ADCC threshold interrupt.
      PIE1bits.ADTIE = 1;
      // PIE1bits.ADIE = 1;

      /* Load config for first enabled sensor channel */
      current_measure_channel = 0u;
      sequence_complete = select_next_channel_to_measure();
      if (0u == sequence_complete)
      {
        touch_seq_lib_state = TOUCH_STATE_BUSY;
        qtm_measure_node(current_measure_channel);
      }
      else
      {
        /* The first sensor is invalid */
        param_ok_status = TOUCH_INVALID_INPUT_PARAM;
        touch_seq_lib_state = TOUCH_STATE_READY;
      }
    }
  }

  return param_ok_status;
}

/*============================================================================
touch_ret_t enable_sensor_node(qtm_acquisition_control_t* qtm_acq_control_ptr, uint16_t qtm_which_node_number)
------------------------------------------------------------------------------
Purpose:  Enables a sensor node for measurement
Input  :  Node configurations pointer, node (channel) number
Output : touch_ret_t:
Notes  :
============================================================================*/
touch_ret_t qtm_enable_sensor_node(qtm_acquisition_control_t *qtm_acq_control_ptr, uint16_t qtm_which_node_number)
{
  touch_ret_t node_enable_t_status = TOUCH_SUCCESS;
  if (qtm_acq_control_ptr == NULL)
  {
    /* Not assigned */
    node_enable_t_status = TOUCH_INVALID_POINTER;
  }
  else if (qtm_which_node_number >= (qtm_acq_control_ptr->qtm_acq_node_group_config->num_channel_nodes))
  {
    node_enable_t_status = TOUCH_INVALID_INPUT_PARAM;
  }
  else
  {
    qtm_acq_control_ptr->qtm_acq_node_data[qtm_which_node_number].node_acq_status = NODE_ENABLED;
  }

  return node_enable_t_status;
}

/*============================================================================
touch_ret_t qtm_calibrate_sensor_node(qtm_acquisition_control_t* qtm_acq_control_ptr, uint16_t qtm_which_node_number)
------------------------------------------------------------------------------
Purpose:  Marks a sensor node for calibration
Input  :  Node configurations pointer, node (channel) number
Output : touch_ret_t:
Notes  :
============================================================================*/
touch_ret_t qtm_calibrate_sensor_node(qtm_acquisition_control_t *qtm_acq_control_ptr, uint16_t qtm_which_node_number)
{
  uint8_t c_temp_calc = 0u;
  touch_ret_t node_calibrate_t_status = TOUCH_SUCCESS;

  if (qtm_acq_control_ptr == NULL)
  {
    /* Not assigned */
    node_calibrate_t_status = TOUCH_INVALID_POINTER;
  }
  else if (qtm_which_node_number >= (qtm_acq_control_ptr->qtm_acq_node_group_config->num_channel_nodes))
  {
    node_calibrate_t_status = TOUCH_INVALID_INPUT_PARAM;
  }
  else
  {
    c_temp_calc = qtm_acq_control_ptr->qtm_acq_node_group_config->acq_sensor_type;
    /* Self - to decide starting CCCAL */
    if (NODE_SELFCAP & c_temp_calc)
    {
      /* Initial CC Value */
      qtm_acq_control_ptr->qtm_acq_node_data[qtm_which_node_number].node_comp_caps = SELFCAP_START_CCCAL;
      /* CAL Flag */
      qtm_acq_control_ptr->qtm_acq_node_data[qtm_which_node_number].node_acq_status |= NODE_CAL_REQ;
    }
    else
    {
      node_calibrate_t_status = TOUCH_INVALID_INPUT_PARAM;
    }
  }

  return node_calibrate_t_status;
}

/*============================================================================
static uint8_t select_next_channel_to_measure(void);
------------------------------------------------------------------------------
Purpose: Select the next channel for measuring
Input  : none
Output : current_measure_channel set to next channel number
Notes  : none
============================================================================*/
static uint8_t select_next_channel_to_measure(void)
{
  uint8_t sequence_complete = 2u;
  uint8_t any_enabled_flag;
  uint8_t next_channel;

  next_channel = current_measure_channel;
  do
  {
    if (next_channel < qtm_acquisition_control_working_set_ptr->qtm_acq_node_group_config->num_channel_nodes)
    {
      /* Any group channel enabled? */
      any_enabled_flag = 0u;
      if (0u != (qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[next_channel].node_acq_status & NODE_ENABLED))
      {
        any_enabled_flag = 1u;
      }

      if (0u == any_enabled_flag)
      {
        /* This channel disabled */
        next_channel++;
      }
      else
      {
        /* This channel next */
        sequence_complete = 0u;
        current_measure_channel = next_channel;
      }
    }
    else
    {
      /* No more channels */
      sequence_complete = 1u;
      current_measure_channel = 0u;
    }
  } while (2u == sequence_complete);

  return sequence_complete;
}

/*============================================================================
static void qtm_measure_node(uint8_t channel_number);
------------------------------------------------------------------------------
Purpose: Configures the CVD for the selected channel + starts measurement
Input  : channel number
Output : none
Notes  : none
============================================================================*/
static void qtm_measure_node(uint8_t channel_number)
{
  uint8_t temp_8t_1, temp_8t_2;
  uint8_t cal_in_progress_check;
  uint8_t sensor;

  /* config sensor pin to normal */
  qtm_ds_pin_ctrl_set(qtm_acquisition_control_working_set_ptr, channel_number, F_PIN_SET_SHIELD);

  /* Sensor channel */
  sensor = qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[channel_number].nodes[NODE_SENSOR];
  ADPCH = sensor;

  /* Compensation cap */
  ADCAP = (uint8_t)(qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[channel_number].node_comp_caps & 0xFFu);

  /* Calibration requested? */
  if (0u == (qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[channel_number].node_acq_status & NODE_CAL_REQ))
  {
  }
  else
  {
    /* Calibration requested */
    /* Set node status to calibrate comp cap, clear CAL request flag */
    qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[channel_number].node_acq_status = (uint8_t)((NODE_CC_CAL << NODE_STATUS_POS) | NODE_ENABLED);
    qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[channel_number].node_acq_signals = 0; // Set to zero when calibration begin
  }

  /* CSD */
  ADACQ = qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[channel_number].node_csd;

  temp_8t_2 = qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[channel_number].node_oversampling;
  /* repeat count */
  ADRPT = temp_8t_2;

  
  /* Freq Hop */

  /* Skip Freq Hopping for any ongoing calibration */
  cal_in_progress_check = qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[channel_number].node_acq_status & (NODE_CC_CAL << NODE_STATUS_POS);
  if (cal_in_progress_check == 0) {
    if (qtm_acquisition_control_working_set_ptr->qtm_acq_node_group_config->freq_option_select < FREQ_SEL_SPREAD)
    {
      /* SDS Setting selected */
      temp_8t_1 = (qtm_acquisition_control_working_set_ptr->qtm_acq_node_group_config->freq_option_select); /* Precharge sampling delay */
    }
    else
    {
      /* Spread spectrum selected */
      temp_8t_1 = 0x10u; /* Automatic Sampling Delay Variation (Spread Spectrum) -> Enabled */
    }
  } else {
    temp_8t_1 = 0;
  }

  /* Precharge Delay */
  temp_8t_2 = qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[channel_number].node_precharge;
  if (temp_8t_2 >= 255 - temp_8t_1)
  {
    temp_8t_1 = 255;
  }
  else
  {
    temp_8t_1 += temp_8t_2;
  }
  ADPRE = temp_8t_1;

  /* Clock Prescaler */
  ADCLK = qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[channel_number].node_prescale;

  /* Enable ADC */
  ADCON0bits.ADON = 1;

  // FIXME: Set Sensor Input - TBD

  /* Balance the ADACC */
  // ADACC = (uint16_t)((uint16_t)qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[channel_number].node_oversampling << 9);

  ADCON2bits.ADACLR = 1;
  while (ADCON0bits.ADON && ADCON2bits.ADACLR)
    ;

  /* Start Measurement */
  ADCON0bits.GO = 1;
}

/*============================================================================
touch_ret_t autoscan_sensor_node(qtm_auto_scan_config_t* qtm_auto_scan_config_ptr, void (*auto_scan_callback)(void))
------------------------------------------------------------------------------
Purpose: Configures the CVD for sleep mode measurement of a single node, with window comparator wake
Input  : Acquisition set, channel number, threshold, scan trigger
Output : touch_ret_t
Notes  : none
============================================================================*/
touch_ret_t qtm_autoscan_sensor_node(qtm_auto_scan_config_t *qtm_auto_scan_config_ptr, void (*auto_scan_callback)(void))
{
  touch_ret_t touch_return = TOUCH_SUCCESS;
  uint8_t channel_number, sensor;
  uint8_t temp_8t_1, temp_8t_2;

  if (auto_scan_callback == NULL)
  {
    touch_return = TOUCH_INVALID_POINTER;
  }
  else if (qtm_auto_scan_config_ptr == NULL)
  {
    touch_return = TOUCH_INVALID_POINTER;
  }
  else if (touch_seq_lib_state == TOUCH_STATE_BUSY)
  {
    touch_return = TOUCH_ACQ_INCOMPLETE;
  }
  else if (qtm_auto_scan_config_ptr->auto_scan_node_number >= qtm_auto_scan_config_ptr->qtm_acq_control->qtm_acq_node_group_config->num_channel_nodes)
  {
    touch_return = TOUCH_INVALID_INPUT_PARAM;
  }
  else
  {
    /* Disable ADC */
    ADCON0bits.ADON = 0;

    // Event sleep
    qtm_which_mode_current = QTM_ACQ_WINDOWCOMP;

    /* Callback pointer */
    adc_wcomp_happened_pointer = auto_scan_callback;

    /* Node config */
    current_measure_channel = (uint8_t)qtm_auto_scan_config_ptr->auto_scan_node_number;
    qtm_acquisition_control_working_set_ptr = qtm_auto_scan_config_ptr->qtm_acq_control;

    // FIXME: Should set the windowcomp mode
    channel_number = current_measure_channel;

    /* config sensor pin to normal */
    qtm_ds_pin_ctrl_set(qtm_acquisition_control_working_set_ptr, channel_number, F_PIN_SET_SHIELD);

    /* Sensor channel */
    sensor = qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[channel_number].nodes[NODE_SENSOR];
    ADPCH = sensor;

    /* Compensation cap */
    ADCAP = (uint8_t)(qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[channel_number].node_comp_caps & 0xFFu);

    /* CSD */
    ADACQ = qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[channel_number].node_csd;

    temp_8t_2 = qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[channel_number].node_oversampling;
    /* Not calibrating, use configured settings */
    temp_8t_1 = temp_8t_2 - NODE_GAIN_DIG(qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[channel_number].node_gain);
    if (temp_8t_1 & 0x80)
    { // Negative
      temp_8t_1 = 0;
    }
    else if (temp_8t_1 > ADCON2_ADCRS_MAX_VALUE)
    {
      temp_8t_1 = ADCON2_ADCRS_MAX_VALUE;
    }

    /* repeat count */
    ADRPT = (uint8_t)(1 << temp_8t_2);
    /* Sampling result right shift */
    ADCON2bits.ADCRS = temp_8t_1;

    /* Freq Hop */
    if (qtm_acquisition_control_working_set_ptr->qtm_acq_node_group_config->freq_option_select < FREQ_SEL_SPREAD)
    {
      /* SDS Setting selected */
      temp_8t_1 = (qtm_acquisition_control_working_set_ptr->qtm_acq_node_group_config->freq_option_select); /* Precharge sampling delay */
    }
    else
    {
      /* Spread spectrum selected */
      temp_8t_1 = 0x10u; /* Automatic Sampling Delay Variation (Spread Spectrum) -> Enabled */
    }

    /* Precharge Delay */
    temp_8t_2 = qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[channel_number].node_precharge;
    if (temp_8t_2 >= 255 - temp_8t_1)
    {
      temp_8t_1 = 255;
    }
    else
    {
      temp_8t_1 += temp_8t_2;
    }
    ADPRE = temp_8t_1;

    /* Clock Select ADCRC(FRC) */
    ADCON0bits.ADCS = 1;

    /* Window comparator */
    ADUTH = (register16_t)(qtm_auto_scan_config_ptr->qtm_acq_control->qtm_acq_node_data[qtm_auto_scan_config_ptr->auto_scan_node_number].node_acq_signals + (qtm_auto_scan_config_ptr->auto_scan_node_threshold));
    ADLTH = (register16_t)(qtm_auto_scan_config_ptr->qtm_acq_control->qtm_acq_node_data[qtm_auto_scan_config_ptr->auto_scan_node_number].node_acq_signals - (qtm_auto_scan_config_ptr->auto_scan_node_threshold));
    ADCON3 = ADCON3_ADCALC_RESULT_VS_PREV | ADCON3_ADSOI_ADGO_STOP_ON_IRQ | ADCON3_ADTMD_IRQ_WHEN_ADERR_LESS_THAN_ADLTH_OR_MORE_THAN_ADUTH;

    /* Set Event input */
    ADACT = ADC_ADACT_TMR1;

    /* Enable ADC */
    ADCON0bits.ADON = 1;

    ADCON2bits.ADACLR = 1;
    while (ADCON0bits.ADON && ADCON2bits.ADACLR)
      ;

    /* Start Measurement */
    ADCON0bits.GO = 1;
  }

  /* Allow a sequential call to interrupt WCOMP measurements */
  touch_seq_lib_state = TOUCH_STATE_READY;

  return touch_return;
}

/*============================================================================
touch_ret_t qtm_autoscan_node_cancel(void)
------------------------------------------------------------------------------
Purpose: Cancel auto-scan config
Input  : None
Output : touch_ret_t
Notes  : none
============================================================================*/
touch_ret_t qtm_autoscan_node_cancel(void)
{
  touch_ret_t this_test_return = TOUCH_SUCCESS;

  if (qtm_which_mode_current == QTM_ACQ_WINDOWCOMP)
  {
    /* Cancel Autoscan mode */
    qtm_which_mode_current = QTM_ACQ_SEQUENTIAL;

    /* Disable ADC */
    ADCON0bits.ADON = 0;

    /* Clock Select OSC */
    ADCON0bits.ADCS = 0;

    // 0x0F: ADCALC First derivative of Single measurement; ADTMD enabled; ADSOI ADGO is cleared until ADTIF is triggered
    ADCON3 = ADCON3_ADCALC_RESULT_VS_PREV | ADCON3_ADSOI_ADGO_STOP_ON_IRQ | ADCON3_ADTMD_IRQ_ANY;

    /* Cancel Event input */
    ADACT = ADC_ADACT_DISABLED;

    touch_seq_lib_state = TOUCH_STATE_READY;
  }
  else
  {
    this_test_return = TOUCH_INVALID_LIB_STATE;
  }

  return this_test_return;
}

/*============================================================================
static void adc_isr_data_capture(void)
------------------------------------------------------------------------------
Purpose: Captures the measurement, starts the next or End Of Sequence handler
Input  : none
Output : none
Notes  : none
============================================================================*/
static void adc_isr_data_capture(void)
{
  uint8_t sequence_complete;
  signal_t touch_measurement_result;

  // Reconfig sensor to low or ds
  qtm_ds_pin_ctrl_set(qtm_acquisition_control_working_set_ptr, current_measure_channel, F_PIN_SET_SENSOR);

  // Get ADC result
  touch_measurement_result = get_adc_result();
  qtm_raw_data_measurements_ptr[current_measure_channel] = touch_measurement_result;

  if (ADACT == ADC_ADACT_DISABLED)
  {
    // Normal mode: Measure next channel
    current_measure_channel++;
    sequence_complete = select_next_channel_to_measure();
    if (0u == sequence_complete)
    {
      touch_seq_lib_state = TOUCH_STATE_BUSY;
      qtm_measure_node(current_measure_channel);
    }
    else
    {
      /* Acquisition postprocessing to be called from application */
      adc_seq_measure_complete_pointer();
    }
  }
  else
  {
    // Autoscan mode
    touch_seq_lib_state = TOUCH_STATE_READY;
    adc_wcomp_happened_pointer();
  }
}

/*============================================================================
void qtm_adc_handler_eoc(void)
------------------------------------------------------------------------------
Purpose: Captures the measurement, starts the next or End Of Sequence handler
Input  : none
Output : none
Notes  : none
============================================================================*/
void qtm_adc_handler_eoc(void)
{
  /* Disable ADC */
  ADCON0bits.ADON = 0;

  // Clear the ADC Threshold interrupt flag
  PIR1bits.ADTIF = 0;

  adc_isr_data_capture();
}

/*============================================================================
void qtm_adc_handler_wcomp(void)
------------------------------------------------------------------------------
Purpose: Captures the measurement, starts the next or End Of Sequence handler
Input  : none
Output : none
Notes  : none
============================================================================*/
void qtm_adc_handler_wcomp(void)
{
  adc_isr_data_capture();
}

/*============================================================================
touch_ret_t qtm_adc_de_init(qtm_acquisition_control_t* qtm_acq_control_ptr)
------------------------------------------------------------------------------
Purpose: Clear CVD Pin registers, set TOUCH_STATE_NULL
Input  : none
Output : the lib state after deinit
Notes  : none
============================================================================*/
touch_ret_t qtm_adc_de_init(qtm_acquisition_control_t *qtm_acq_control_ptr)
{
  touch_ret_t touch_return_param = TOUCH_SUCCESS;

  if (touch_seq_lib_state == TOUCH_STATE_NULL)
  {
    touch_return_param = TOUCH_INVALID_POINTER;
  }
  else
  {
    // ADON disabled
    ADCON0 = 0x0;

    qtm_ds_pin_ctrl_set(qtm_acq_control_ptr, qtm_acq_control_ptr->qtm_acq_node_group_config->num_channel_nodes, 0);
    touch_seq_lib_state = TOUCH_STATE_NULL;
  }

  return touch_return_param;
}

/*============================================================================
uint16_t qtm_acq_module_get_id(void)
------------------------------------------------------------------------------
Purpose: Returns the module ID
Input  : none
Output : Module ID
Notes  : none
============================================================================*/
uint16_t qtm_acq_module_get_id(void)
{
  return (uint16_t)(QTM_MODULE_ID);
}

/*============================================================================
uint8_t qtm_acq_module_get_version(void)
------------------------------------------------------------------------------
Purpose: Returns the module Firmware version
Input  : none
Output : Module ID - Upper nibble major / Lower nibble minor
Notes  : none
============================================================================*/
uint8_t qtm_acq_module_get_version(void)
{
  return QTM_MODULE_VERSION;
}
