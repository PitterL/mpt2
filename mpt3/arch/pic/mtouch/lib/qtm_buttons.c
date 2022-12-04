/*============================================================================
Filename : qtm_buttons.c
Project : QTouch Modular Library
Purpose : Touch post-proc
------------------------------------------------------------------------------
Version Number : 1.1
Last Updated   : 01/Feb/2017
Updated By     : Feargal Cleary
------------------------------------------------------------------------------
Copyright (c) 2016 Atmel. All rights reserved.
------------------------------------------------------------------------------
Revision 0.1 - Touch keys post-processing Alpha
Revision 0.2 - Bug fixes -> Beta
Revision 0.3 - API Updates
Revision 0.4 - Validation build with qtm_
Revision 1.0 - Release to Atmel Start
Revision 1.1 - Post-release validation fixes

============================================================================*/

/* Include files */
#include "qtm_buttons.h"

/* Variables */
/* Internal timing management */
uint16_t qtm_local_ms_timecount = 0u;

/*----------------------------------------------------------------------------
Prototypes
----------------------------------------------------------------------------*/


/*============================================================================
static uint8_t check_for_aks_block(qtm_touch_key_control_t* qtm_lib_key_group_ptr, uint16_t which_sensor_key);
------------------------------------------------------------------------------
Purpose: Check whether another key can block >this< one from going into detect
Input  : Pointer to key group control data, key number 
Output : 0 = Not blocked / 1 = Blocked
Notes  : none
============================================================================*/
static uint8_t check_for_aks_block(qtm_touch_key_control_t* qtm_lib_key_group_ptr, uint16_t which_sensor_key);

/*============================================================================
static void qtm_timed_feature_process(qtm_touch_key_control_t* qtm_lib_key_group_ptr);
------------------------------------------------------------------------------
Purpose: Implements drift / recal features based on 200ms timer
Input  : Pointer to key group control data
Output : none
Notes  : none
============================================================================*/
static void qtm_timed_feature_process(qtm_touch_key_control_t* qtm_lib_key_group_ptr);

/*============================================================================
static void qtm_disable_non_reburst_nodes(qtm_touch_key_control_t* qtm_local_lib_key_group_ptr)
------------------------------------------------------------------------------
Purpose: Disable node measurement on those whose condition is resolved
Input  : Pointer to key group control data
Output : (pointer to linked sensor node)
Notes  : none
============================================================================*/
static void qtm_disable_non_reburst_nodes(qtm_touch_key_control_t* qtm_local_lib_key_group_ptr);

/*============================================================================
static void qtm_re_enable_non_reburst_nodes(qtm_touch_key_control_t* qtm_local_lib_key_group_ptr)
------------------------------------------------------------------------------
Purpose: Re-enable node measurement on those disabled for rebursting
Input  : Pointer to key group control data
Output : none
Notes  : none
============================================================================*/
static void qtm_re_enable_non_reburst_nodes(qtm_touch_key_control_t* qtm_local_lib_key_group_ptr);

/*----------------------------------------------------------------------------
functions
----------------------------------------------------------------------------*/

/*============================================================================
void qtm_update_qtlib_timer(uint16_t time_elapsed_since_update)
------------------------------------------------------------------------------
Purpose: Updates local variable with time period
Input  : Number of ms since last update
Output : none
Notes  : none
============================================================================*/
void qtm_update_qtlib_timer(uint16_t time_elapsed_since_update)
{
  /* Allow rollover */
  qtm_local_ms_timecount += time_elapsed_since_update;
}

