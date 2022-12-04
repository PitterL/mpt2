/*============================================================================
Filename : freq_hop.c
Project  : Modular Library
Purpose  : Frequency hop operation on selected frequencies, Median filter
------------------------------------------------------------------------------
Version Number : 1.1
Last Updated   : 13/02/2019
Updated By     : Kamal
------------------------------------------------------------------------------
Copyright (c) 2019 Atmel. All rights reserved.
------------------------------------------------------------------------------
Revision 0.1 - Initial build
Revision 0.2 - API Updates
Revision 0.3 - Validation for null pointers
Revision 0.4 - Toolchain Update
Revision 1.0 - Release build
Revision 1.1 - Number of sensor field "num_sensors" is updated to uint16_t in
	"qtm_freq_hop_0x0006_api.h" to support 256 channels
============================================================================*/

/* Include files */
#include "freq_hop.h"

/* Variables */

/* Prototypes*/
static signal_t median_filter(signal_t *filter_buffer, signal_t current_signal,uint8_t current_freq, uint8_t num_freqs );

/*----------------------------------------------------------------------------
  functions
----------------------------------------------------------------------------*/

/*======================================================================================================================
static signal_t median_filter(signal_t *filter_buffer, signal_t current_signal, uint8_t current_freq, uint8_t num_freqs)
------------------------------------------------------------------------------------------------------------------------
Purpose: Returns the median of 3 signals for this channel
Input  : Channel, signal, current frequency, num of freq steps
Output : median signal 
Notes  : This method takes the current and previous cycle signal values based on the configured frequnecy 
         steps and identifies the median value of them.
         The filter buffer is used as ring buffer and the current freq is used as index to store the 
         current signal value. 
======================================================================================================================*/

static signal_t median_filter(signal_t *filter_buffer, signal_t current_signal,uint8_t current_freq, uint8_t num_freqs )
{
  uint8_t i,j,pivot_position;
  uint8_t less_count,equal_count;
  signal_t median=0u;
  
  /* store signal values as ring buffer fashion taking the current freq as the index */
  filter_buffer[current_freq] = current_signal;

  /* Identify the pivot position from the number of frequency steps */
  pivot_position = (num_freqs >> 1u) +1u;                
  
  /* Take each sample value and compare with rest of them to identify its position */
  for(i = 0u; i < num_freqs; i++)
  {
    equal_count = 1u;
    less_count = 0u;
    for(j = 0u; j < num_freqs; j++)
    {
      /* Compare against measurements at other frequencies */
      if(i == j)
      {  
        /* Don't compare to itself */
      }
      else if(filter_buffer[j] == filter_buffer[i])
      {
        equal_count++;
      }
      else if(filter_buffer[j] < filter_buffer[i])
      {
        /* Signal at freq j is smaller than freq i */
        less_count++;
      }
      else
      {
        /* Not a smaller signal */
      }
    }
    
    /* If the current sample position equals to pivot (center position), then return it as median value */
    if( (equal_count >= pivot_position) || ((less_count < pivot_position) && ((less_count + equal_count) >= pivot_position)) )
    {
      /* This sample is the median */
      median = filter_buffer[i];
      
      /* No need to check the others */
      i = num_freqs;
    }
  }
 
  return median;

} /* median_filter(...) */


/* Frequency Mode HOP operation - scans sensor with three different frequencies and the resultant values are passed through median filter*/ 
/*============================================================================
touch_ret_t qtm_freq_hop(qtm_freq_hop_control_t *qtm_freq_hop_control)
------------------------------------------------------------------------------
Purpose: sets three differnet ptc scanning frequencies, processes the ouput signal values with median filter and gives filtered signal value
Input  : Buffer of the selected channel, signal value of the selected channel
Output : median signal (of last 3)
Notes  : 
============================================================================*/
touch_ret_t qtm_freq_hop(qtm_freq_hop_control_t *qtm_freq_hop_control)
{
  uint16_t channel_index,counter=0u;
  signal_t conversion_result;
  uint8_t node_state,current_freq = 0u, num_freqs;

  touch_ret_t touch_ret_fh = TOUCH_SUCCESS;
  if(qtm_freq_hop_control == NULL_POINTER)
  {
    touch_ret_fh = TOUCH_INVALID_POINTER;    
  }
  else
  { 
	  num_freqs = qtm_freq_hop_control->qtm_freq_hop_config->num_freqs;
	  current_freq = qtm_freq_hop_control->qtm_freq_hop_data->current_freq;
	  for(counter=0u; counter < qtm_freq_hop_control->qtm_freq_hop_config->num_sensors; counter++)
	  {
	    node_state = (qtm_freq_hop_control->qtm_freq_hop_data->qtm_acq_node_data[counter].node_acq_status & NODE_STATUS_MASK) >> NODE_STATUS_POS;
    
	    /* Apply Median filter - when sensor not in calibration modes */
	    if(NODE_MEASURE == node_state)
	    {
	      channel_index = (uint16_t)(counter * num_freqs);
      
	      /* Pass starting pointer of the channel channel buffer and the current conversion result to median filter */
	      conversion_result = median_filter(&qtm_freq_hop_control->qtm_freq_hop_data->filter_buffer[channel_index],qtm_freq_hop_control->qtm_freq_hop_data->qtm_acq_node_data[counter].node_acq_signals, current_freq, num_freqs);  
	      qtm_freq_hop_control->qtm_freq_hop_data->qtm_acq_node_data[counter].node_acq_signals = conversion_result;
	    }
	  }
  
	    /* move to next freq when spread option is not selected ( spread option = spread spectrum median operation) */
	    current_freq++;
	    if (current_freq >= num_freqs)
	    {
	      current_freq=0u;
	    }
	    *qtm_freq_hop_control->qtm_freq_hop_config->freq_option_select = qtm_freq_hop_control->qtm_freq_hop_config->median_filter_freq[current_freq];
	    qtm_freq_hop_control->qtm_freq_hop_data->current_freq = current_freq;      
   }
   return touch_ret_fh;
}



/*============================================================================
uint16_t qtm_get_freq_hop_module_id(void)
------------------------------------------------------------------------------
Purpose: Returns the module ID
Input  : none
Output : Module ID
Notes  : none
============================================================================*/
uint16_t qtm_get_freq_hop_module_id(void)
{
  return QTM_MODULE_ID_FREQ_HOP;
}

/*============================================================================
uint8_t qtm_get_freq_hop_module_ver(void)
------------------------------------------------------------------------------
Purpose: Returns the module Firmware version
Input  : none
Output : Module ID - Upper nibble major / Lower nibble minor 
Notes  : none
============================================================================*/
uint8_t qtm_get_freq_hop_module_ver(void)
{
  return QTM_MODULE_VERSION;
}