/*============================================================================
static void qtm_timed_feature_process(qtm_touch_key_control_t* qtm_lib_key_group_ptr);
------------------------------------------------------------------------------
Purpose: Implements drift / recal features based on 200ms timer
Input  : Pointer to key group control data
Output : none
Notes  : none
============================================================================*/
static void qtm_timed_feature_process(qtm_touch_key_control_t* qtm_lib_key_group_ptr)
{
  uint16_t key_counterz, other_key_counterz;
  uint16_t crossed_timebase = 0u;
  uint16_t local_timecount_copy;
  uint8_t this_key_aks_group = 0u;
  
  /* Copy the timestamp, it could be updated in a higher priority ISR */
  local_timecount_copy = qtm_local_ms_timecount;
  
  if(local_timecount_copy > (qtm_lib_key_group_ptr->qtm_touch_key_group_data->acq_group_timestamp + QTLIB_TIMEBASE))
  {
    /* Time period has passed */
    crossed_timebase = local_timecount_copy - qtm_lib_key_group_ptr->qtm_touch_key_group_data->acq_group_timestamp;
  }
  else if(local_timecount_copy < qtm_lib_key_group_ptr->qtm_touch_key_group_data->acq_group_timestamp)
  {
    /* Rollover */
    if(((0xFFFFu - qtm_lib_key_group_ptr->qtm_touch_key_group_data->acq_group_timestamp) + local_timecount_copy) > QTLIB_TIMEBASE)
    {
      /* Time period has passed */
      crossed_timebase = ((0xFFFFu - qtm_lib_key_group_ptr->qtm_touch_key_group_data->acq_group_timestamp) + local_timecount_copy);
    }
  }
  else
  {
    /* Time period has not passed */
  }
  
  /* Update counters + drift if appropriate */
  while(crossed_timebase > QTLIB_TIMEBASE)
  {
    /* Cycle through once per time period */
    crossed_timebase -= QTLIB_TIMEBASE;
    
    /* Update key group timestamp (Allow it to rollover) */
    qtm_lib_key_group_ptr->qtm_touch_key_group_data->acq_group_timestamp += QTLIB_TIMEBASE;
    
    if(0u == qtm_lib_key_group_ptr->qtm_touch_key_group_data->dht_count_in )
    {
      /* Drift hold = 0 -> Decrement drift counters */
      if(0u < qtm_lib_key_group_ptr->qtm_touch_key_group_data->tch_drift_count_in)
      {
        /* Count down */
        qtm_lib_key_group_ptr->qtm_touch_key_group_data->tch_drift_count_in--;
      }
      
      if(0u < qtm_lib_key_group_ptr->qtm_touch_key_group_data->antitch_drift_count_in)
      {
        /* Count down */
        qtm_lib_key_group_ptr->qtm_touch_key_group_data->antitch_drift_count_in--;
      }
      
      /* Towards touch drift */
      if(0u == qtm_lib_key_group_ptr->qtm_touch_key_group_config->sensor_touch_drift_rate)
      {
        /* Towards touch drift disabled */
      }
      else
      {
        if(0u == qtm_lib_key_group_ptr->qtm_touch_key_group_data->tch_drift_count_in)
        {
          for(key_counterz = 0u; key_counterz < qtm_lib_key_group_ptr->qtm_touch_key_group_config->num_key_sensors; key_counterz++)
          {
            /* Keys in NO_DETECT or qtm_KEY_STATE_SUSPEND only - Suspended keys may have been measured during the drift period */
            if((QTM_KEY_STATE_NO_DET == qtm_lib_key_group_ptr->qtm_touch_key_data[key_counterz].sensor_state)||(QTM_KEY_STATE_SUSPEND == qtm_lib_key_group_ptr->qtm_touch_key_data[key_counterz].sensor_state))
            {			   
              if(qtm_lib_key_group_ptr->qtm_touch_key_data[key_counterz].channel_reference < qtm_lib_key_group_ptr->qtm_touch_key_data[key_counterz].node_data_struct_ptr->node_acq_signals)
              {
                qtm_lib_key_group_ptr->qtm_touch_key_data[key_counterz].channel_reference++;
              }
            }
          }        
          /* Reload towards-touch drift time */
          qtm_lib_key_group_ptr->qtm_touch_key_group_data->tch_drift_count_in = qtm_lib_key_group_ptr->qtm_touch_key_group_config->sensor_touch_drift_rate;
        }
      }
      
      
      /* Away from touch drift */
      if(0u == qtm_lib_key_group_ptr->qtm_touch_key_group_config->sensor_anti_touch_drift_rate)
      {
        /* Away from touch drift disabled */
      }
      else 
      {
        if(0u == qtm_lib_key_group_ptr->qtm_touch_key_group_data->antitch_drift_count_in)
        {
          for(key_counterz = 0u; key_counterz < qtm_lib_key_group_ptr->qtm_touch_key_group_config->num_key_sensors; key_counterz++)
          {
            /* Keys in NO_DETECT or qtm_KEY_STATE_SUSPEND only - Suspended keys may have been measured during the drift period */
            if((QTM_KEY_STATE_NO_DET == qtm_lib_key_group_ptr->qtm_touch_key_data[key_counterz].sensor_state)||(QTM_KEY_STATE_SUSPEND == qtm_lib_key_group_ptr->qtm_touch_key_data[key_counterz].sensor_state))
            {
              if(qtm_lib_key_group_ptr->qtm_touch_key_data[key_counterz].channel_reference > qtm_lib_key_group_ptr->qtm_touch_key_data[key_counterz].node_data_struct_ptr->node_acq_signals)
              {
                qtm_lib_key_group_ptr->qtm_touch_key_data[key_counterz].channel_reference--;
              }
            }
          }
          /* Reload anti-touch drift time */
          qtm_lib_key_group_ptr->qtm_touch_key_group_data->antitch_drift_count_in = qtm_lib_key_group_ptr->qtm_touch_key_group_config->sensor_anti_touch_drift_rate;
        }
      }
      
    }
    else
    {
      /* Drift Hold */
      qtm_lib_key_group_ptr->qtm_touch_key_group_data->dht_count_in--;
    }
    
    /* Max On Duration */
    if(0u == qtm_lib_key_group_ptr->qtm_touch_key_group_config->sensor_max_on_time)
    {
      /* MOD Disabled */
    }
    else
    {
      for(key_counterz = 0u; key_counterz < qtm_lib_key_group_ptr->qtm_touch_key_group_config->num_key_sensors; key_counterz++)
      {
        /* Keys in DETECT only */
        if(QTM_KEY_STATE_DETECT == qtm_lib_key_group_ptr->qtm_touch_key_data[key_counterz].sensor_state)
        {
          if(qtm_lib_key_group_ptr->qtm_touch_key_data[key_counterz].sensor_state_counter > 0u)
          {
            qtm_lib_key_group_ptr->qtm_touch_key_data[key_counterz].sensor_state_counter--;
          }
          else
          {
            /* Recalibrate */
            qtm_lib_key_group_ptr->qtm_touch_key_data[key_counterz].sensor_state = QTM_KEY_STATE_INIT;             
            /* Set node to enable + cal */
            qtm_lib_key_group_ptr->qtm_touch_key_data[key_counterz].node_data_struct_ptr->node_acq_status = (uint8_t)(NODE_CAL_REQ | NODE_ENABLED);
            
            /* And all other nodes / keys in the same AKS group */
            this_key_aks_group = (qtm_lib_key_group_ptr->qtm_touch_key_config[key_counterz].channel_aks_group);
            if(0u == this_key_aks_group)
            {
              /* No AKS enabled for this key */
            }
            else
            {							  
              for(other_key_counterz = 0u; other_key_counterz < qtm_lib_key_group_ptr->qtm_touch_key_group_config->num_key_sensors; other_key_counterz++)
              {
                if(this_key_aks_group == (qtm_lib_key_group_ptr->qtm_touch_key_config[other_key_counterz].channel_aks_group))
                {
                  /* Same AKS Group -> Also recalibrate If enabled */
                  if(0u == ((qtm_lib_key_group_ptr->qtm_touch_key_data[other_key_counterz].node_data_struct_ptr->node_acq_status) & NODE_ENABLED))						
                  {
                    /* This node is not enabled */														
                  }
                  else
                  {
                    /* Recalibrate */
                    qtm_lib_key_group_ptr->qtm_touch_key_data[other_key_counterz].sensor_state = QTM_KEY_STATE_INIT;						
                    /* Set node to enable + cal */
                    qtm_lib_key_group_ptr->qtm_touch_key_data[other_key_counterz].node_data_struct_ptr->node_acq_status = (uint8_t)(NODE_CAL_REQ | NODE_ENABLED);						
                  }							
                }					
                else
                {
                  /* Different AKS group */
                }
              }
            }
            
            
          } /* Recalibrate */
        } /* Detect */
      } /* Count through keys */ 
    } /* Max On Not disabled */
  } /* Timebase loops */ 
}

/*============================================================================
static uint8_t check_for_aks_block(qtm_touch_key_control_t* qtm_lib_key_group_ptr, uint16_t which_sensor_key)
------------------------------------------------------------------------------
Purpose: Checks if a key is blocked from DETECT by AKS
Input  : Pointer to key group control, sensor key to check
Output : 0 = not blocked, 1 = blocked
Notes  : none
============================================================================*/
static uint8_t check_for_aks_block(qtm_touch_key_control_t* qtm_lib_key_group_ptr, uint16_t which_sensor_key)
{
  uint16_t count_through_all;
  delta_t surplus_delta_this_key;
  delta_t surplus_delta_check_key;
  uint8_t this_key_aks_group;
  uint8_t this_key_aks_blocked = 0u;
  
  this_key_aks_group = qtm_lib_key_group_ptr->qtm_touch_key_config[which_sensor_key].channel_aks_group;
  surplus_delta_this_key = (qtm_lib_key_group_ptr->qtm_touch_key_data[which_sensor_key].node_data_struct_ptr->node_acq_signals) - (qtm_lib_key_group_ptr->qtm_touch_key_data[which_sensor_key].channel_reference);
  surplus_delta_this_key -= qtm_lib_key_group_ptr->qtm_touch_key_config[which_sensor_key].channel_threshold;
  
  /* Look for other keys which may block this one */
  for(count_through_all = 0u; count_through_all < (qtm_lib_key_group_ptr->qtm_touch_key_group_config->num_key_sensors); count_through_all++)
  {
    if(count_through_all == which_sensor_key)
    {
      /* Don't check against itself */
    }
    else
    {
      if(qtm_lib_key_group_ptr->qtm_touch_key_config[count_through_all].channel_aks_group == this_key_aks_group)
      {
        /* Same AKS Group -> Check if the key is already in detect */
        if((qtm_lib_key_group_ptr->qtm_touch_key_data[count_through_all].sensor_state == QTM_KEY_STATE_DETECT)||(qtm_lib_key_group_ptr->qtm_touch_key_data[count_through_all].sensor_state == QTM_KEY_STATE_FILT_OUT))
        {
          /* Blocked */
          this_key_aks_blocked = 1u;
        }
        /* Otherwise compare (delta-threshold) */
        else if((qtm_lib_key_group_ptr->qtm_touch_key_data[count_through_all].channel_reference) < (qtm_lib_key_group_ptr->qtm_touch_key_data[count_through_all].node_data_struct_ptr->node_acq_signals))
        {
          /* Towards touch delta */
          surplus_delta_check_key = (qtm_lib_key_group_ptr->qtm_touch_key_data[count_through_all].node_data_struct_ptr->node_acq_signals) - (qtm_lib_key_group_ptr->qtm_touch_key_data[count_through_all].channel_reference);
          if(surplus_delta_check_key > qtm_lib_key_group_ptr->qtm_touch_key_config[count_through_all].channel_threshold)
          {
            /* Delta for check key exceeds threshold */
            surplus_delta_check_key -= qtm_lib_key_group_ptr->qtm_touch_key_config[count_through_all].channel_threshold;
            if(surplus_delta_check_key > surplus_delta_this_key)
            {
              /* Blocked */
              this_key_aks_blocked = 1u;
            }            
          }
        }
        else
        {
          /* Not blocked */
        }               
        
        if(0u == this_key_aks_blocked)
        {}
        else
        {
          /* Exit the loop when a blocking key is found */
          break;
        }            
      } /* Same AKS Group */      
    }
  } /* for (...) */
  
  return this_key_aks_blocked;
}

/*============================================================================
touch_ret_t qtm_init_sensor_key(qtm_touch_key_control_t* qtm_lib_key_group_ptr, uint8_t which_sensor_key, qtm_acq_node_data_t* acq_lib_node_ptr)
------------------------------------------------------------------------------
Purpose: Initialize a touch key sensor
Input  : Pointer to key group control data, key number, pointer to acquisition node data structure
Output : TOUCH_SUCCESS
Notes  : none
============================================================================*/
touch_ret_t qtm_init_sensor_key(qtm_touch_key_control_t* qtm_lib_key_group_ptr, uint8_t which_sensor_key, qtm_acq_node_data_t* acq_lib_node_ptr)
{
  touch_ret_t touch_return_thing = TOUCH_SUCCESS;
  
  if((NULL_POINTER == qtm_lib_key_group_ptr)||(NULL_POINTER == acq_lib_node_ptr))
  {
    touch_return_thing = TOUCH_INVALID_POINTER;
  }
  else
  {
    /* Configure key */
    qtm_lib_key_group_ptr->qtm_touch_key_data[which_sensor_key].sensor_state = QTM_KEY_STATE_INIT;
    
    /* Set pointers to node status and signal */
    qtm_lib_key_group_ptr->qtm_touch_key_data[which_sensor_key].node_data_struct_ptr = acq_lib_node_ptr;		
  }
  
  return touch_return_thing;
}

/*============================================================================
touch_ret_t qtm_key_sensors_process(qtm_touch_key_control_t* qtm_lib_key_group_ptr)
------------------------------------------------------------------------------
Purpose: Sensor key post-processing (touch detect state machine)
Input  : Pointer to key group control data
Output : TOUCH_SUCCESS
Notes  : none
============================================================================*/
touch_ret_t qtm_key_sensors_process(qtm_touch_key_control_t* qtm_lib_key_group_ptr)
{
  uint8_t reburst_required = 0u;
  uint8_t keys_status_temp = 0u;
  uint16_t num_sensor_keys;
  delta_t touch_delta;
  uint8_t node_cal_state, aks_blocked;
  uint8_t this_key_current_state;
  
  touch_ret_t touch_return_thing = TOUCH_SUCCESS;
  
  if(NULL_POINTER == qtm_lib_key_group_ptr)
  {
    touch_return_thing = TOUCH_INVALID_POINTER;
  }
  else
  {    
    for(num_sensor_keys = 0u; num_sensor_keys < qtm_lib_key_group_ptr->qtm_touch_key_group_config->num_key_sensors; num_sensor_keys++)
    {
      this_key_current_state = qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state;
      switch (this_key_current_state)
      {
      case (QTM_KEY_STATE_SUSPEND):
      case (QTM_KEY_STATE_DISABLE):
        {
          /* Disabled or suspended key - do nothing */
        }
        break;
      case (QTM_KEY_STATE_INIT):
        {
          /* Wait for completion of hardware cal */
          node_cal_state = qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].node_data_struct_ptr->node_acq_status;
          node_cal_state &= NODE_CAL_MASK;
          if(0u == node_cal_state)
          {
            /* Hardware cal complete, do Key cal - Reference setting */
            qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state = QTM_KEY_STATE_CAL;
            qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter = CAL_SEQ_1_COUNT;
          }
          /* Reburst during hardware cal */
          reburst_required = 1u;
        }
        break;
      case (QTM_KEY_STATE_CAL):
        {
          if(qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter > 0u)
          {                 
            if(qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter > CAL_SEQ_2_COUNT)
            {
              /* Reference -> Signal */
              qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].channel_reference = qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].node_data_struct_ptr->node_acq_signals;
            }
            else
            {          
              if(qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].channel_reference == qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].node_data_struct_ptr->node_acq_signals)
              {
                /* Reference = Signal */
              }
              else if (qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].channel_reference > qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].node_data_struct_ptr->node_acq_signals)
              {
                /* Reference > Signal */
                qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].channel_reference--;
              }          
              else
              {
                /* Reference < Signal */
                qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].channel_reference++;
              }
            }
            qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter--;
            
            /* Reburst during reference cal */
            reburst_required = 1u;
          }
          else
          {
            qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state = QTM_KEY_STATE_NO_DET;
          }
        }
        break;
      case (QTM_KEY_STATE_NO_DET):
        {
          if(qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].channel_reference == qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].node_data_struct_ptr->node_acq_signals)
          {
            /* Reference = Signal -> No Action */
          }
          else if (qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].channel_reference > qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].node_data_struct_ptr->node_acq_signals)
          {
            /* Reference > Signal -> Anti-touch? */
            touch_delta = (qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].channel_reference - qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].node_data_struct_ptr->node_acq_signals);
            if(touch_delta < (uint8_t)((qtm_lib_key_group_ptr->qtm_touch_key_config[num_sensor_keys].channel_threshold) >> qtm_lib_key_group_ptr->qtm_touch_key_group_config->sensor_anti_touch_recal_thr))
            {
              /* Delta is less than anti-touch threshold */
            }
            else
            {
              /* Delta is greater than anti-touch threhsold */
              if(0u == (qtm_lib_key_group_ptr->qtm_touch_key_group_config->sensor_anti_touch_di))
              {
                /* Anti-touch recalibration disabled */
              }
              else
              {              
                qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state = QTM_KEY_STATE_ANTI_TCH;
                qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter = qtm_lib_key_group_ptr->qtm_touch_key_group_config->sensor_anti_touch_di;
              }
            }            
          }          
          else
          {
            /* Reference < Signal -> Touch? */
            touch_delta = (qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].node_data_struct_ptr->node_acq_signals - qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].channel_reference);
            if(touch_delta < (uint8_t)(qtm_lib_key_group_ptr->qtm_touch_key_config[num_sensor_keys].channel_threshold))
            {
              /* Delta is less than touch threshold */
            }
            else
            {
              /* Delta is greater than touch threhsold */
              /* AKS Block ? - Any key in same AKS group either In Detect or (Delta - Threshold) > (This Delta - This Threshold) */
              if(0u == qtm_lib_key_group_ptr->qtm_touch_key_config[num_sensor_keys].channel_aks_group)
              {
                aks_blocked = 0u;
              }
              else
              {
                aks_blocked = check_for_aks_block(qtm_lib_key_group_ptr, num_sensor_keys);
              }            
              if(0u == aks_blocked)            
              {
                /* This key is not blocked from detection by AKS */
                qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state = QTM_KEY_STATE_FILT_IN;
                qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter = qtm_lib_key_group_ptr->qtm_touch_key_group_config->sensor_touch_di;
              }
            }
          }
        }
        break;
      case (QTM_KEY_STATE_FILT_IN):
        {
          if((qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].channel_reference) > (qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].node_data_struct_ptr->node_acq_signals))
          {
            /* Signal < Reference -> Return to No Detect */
            qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state = QTM_KEY_STATE_NO_DET;
            qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter = 0u;
          }
          else          
          {
            touch_delta = (qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].node_data_struct_ptr->node_acq_signals) - (qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].channel_reference);
            if(touch_delta > qtm_lib_key_group_ptr->qtm_touch_key_config[num_sensor_keys].channel_threshold)
            {
              if(0u == qtm_lib_key_group_ptr->qtm_touch_key_config[num_sensor_keys].channel_aks_group)
              {
                /* This key not enabled for AKS (Grp = 0) */
                aks_blocked = 0u;
              }
              else
              {
                /* This key enabled for AKS, check for competing keys */
                aks_blocked = check_for_aks_block(qtm_lib_key_group_ptr, num_sensor_keys);
              }            
              if(0u == aks_blocked)            
              {
                /* This key is not blocked from detection by AKS */
                if(qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter > 0u)
                {
                  /* Count into detect */
                  qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter--;
                  /* Reburst during Filter In */
                  reburst_required = 1u;
                }
                else
                {
                  qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state = QTM_KEY_STATE_DETECT;
                  
                  keys_status_temp = QTM_KEY_DETECT;
                  
                  /* Reload MAX_ON_DURATION */
                  qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter = qtm_lib_key_group_ptr->qtm_touch_key_group_config->sensor_max_on_time;
                }
              }
            } 
            else
            {
              /* (Sig - Ref) < Threshold -> Back to NO_DETECT */
              qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state = QTM_KEY_STATE_NO_DET;
              qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter = 0u;
            }
          }
        }
        break;
      case (QTM_KEY_STATE_DETECT):
        {
          /* Status Flag */
          keys_status_temp = QTM_KEY_DETECT;
          
          /* Drift Hold */
          qtm_lib_key_group_ptr->qtm_touch_key_group_data->dht_count_in = qtm_lib_key_group_ptr->qtm_touch_key_group_config->sensor_drift_hold_time;
          
          if((qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].channel_reference) > (qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].node_data_struct_ptr->node_acq_signals))
          {
            /* Signal < Reference -> Filter Out */
            qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state = QTM_KEY_STATE_FILT_OUT;
            /* Reburst during Filter Out */
            reburst_required = 1u;
            qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter = qtm_lib_key_group_ptr->qtm_touch_key_group_config->sensor_touch_di;          
          }
          else          
          {
            touch_delta = (qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].node_data_struct_ptr->node_acq_signals) - (qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].channel_reference);
            
            /* Variable re-use node_cal_state & aks_blocked for threshold - hysteresis */      
            aks_blocked = (qtm_lib_key_group_ptr->qtm_touch_key_config[num_sensor_keys].channel_threshold);
            
            /* Increment hysteresis to get bit-shift as setting of 0 should be 50% hysteresis */
            node_cal_state = (uint8_t)(aks_blocked >> ((qtm_lib_key_group_ptr->qtm_touch_key_config[num_sensor_keys].channel_hysteresis)+1u));          
            aks_blocked -= node_cal_state;
            
            /* Check delta vs (Threshold - Hysteresis) */
            if(touch_delta > aks_blocked)
            {
              /* Still in detect */
            }
            else
            {
              qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state = QTM_KEY_STATE_FILT_OUT;
              /* Reburst during Filter Out */
              reburst_required = 1u;
              qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter = qtm_lib_key_group_ptr->qtm_touch_key_group_config->sensor_touch_di;
            }          
          }
        }
        break;
      case (QTM_KEY_STATE_FILT_OUT):
        {
          /* Status Flag */
          keys_status_temp = QTM_KEY_DETECT;
          
          /* Count out of detect */
          if((qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].channel_reference) > (qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].node_data_struct_ptr->node_acq_signals))
          {
            /* Signal < Reference -> Return to No Detect */
            if(0u == qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter)
            {
              /* Finished count-out -> go to no detect */
              qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state = QTM_KEY_STATE_NO_DET;
            }
            else
            {
              qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter--;
            }        
          }
          else
          {
            touch_delta = (qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].node_data_struct_ptr->node_acq_signals) - (qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].channel_reference);
            
            /* Variable re-use node_cal_state & aks_blocked for threshold - hysteresis */      
            aks_blocked = (qtm_lib_key_group_ptr->qtm_touch_key_config[num_sensor_keys].channel_threshold);
            
            /* Increment hysteresis to get bit-shift as setting of 0 should be 50% hysteresis */
            node_cal_state = (uint8_t)(aks_blocked >> ((qtm_lib_key_group_ptr->qtm_touch_key_config[num_sensor_keys].channel_hysteresis)+1u));          
            aks_blocked -= node_cal_state;
            
            /* Check delta vs (Threshold - Hysteresis) */
            if(touch_delta > aks_blocked)
            {
              /* Back to 'Detect' */
              qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state = QTM_KEY_STATE_DETECT;
              
              /* Reload MAX_ON_DURATION */
              qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter = qtm_lib_key_group_ptr->qtm_touch_key_group_config->sensor_max_on_time;            
            }
            else
            {
              if(0u == qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter)
              {
                /* Finished count-out -> go to no detect */
                qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state = QTM_KEY_STATE_NO_DET;
              }
              else
              {
                qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter--;
                /* Reburst during Filter Out */
                reburst_required = 1u;
              }
            }/* /else */
          } /* /else */
        }
        break;      
      case (QTM_KEY_STATE_ANTI_TCH):
        {
          if((qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].channel_reference) > (qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].node_data_struct_ptr->node_acq_signals))
          {
            touch_delta = (qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].channel_reference - qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].node_data_struct_ptr->node_acq_signals);
            if(touch_delta < (uint8_t)((qtm_lib_key_group_ptr->qtm_touch_key_config[num_sensor_keys].channel_threshold) >> qtm_lib_key_group_ptr->qtm_touch_key_group_config->sensor_anti_touch_recal_thr))
            {
              /* Delta is less than anti-touch threshold */
              qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state = QTM_KEY_STATE_NO_DET;
              qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter = 0u;
            }                   
            /* Signal < Reference -> Still in anti-touch, count into recal */
            else if(qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter == 0u)
            {
              /* Recalibrate sensor node, then key */
              qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state = QTM_KEY_STATE_INIT;
              qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].node_data_struct_ptr->node_acq_status = (uint8_t)(NODE_CAL_REQ | NODE_ENABLED);
              
            }
            else
            {
              qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter--;
              /* Reburst during Anti-touch */
              reburst_required = 1u;
            }          
          }
          else
          {
            qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state = QTM_KEY_STATE_NO_DET;
            qtm_lib_key_group_ptr->qtm_touch_key_data[num_sensor_keys].sensor_state_counter = 0u;
          }
          
        }
        break;
      default:
        {
          
        }
        break;           
      } /* Switch */
    }  
    
    if(0u == (qtm_lib_key_group_ptr->qtm_touch_key_group_data->qtm_keys_status & QTM_KEY_REBURST))
    {
      /* This was not a reburst */
    }
    else
    {
      /* This was a reburst - restore disabled (non reburst) nodes */
      qtm_re_enable_non_reburst_nodes(qtm_lib_key_group_ptr);
      qtm_lib_key_group_ptr->qtm_touch_key_group_data->qtm_keys_status &= (uint8_t)(~QTM_KEY_REBURST);
    }
    
    
    if(0u == reburst_required)
    {
    }    
    else
    {
      /* Reburst according to config */
      if(REBURST_NONE == qtm_lib_key_group_ptr->qtm_touch_key_group_config->sensor_reburst_mode)
      {
        /* No reburst - application timed measurements only */
      }
      else 
      {
        /* Flag lib group for REBURST */
        qtm_lib_key_group_ptr->qtm_touch_key_group_data->qtm_keys_status |= QTM_KEY_REBURST;
        if(REBURST_UNRESOLVED == qtm_lib_key_group_ptr->qtm_touch_key_group_config->sensor_reburst_mode)
        {
          /* Disable nodes for keys which shouldn't be reburst */
          qtm_disable_non_reburst_nodes(qtm_lib_key_group_ptr);
        }
      }    
    }  
    
    if(0u == keys_status_temp)
    {
      /* No key in detect */
      qtm_lib_key_group_ptr->qtm_touch_key_group_data->qtm_keys_status &= (uint8_t)~(QTM_KEY_DETECT);
    }
    else
    {
      /* Some key in detect */
      qtm_lib_key_group_ptr->qtm_touch_key_group_data->qtm_keys_status |= keys_status_temp;
    }
    qtm_timed_feature_process(qtm_lib_key_group_ptr);
  }
  
  return touch_return_thing;  
}

/*============================================================================
static void qtm_disable_non_reburst_nodes(qtm_touch_key_control_t* qtm_local_lib_key_group_ptr)
------------------------------------------------------------------------------
Purpose: Disable node measurement on those whose condition is resolved
Input  : Pointer to key group control data
Output : (pointer to linked sensor node)
Notes  : none
============================================================================*/
static void qtm_disable_non_reburst_nodes(qtm_touch_key_control_t* qtm_local_lib_key_group_ptr)
{
  uint8_t key_filter_req;
  uint16_t knd_counter;
  uint16_t other_k_cntr;
  
  for(knd_counter = 0u; knd_counter < qtm_local_lib_key_group_ptr->qtm_touch_key_group_config->num_key_sensors; knd_counter++)
  {      
    /* Suspended or disabled */
    if((QTM_KEY_STATE_DISABLE == qtm_local_lib_key_group_ptr->qtm_touch_key_data[knd_counter].sensor_state)||(QTM_KEY_STATE_SUSPEND == qtm_local_lib_key_group_ptr->qtm_touch_key_data[knd_counter].sensor_state))
    {
      key_filter_req = 0u;
    }
    /* Detect or no detect */
    else if((QTM_KEY_STATE_DETECT == qtm_local_lib_key_group_ptr->qtm_touch_key_data[knd_counter].sensor_state)||(QTM_KEY_STATE_NO_DET == qtm_local_lib_key_group_ptr->qtm_touch_key_data[knd_counter].sensor_state))
    {
      key_filter_req = 0u;
      
      /* Only nodes in DETECT or NO DETECT may not be included in the reburst */      
      if(0u == qtm_local_lib_key_group_ptr->qtm_touch_key_config[knd_counter].channel_aks_group)
      {
        /* If AKS is disabled then this key cannot block another and may be disabled */
      }
      else
      {
        /* Don't disable this key if another with same AKS group is in FILTER state. */
        for(other_k_cntr = 0u; other_k_cntr < qtm_local_lib_key_group_ptr->qtm_touch_key_group_config->num_key_sensors; other_k_cntr++)
        {
          if(other_k_cntr == knd_counter)
          {
            /* Don't check against itself */
          }
          else
          {
            if((QTM_KEY_STATE_FILT_IN == qtm_local_lib_key_group_ptr->qtm_touch_key_data[other_k_cntr].sensor_state)||(QTM_KEY_STATE_FILT_OUT == qtm_local_lib_key_group_ptr->qtm_touch_key_data[other_k_cntr].sensor_state))
            {
              if(qtm_local_lib_key_group_ptr->qtm_touch_key_config[knd_counter].channel_aks_group == qtm_local_lib_key_group_ptr->qtm_touch_key_config[other_k_cntr].channel_aks_group)
              {
                /* Don't disable key - may require measurements for AKS */
                key_filter_req = 1u;
                
                /* Exit loop */
                other_k_cntr = qtm_local_lib_key_group_ptr->qtm_touch_key_group_config->num_key_sensors;
              } /* In same AKS group */
            } /* Another key in Filter state */
          } /* Not itself */
        } /* for(other_k_cntr...) */
      } /* AKS Enabled */
    } /* Key in DETECT or NO_DETECT */
    else
    {
      /* Other states - reburst sensor */
      key_filter_req = 1u;
    }
    
    if(0u == key_filter_req)
    {
      /* Disable node measurement */
      qtm_local_lib_key_group_ptr->qtm_touch_key_data[knd_counter].node_data_struct_ptr->node_acq_status &= (uint8_t)(~NODE_ENABLED);
    } /* This key not in the same group as a FILTER key */
    else
    {
      /* Ensure node is enabled unless suspended or disabled */
      if((QTM_KEY_STATE_DISABLE == qtm_local_lib_key_group_ptr->qtm_touch_key_data[knd_counter].sensor_state)||(QTM_KEY_STATE_SUSPEND == qtm_local_lib_key_group_ptr->qtm_touch_key_data[knd_counter].sensor_state))
      {
      }
      else
      {
        qtm_local_lib_key_group_ptr->qtm_touch_key_data[knd_counter].node_data_struct_ptr->node_acq_status |= NODE_ENABLED;
      }            
    }    
  } /* for(knd_counter...) */
}

/*============================================================================
static void qtm_re_enable_non_reburst_nodes(qtm_touch_key_control_t* qtm_local_lib_key_group_ptr)
------------------------------------------------------------------------------
Purpose: Re-enable node measurement on those disabled for rebursting
Input  : Pointer to key group control data
Output : none
Notes  : none
============================================================================*/
static void qtm_re_enable_non_reburst_nodes(qtm_touch_key_control_t* qtm_local_lib_key_group_ptr)
{
  uint16_t knd_counter;
  
  for(knd_counter = 0u; knd_counter < qtm_local_lib_key_group_ptr->qtm_touch_key_group_config->num_key_sensors; knd_counter++)
  {
    if(0u == (qtm_local_lib_key_group_ptr->qtm_touch_key_data[knd_counter].node_data_struct_ptr->node_acq_status & NODE_ENABLED))
    {
      /* Node measurement disabled */
      if((QTM_KEY_STATE_DISABLE == qtm_local_lib_key_group_ptr->qtm_touch_key_data[knd_counter].sensor_state)||(QTM_KEY_STATE_SUSPEND == qtm_local_lib_key_group_ptr->qtm_touch_key_data[knd_counter].sensor_state))
      {
        /* Node should not be enabled */
      }
      else
      {
        /* Re-enable node */        
        qtm_local_lib_key_group_ptr->qtm_touch_key_data[knd_counter].node_data_struct_ptr->node_acq_status |= NODE_ENABLED;
      }
    }
  }
}
/*============================================================================
touch_ret_t qtm_key_suspend(uint16_t which_sensor_key, qtm_touch_key_control_t* qtm_lib_key_group_ptr)
------------------------------------------------------------------------------
Purpose: Suspends acquisition measurements for the key
Input  : Key number, Pointer to key group control data
Output : TOUCH_SUCCESS
Notes  : none
============================================================================*/
touch_ret_t qtm_key_suspend(uint16_t which_sensor_key, qtm_touch_key_control_t* qtm_lib_key_group_ptr)
{
  touch_ret_t touch_return_stat;
  if(NULL_POINTER == qtm_lib_key_group_ptr)
  {
    touch_return_stat = TOUCH_INVALID_POINTER;
  }
  else if(which_sensor_key < qtm_lib_key_group_ptr->qtm_touch_key_group_config->num_key_sensors)
  {
    /* Key status -> SUSPEND */
    qtm_lib_key_group_ptr->qtm_touch_key_data[which_sensor_key].sensor_state = QTM_KEY_STATE_SUSPEND;
    
    /* Disable node */    
    qtm_lib_key_group_ptr->qtm_touch_key_data[which_sensor_key].node_data_struct_ptr->node_acq_status &= (uint8_t)(~NODE_ENABLED);
    
    touch_return_stat = TOUCH_SUCCESS;
  }
  else
  {
    touch_return_stat = TOUCH_INVALID_INPUT_PARAM;
  }
  
  return touch_return_stat;
}

/*============================================================================
touch_ret_t qtm_key_resume(uint16_t which_sensor_key, qtm_touch_key_control_t* qtm_lib_key_group_ptr)
------------------------------------------------------------------------------
Purpose: Resumes acquisition measurements for the key
Input  : Key number, Pointer to key group control data
Output : TOUCH_SUCCESS
Notes  : none
============================================================================*/
touch_ret_t qtm_key_resume(uint16_t which_sensor_key, qtm_touch_key_control_t* qtm_lib_key_group_ptr)
{
  touch_ret_t touch_return_stat;
  if(NULL_POINTER == qtm_lib_key_group_ptr)
  {
    touch_return_stat = TOUCH_INVALID_POINTER;
  }
  
  else if(which_sensor_key < qtm_lib_key_group_ptr->qtm_touch_key_group_config->num_key_sensors)
  {
    /* Key status -> SUSPEND */
    qtm_lib_key_group_ptr->qtm_touch_key_data[which_sensor_key].sensor_state = QTM_KEY_STATE_NO_DET;
    
    /* Re-enable node */
    qtm_lib_key_group_ptr->qtm_touch_key_data[which_sensor_key].node_data_struct_ptr->node_acq_status |= (uint8_t)(NODE_ENABLED);
    
    touch_return_stat = TOUCH_SUCCESS;
  }
  else
  {
    touch_return_stat = TOUCH_INVALID_INPUT_PARAM;
  }
  
  return touch_return_stat;  
}


/*============================================================================
uint16_t qtm_get_touch_keys_module_id(void)
------------------------------------------------------------------------------
Purpose: Returns the module ID
Input  : none
Output : Module ID
Notes  : none
============================================================================*/
uint16_t qtm_get_touch_keys_module_id(void)
{
  return QTM_MODULE_ID_KEYS;
}

/*============================================================================
uint8_t qtm_get_touch_keys_module_ver(void)
------------------------------------------------------------------------------
Purpose: Returns the module Firmware version
Input  : none
Output : Module ID - Upper nibble major / Lower nibble minor 
Notes  : none
============================================================================*/
uint8_t qtm_get_touch_keys_module_ver(void)
{
  return QTM_MODULE_VERSION;
}

