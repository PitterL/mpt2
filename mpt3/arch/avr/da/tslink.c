/*
 * trans.c
 *
 * Created: 6/14/2019 10:14:51 PM
 *  Author: A41450
 */

#include <common/include/types.h>

#include "arch/cpu.h"
#include "arch/fuse.h"
#include "arch/tslink.h"
#include "arch/timer.h"
#include "arch/pinconf.h"
#include "qtouch/lib/tslib.h"
#include "objects/txx.h"

/* Definition */
#ifdef OBJECT_T15
#ifndef TOUCH_API_BUTTON
#error "Defined OBJECT T15 But no Button"
#endif
#endif

#ifdef OBJECT_T9
#if !(defined(TOUCH_API_SURFACE) || defined(TOUCH_API_SCROLLER))
#error "Defined OBJECT T9 But no surface"
#endif
#endif

#ifdef OBJECT_T126
#ifndef TOUCH_API_AUTOSCAN
#error "Defined OBJECT T126 But no Autoscan"
#endif
#endif

/*----------------------------------------------------------------------------
 *   prototypes
 *----------------------------------------------------------------------------*/
#ifdef OBJECT_T8
static u8 sensing_mode_to_tslib_translate(u8 mode);
static u8 sensing_mode_to_object_translate(u8 mode);
#endif

enum
{
  COMMON_RW,
  ARRAY_MEM_RW,
  // BIT_RW,
  NUM_RW_OP
};

typedef struct tch_config_callback
{
  u8 type;
  void *buf;
  u8 size; // For common_write, this is data size, for array_member_write, this is struct size
  u8 trunk_size;
} tch_config_callback_t;

static const tch_config_callback_t touch_config_list[] = {
#ifdef API_DEF_TOUCH_MEASUREMENT_PERIOD_MS
    {API_DEF_TOUCH_MEASUREMENT_PERIOD_MS, &GET_MEASUREMENT_PERIOD_ACTIVE(), sizeof(GET_MEASUREMENT_PERIOD_ACTIVE()), 0},
#endif
#ifdef API_DEF_TOUCH_MEASUREMENT_IDLE_PERIOD_MS
    {API_DEF_TOUCH_MEASUREMENT_IDLE_PERIOD_MS, &GET_MEASUREMENT_PERIOD_IDLE(), sizeof(GET_MEASUREMENT_PERIOD_IDLE()), 0},
#endif
#ifdef API_DEF_TOUCH_ACTIVE_IDLE_TIMEOUT
    {API_DEF_TOUCH_ACTIVE_IDLE_TIMEOUT, &GET_MEASUREMENT_PERIOD_ACTIVE_TO_IDLE(), sizeof(GET_MEASUREMENT_PERIOD_ACTIVE_TO_IDLE()), 0},
#endif
#ifdef API_DEF_SENSOR_TYPE
    {API_DEF_SENSOR_TYPE, &GET_ACQ_NODE_GRP_CFG_i(acq_sensor_type), sizeof(GET_ACQ_NODE_GRP_CFG_i(acq_sensor_type)), 0},
#endif
#ifdef API_NUM_CHANNELS
    {API_NUM_CHANNELS, &GET_ACQ_NODE_GRP_CFG_i(num_channel_nodes), sizeof(GET_ACQ_NODE_GRP_CFG_i(num_channel_nodes)), 0},
#endif
#ifdef API_NODE_PARAMS_PRECHARGE
    {API_NODE_PARAMS_PRECHARGE, &GET_SEQ_NODE_CFG_ni(0, node_precharge), sizeof(GET_SEQ_NODE_CFG_ni(0, node_precharge)), sizeof(GET_SEQ_NODE_CFG_n(0))},
#endif
#ifdef API_NODE_PARAMS_CSD
    {API_NODE_PARAMS_CSD, &GET_SEQ_NODE_CFG_ni(0, node_csd), sizeof(GET_SEQ_NODE_CFG_ni(0, node_csd)), sizeof(GET_SEQ_NODE_CFG_n(0))},
#endif
#ifdef API_NODE_PARAMS_RESISTOR_PRESCALER
    {API_NODE_PARAMS_RESISTOR_PRESCALER, &GET_SEQ_NODE_CFG_ni(0, node_rsel_prsc), sizeof(GET_SEQ_NODE_CFG_ni(0, node_rsel_prsc)), sizeof(GET_SEQ_NODE_CFG_n(0))},
#endif
#ifdef API_NODE_PARAMS_GAIN
    {API_NODE_PARAMS_GAIN, &GET_SEQ_NODE_CFG_ni(0, node_gain), sizeof(GET_SEQ_NODE_CFG_ni(0, node_gain)), sizeof(GET_SEQ_NODE_CFG_n(0))},
#endif
#ifdef API_NODE_PARAMS_ADC_OVERSAMPLING
    {API_NODE_PARAMS_ADC_OVERSAMPLING, &GET_SEQ_NODE_CFG_ni(0, node_oversampling), sizeof(GET_SEQ_NODE_CFG_ni(0, node_oversampling)), sizeof(GET_SEQ_NODE_CFG_n(0))},
#endif
#ifdef API_KEY_PARAMS_THRESHOLD
    {API_KEY_PARAMS_THRESHOLD, &GET_KEY_CFG_ni(0, channel_threshold), sizeof(GET_KEY_CFG_ni(0, channel_threshold)), sizeof(GET_KEY_CFG_n(0))},
#endif
#ifdef API_KEY_PARAMS_HYSTERESIS
    {API_KEY_PARAMS_HYSTERESIS, &GET_KEY_CFG_ni(0, channel_hysteresis), sizeof(GET_KEY_CFG_ni(0, channel_hysteresis)), sizeof(GET_KEY_CFG_n(0))},
#endif
#ifdef API_KEY_PARAMS_AKS_GROUP
    {API_KEY_PARAMS_AKS_GROUP, &GET_KEY_CFG_ni(0, channel_aks_group), sizeof(GET_KEY_CFG_ni(0, channel_aks_group)), sizeof(GET_KEY_CFG_n(0))},
#endif
#ifdef API_NUM_SENSORS
    {API_NUM_SENSORS, &GET_KEY_GRP_CFG_SET_i(num_key_sensors), sizeof(GET_KEY_GRP_CFG_SET_i(num_key_sensors)), 0},
#endif
#ifdef API_DEF_TOUCH_DET_INT
    {API_DEF_TOUCH_DET_INT, &GET_KEY_GRP_CFG_SET_i(sensor_touch_di), sizeof(GET_KEY_GRP_CFG_SET_i(sensor_touch_di)), 0},
#endif
#ifdef API_DEF_MAX_ON_DURATION
    {API_DEF_MAX_ON_DURATION, &GET_KEY_GRP_CFG_SET_i(sensor_max_on_time), sizeof(GET_KEY_GRP_CFG_SET_i(sensor_max_on_time)), 0},
#endif
#ifdef API_DEF_ANTI_TCH_DET_INT
    {API_DEF_ANTI_TCH_DET_INT, &GET_KEY_GRP_CFG_SET_i(sensor_anti_touch_di), sizeof(GET_KEY_GRP_CFG_SET_i(sensor_anti_touch_di)), 0},
#endif
#ifdef API_DEF_ANTI_TCH_RECAL_THRSHLD
    {API_DEF_ANTI_TCH_RECAL_THRSHLD, &GET_KEY_GRP_CFG_SET_i(sensor_anti_touch_recal_thr), sizeof(GET_KEY_GRP_CFG_SET_i(sensor_anti_touch_recal_thr)), 0},
#endif
#ifdef API_DEF_TCH_DRIFT_RATE
    {API_DEF_TCH_DRIFT_RATE, &GET_KEY_GRP_CFG_SET_i(sensor_touch_drift_rate), sizeof(GET_KEY_GRP_CFG_SET_i(sensor_touch_drift_rate)), 0},
#endif
#ifdef API_DEF_ANTI_TCH_DRIFT_RATE
    {API_DEF_ANTI_TCH_DRIFT_RATE, &GET_KEY_GRP_CFG_SET_i(sensor_anti_touch_drift_rate), sizeof(GET_KEY_GRP_CFG_SET_i(sensor_anti_touch_drift_rate)), 0},
#endif
#ifdef API_DEF_DRIFT_HOLD_TIME
    {API_DEF_DRIFT_HOLD_TIME, &GET_KEY_GRP_CFG_SET_i(sensor_drift_hold_time), sizeof(GET_KEY_GRP_CFG_SET_i(sensor_drift_hold_time)), 0},
#endif
#ifdef API_DEF_QTM_AUTOSCAN_DRIFT_PERIOD_MS
    {API_DEF_QTM_AUTOSCAN_DRIFT_PERIOD_MS, &GET_MEASUREMENT_PERIOD_IDLE_DRIFT(), sizeof(GET_MEASUREMENT_PERIOD_IDLE_DRIFT()), 0},
#endif
#ifdef API_DEF_QTM_AUTOSCAN_THRESHOLD
    {API_DEF_QTM_AUTOSCAN_THRESHOLD, &GET_AUTO_SCAN_SETUP_i(auto_scan_node_threshold), sizeof(GET_AUTO_SCAN_SETUP_i(auto_scan_node_threshold)), 0},
#endif
#ifdef API_DEF_QTM_AUTOSCAN_NODE
    {API_DEF_QTM_AUTOSCAN_NODE, &GET_AUTO_SCAN_SETUP_i(auto_scan_node_number), sizeof(GET_AUTO_SCAN_SETUP_i(auto_scan_node_number)), 0},
#endif
#ifdef API_DEF_QTM_AUTOSCAN_DRIFT_TARGET_NODE
    {API_DEF_QTM_AUTOSCAN_DRIFT_TARGET_NODE, &GET_AUTO_SCAN_SETUP_i(auto_scan_drift_target_node), sizeof(GET_AUTO_SCAN_SETUP_i(auto_scan_drift_target_node)), 0},
#endif
#ifdef API_DEF_QTM_AUTOSCAN_TUNING_PARAM
    {API_DEF_QTM_AUTOSCAN_TUNING_PARAM, &GET_AUTO_SCAN_SETUP_i(autoscan_comp_caps), sizeof(GET_AUTO_SCAN_SETUP_i(autoscan_comp_caps)), 0},
#endif
#ifdef API_DEF_QTM_AUTOSCAN_CSD
    {API_DEF_QTM_AUTOSCAN_CSD, &ptc_seq_auto_scan_node_cfg[0].node_csd, sizeof(ptc_seq_auto_scan_node_cfg[0].node_csd), sizeof(ptc_seq_auto_scan_node_cfg[0])},
#endif
#ifdef API_DEF_QTM_AUTOSCAN_RESISTOR_PRESCALER
    {API_DEF_QTM_AUTOSCAN_RESISTOR_PRESCALER, &ptc_seq_auto_scan_node_cfg[0].node_rsel_prsc, sizeof(ptc_seq_auto_scan_node_cfg[0].node_rsel_prsc), sizeof(ptc_seq_auto_scan_node_cfg[0])},
#endif
#ifdef API_DEF_QTM_AUTOSCAN_GAIN
    {API_DEF_QTM_AUTOSCAN_GAIN, &ptc_seq_auto_scan_node_cfg[0].node_gain, sizeof(ptc_seq_auto_scan_node_cfg[0].node_gain), sizeof(ptc_seq_auto_scan_node_cfg[0])},
#endif
#ifdef API_DEF_QTM_AUTOSCAN_ADC_OVERSAMPLING
    {API_DEF_QTM_AUTOSCAN_ADC_OVERSAMPLING, &ptc_seq_auto_scan_node_cfg[0].node_oversampling, sizeof(ptc_seq_auto_scan_node_cfg[0].node_oversampling), sizeof(ptc_seq_auto_scan_node_cfg[0])},
#endif
#ifdef API_NODE_COMPCAP_VALUE
    {API_NODE_COMPCAP_VALUE, &GET_NODE_STAT_ni(0, node_comp_caps), sizeof(GET_NODE_STAT_ni(0, node_comp_caps)), sizeof(GET_NODE_STAT_n(0))},
#endif
#ifdef API_NUM_SLIDERS
    {API_NUM_SLIDERS, &GET_SCROLLER_GRP_CFG_i(num_scrollers), sizeof(GET_SCROLLER_GRP_CFG_i(num_scrollers)), 0},
#endif
#ifdef API_SLIDER_START_KEY
    {API_SLIDER_START_KEY, &GET_SCROLLER_CFG_ni(0, start_key), sizeof(GET_SCROLLER_CFG_ni(0, start_key)), sizeof(GET_SCROLLER_CFG_n(0))},
#endif
#ifdef API_SLIDER_NUM_KEYS
    {API_SLIDER_NUM_KEYS, &GET_SCROLLER_CFG_ni(0, number_of_keys), sizeof(GET_SCROLLER_CFG_ni(0, number_of_keys)), sizeof(GET_SCROLLER_CFG_n(0))},
#endif
#ifdef API_SLIDER_POS_HYST
    {API_SLIDER_POS_HYST, &GET_SCROLLER_CFG_ni(0, position_hysteresis), sizeof(GET_SCROLLER_CFG_ni(0, position_hysteresis)), sizeof(GET_SCROLLER_CFG_n(0))},
#endif
#ifdef API_SLIDER_MIN_CONTACT
    {API_SLIDER_MIN_CONTACT, &GET_SCROLLER_CFG_ni(0, contact_min_threshold), sizeof(GET_SCROLLER_CFG_ni(0, contact_min_threshold)), sizeof(GET_SCROLLER_CFG_n(0))},
#endif
#ifdef API_SLIDER_RESOL_DB
    {API_SLIDER_RESOL_DB, &GET_SCROLLER_CFG_ni(0, resol_deadband), sizeof(GET_SCROLLER_CFG_ni(0, resol_deadband)), sizeof(GET_SCROLLER_CFG_n(0))},
#endif
#ifdef API_SURFACE_CS_START_KEY_V
    {API_SURFACE_CS_START_KEY_V, &GET_SURFACE_CS_CFG_i(start_key_v), sizeof(GET_SURFACE_CS_CFG_i(start_key_v)), 0},
#endif
#ifdef API_SURFACE_CS_START_KEY_H
    {API_SURFACE_CS_START_KEY_H, &GET_SURFACE_CS_CFG_i(start_key_h), sizeof(GET_SURFACE_CS_CFG_i(start_key_h)), 0},
#endif
#ifdef API_SURFACE_CS_NUM_KEYS_V
    {API_SURFACE_CS_NUM_KEYS_V, &GET_SURFACE_CS_CFG_i(number_of_keys_v), sizeof(GET_SURFACE_CS_CFG_i(number_of_keys_v)), 0},
#endif
#ifdef API_SURFACE_CS_NUM_KEYS_H
    {API_SURFACE_CS_NUM_KEYS_H, &GET_SURFACE_CS_CFG_i(number_of_keys_h), sizeof(GET_SURFACE_CS_CFG_i(number_of_keys_h)), 0},
#endif
#ifdef API_SURFACE_CS_POS_HYST
    {API_SURFACE_CS_POS_HYST, &GET_SURFACE_CS_CFG_i(position_hysteresis), sizeof(GET_SURFACE_CS_CFG_i(position_hysteresis)), 0},
#endif
#ifdef API_SURFACE_CS_FILT_CFG
    {API_SURFACE_CS_FILT_CFG, &GET_SURFACE_CS_CFG_i(position_filter), sizeof(GET_SURFACE_CS_CFG_i(position_filter)), 0},
#endif
#ifdef API_SURFACE_CS_MIN_CONTACT
    {API_SURFACE_CS_MIN_CONTACT, &GET_SURFACE_CS_CFG_i(contact_min_threshold), sizeof(GET_SURFACE_CS_CFG_i(contact_min_threshold)), 0},
#endif
#ifdef API_SURFACE_CS_RESOL_DB
    {API_SURFACE_CS_RESOL_DB, &GET_SURFACE_CS_CFG_i(resol_deadband), sizeof(GET_SURFACE_CS_CFG_i(resol_deadband)), 0}
#endif
};

#define TCH_CONFIG_WRITEBACK_NUM ARRAY_SIZE(touch_config_list)

static void trigger_calibration(u8 type, u8 index)
{
  switch (type)
  {
#ifdef API_DEF_SENSOR_TYPE
  case API_DEF_SENSOR_TYPE:
#endif
#ifdef API_NODE_PARAMS_CSD
  case API_NODE_PARAMS_CSD:
#endif
#ifdef API_NODE_PARAMS_PRECHARGE
  case API_NODE_PARAMS_PRECHARGE:
#endif
#ifdef API_NODE_PARAMS_RESISTOR_PRESCALER
  case API_NODE_PARAMS_RESISTOR_PRESCALER:
#endif
#ifdef API_NODE_PARAMS_GAIN
  case API_NODE_PARAMS_GAIN:
#endif
#ifdef API_NODE_PARAMS_ADC_OVERSAMPLING
  case API_NODE_PARAMS_ADC_OVERSAMPLING:
#endif
#ifdef API_DEF_QTM_AUTOSCAN_DRIFT_TARGET_NODE
  case API_DEF_QTM_AUTOSCAN_DRIFT_TARGET_NODE:
#endif
#ifdef API_DEF_QTM_AUTOSCAN_GAIN
  case API_DEF_QTM_AUTOSCAN_GAIN:
#endif
    tslink_calibrate();
    break;
#ifdef API_KEY_PARAMS_THRESHOLD
  case API_KEY_PARAMS_THRESHOLD:
#endif
#ifdef API_KEY_PARAMS_HYSTERESIS
  case API_KEY_PARAMS_HYSTERESIS:
#endif
#ifdef API_KEY_PARAMS_AKS_GROUP
  case API_KEY_PARAMS_AKS_GROUP:
#endif
#ifdef API_DEF_TOUCH_DET_INT
  case API_DEF_TOUCH_DET_INT:
#endif
#ifdef API_DEF_MAX_ON_DURATION
  case API_DEF_MAX_ON_DURATION:
#endif
#ifdef API_DEF_ANTI_TCH_DET_INT
  case API_DEF_ANTI_TCH_DET_INT:
#endif
#ifdef API_DEF_ANTI_TCH_RECAL_THRSHLD
  case API_DEF_ANTI_TCH_RECAL_THRSHLD:
#endif
#ifdef API_DEF_TCH_DRIFT_RATE
  case API_DEF_TCH_DRIFT_RATE:
#endif
#ifdef API_DEF_ANTI_TCH_DRIFT_RATE
  case API_DEF_ANTI_TCH_DRIFT_RATE:
#endif
#ifdef API_DEF_DRIFT_HOLD_TIME
  case API_DEF_DRIFT_HOLD_TIME:
#endif
    touch_init_sensor();
    break;
#ifdef TOUCH_API_SCROLLER
#ifdef API_SLIDER_START_KEY
  case API_SLIDER_START_KEY:
#endif
#ifdef API_SLIDER_NUM_KEYS
  case API_SLIDER_NUM_KEYS:
#endif
#ifdef API_SLIDER_RESOL_DB
  case API_SLIDER_RESOL_DB:
#endif
    // case API_SLIDER_FILT_CFG:
#ifdef API_SLIDER_POS_HYST
  case API_SLIDER_POS_HYST:
#endif
#ifdef API_SLIDER_MIN_CONTACT
  case API_SLIDER_MIN_CONTACT:
#endif
    tslib_init_sliders();
    break;
#endif /* TOUCH_API_SCROLLER */
#ifdef TOUCH_API_SURFACE
#ifdef API_SURFACE_CS_START_KEY_H
  case API_SURFACE_CS_START_KEY_H:
#endif
#ifdef API_SURFACE_CS_NUM_KEYS_H
  case API_SURFACE_CS_NUM_KEYS_H:
#endif
#ifdef API_SURFACE_CS_START_KEY_V
  case API_SURFACE_CS_START_KEY_V:
#endif
#ifdef API_SURFACE_CS_NUM_KEYS_V
  case API_SURFACE_CS_NUM_KEYS_V:
#endif
#ifdef API_SURFACE_CS_RESOL_DB
  case API_SURFACE_CS_RESOL_DB:
#endif
#ifdef API_SURFACE_CS_FILT_CFG
  case API_SURFACE_CS_FILT_CFG:
#endif
#ifdef API_SURFACE_CS_POS_HYST
  case API_SURFACE_CS_POS_HYST:
#endif
#ifdef API_SURFACE_CS_MIN_CONTACT
  case API_SURFACE_CS_MIN_CONTACT:
#endif
    tslib_init_surfaces();
    break;
#endif /* TOUCH_API_SURFACE */
  default:;
  }
}

static ssint common_read_write(void *dst, const void *src, u8 size)
{
  if (memcmp(dst, src, size))
  {
    memcpy(dst, src, size);
    return 0;
  }

  return -2;
}

static ssint tch_config_rw(const tch_config_callback_t *param, void *buf, u8 size, u8 index, u8 rw)
{
  void *dst, *src;
  u8 trunk_size = 0;
  ssint result;

  // Not overlap, but could change part of the varible
  if (size > param->size)
  {
    size = param->size;
  }

  trunk_size = param->trunk_size ? param->trunk_size * index : 0;

  if (rw == OP_READ)
  { // read: 1, write: 0
    dst = buf;
    src = param->buf + trunk_size;
  }
  else
  {
    dst = param->buf + trunk_size;
    src = buf;
  }

  result = common_read_write(dst, src, size);
  if (result == 0)
  {
    if (rw == OP_WRITE)
    {
      trigger_calibration(param->type, index);
    }
  }

  return 0;
}

u8 config_cache_ptr[2];
u8 *pre_op(u8 type, void *buf, u8 size, u8 index, u8 rw)
{
  if (size < sizeof(u8))
    return NULL;

  if (rw == OP_WRITE)
  {
    if (0)
    {
#ifdef API_DEF_SENSOR_TYPE
    }
    else if (type == API_DEF_SENSOR_TYPE)
    {
      config_cache_ptr[0] = sensing_mode_to_tslib_translate(*(u8 *)buf);
      return config_cache_ptr;
#endif
#ifdef API_SURFACE_CS_FILT_CFG
    }
    else if (type == API_SURFACE_CS_FILT_CFG)
    {
      nibble_t *nb = (nibble_t *)&config_cache_ptr[0];
      nb->hi = !(((nibble_t *)buf)->hi & 0x8);
      nb->lo = ((nibble_t *)buf)->hi & 0x7;
      return config_cache_ptr;
#endif
#ifdef API_SURFACE_CS_MIN_CONTACT
    }
    else if (type == API_SURFACE_CS_MIN_CONTACT)
    {
      *(u16 *)config_cache_ptr = *(u8 *)buf;
      return config_cache_ptr;
#endif
    }
    /* OP_WRITE */
  }
  else if (rw == OP_READ)
  {
    if (0
#ifdef API_DEF_SENSOR_TYPE
        || type == API_DEF_SENSOR_TYPE
#endif
#ifdef API_SURFACE_CS_FILT_CFG
        || type == API_SURFACE_CS_FILT_CFG
#endif
#ifdef API_SURFACE_CS_MIN_CONTACT
        || type == API_SURFACE_CS_MIN_CONTACT
#endif
    )
    {
      return config_cache_ptr;
    }
    /* OP_READ */
  }
  else
  {
    /* OP_CMD */
  }

  return buf;
}

ssint post_op(u8 type, void *buf, void *cache, u8 size, u8 index, u8 rw, ssint result)
{
  if (size < sizeof(u8))
    return result;

  if (rw == OP_READ)
  {
    if (0)
    {
#ifdef API_DEF_SENSOR_TYPE
    }
    else if (type == API_DEF_SENSOR_TYPE)
    {
      *(u8 *)buf = sensing_mode_to_object_translate(*(u8 *)cache);

      return 0;
#endif
#ifdef API_SURFACE_CS_FILT_CFG
    }
    else if (type == API_SURFACE_CS_FILT_CFG)
    {
      nibble_t *nb = (nibble_t *)cache;
      ((nibble_t *)buf)->hi = (nb->lo & 0x7) | (nb->hi ? 0 : 0x8);
      ((nibble_t *)buf)->lo = 0;

      return 0;
#endif
#ifdef API_SURFACE_CS_MIN_CONTACT
    }
    else if (type == API_SURFACE_CS_MIN_CONTACT)
    {
      *(u8 *)buf = *(u8 *)cache;

      return 0;
#endif
    }
    /* OP_READ */
  }
  else if (rw == OP_WRITE)
  {
    /* OP_WRITE */
  }
  else
  {
    /* OP_CMD */
    if (0)
    {
#ifdef API_CHANNEL_BASE_REF
    }
    else if (type == API_CHANNEL_BASE_REF)
    {
      if (size == sizeof(u16))
      {
        *(u16 *)buf = SENSOR_BASE_REF_VALUE * (GET_SEQ_NODE_CFG_ni(index, node_gain) & 0xF) /*NODE_GAIN_DIG*/;
        return 0;
      }
#endif
#ifdef API_SELFTEST
    }
    else if (type == API_SELFTEST)
    {
      if (index == REQUEST_ADC_RESOURCE)
      {
        if (buf && size == sizeof(bool))
        {
          return api_request_adc_resource(*(bool *)buf);
        }
      }
      else if (index == AVDD_CMD_RUN)
      {
        return api_avdd_test(buf);
      }
      else if (index == PINFAULT_CMD_SET_PARAM)
      {
        return api_pinfault_set_param(buf, size);
      }
      else if (index == PINFAULT_CMD_RUN)
      {
        return api_pinfault_test(buf, size);
      }
#endif
    }
  }

  return result;
}

/**
 * @brief operating the object buffer with target touch varibles
 * 
 * @param type : the operation api code 
 * @param buf : source data
 * @param size : source size
 * @param index : target node channel
 * @param rw : Read or Write
 * @return ssint : return code
 */
ssint tslink_config_op(u8 type, void *buf, u8 size, u8 index, u8 rw)
{
  const tch_config_callback_t *tccs = &touch_config_list[0];
  u8 i, *new_buf = NULL;
  ssint result = -1;

  // Check index overlfow
  if (index >= GET_ACQ_NODE_GRP_CFG_i(num_channel_nodes))
  {
    return result;
  }

  if (rw != OP_CMD)
  {
    new_buf = pre_op(type, buf, size, index, rw);

    if (new_buf)
    {
      for (i = 0; i < TCH_CONFIG_WRITEBACK_NUM; i++)
      {
        if (tccs[i].type == type)
        {
          result = tch_config_rw(&tccs[i], new_buf, size, index, rw);
          break;
        }
      }
    }
  }

  return post_op(type, buf, new_buf, size, index, rw, result);
}

u8 tslink_read_group_config_byte(u8 type, u8 index)
{
  u8 buf[1];
  ssint result;

  result = tslink_config_op(type, buf, sizeof(buf), index, OP_READ);
  if (result == 0)
    return buf[0];

  return (u8)-1; // Default return 0xff
}

u8 tslink_read_config_byte(u8 type)
{
  return tslink_read_group_config_byte(type, 0);
}

void tslink_calibrate(void)
{
  /* CC Calibrate */
  touch_calibrate();

  /* Software calibration */
  touch_init_sensor();
}

u8 tslink_get_chip_state(void)
{
  u8 state = 0;
#ifdef OBJECT_T6
  u8 sensor_state, cal;
  u8 i, node;

  for (i = 0; i < (u8)GET_KEY_GRP_CFG_SET_i(num_key_sensors); i++)
  {
    // FIXME, Why node_acq_status may different with sensor_state?
    node = tslib_get_sensor_key_mapping(i, false);
    cal = (u8)(GET_NODE_STAT_ni(node, node_acq_status) & NODE_CAL_MASK);
    if (cal)
    {
      state = MXT_T6_STATUS_CAL;
    }
    else
    {
      sensor_state = (u8)(GET_KEY_DATA_SET_ni(i, sensor_state) & ~KEY_TOUCHED_MASK);
      switch (sensor_state)
      {
      case QTM_KEY_STATE_DISABLE:
        state = MXT_T6_STATUS_RESET;
        break;
      case QTM_KEY_STATE_CAL:
      case QTM_KEY_STATE_CAL_ERR:
        state = MXT_T6_STATUS_CAL;
        break;
      default:;
      }
    }

    if (state)
      break;
  }
#endif
  return state;
}

u8 tslink_get_number_channel_nodes(void)
{
  // normal we call tslink_read_config_byte(API_NUM_CHANNELS), but we need fast access here

  // Fast read
  return (u8)GET_ACQ_NODE_GRP_CFG_i(num_channel_nodes);
}

u8 tslink_get_number_key_sensors(void)
{
  // normal we call tslink_read_config_byte(API_NUM_SENSORS), but we need fast access here

  // Fast read
  return (u8)GET_KEY_GRP_CFG_SET_i(num_key_sensors);
}

#ifdef TOUCH_API_SCROLLER
u8 tslink_get_number_slider_sensors(void)
{
  // normal we call tslink_read_config_byte(API_NUM_SLIDERS), but we need fast access here

  // Fast read
  return (u8)GET_SCROLLER_GRP_CFG_i(num_scrollers);
}
#endif

#if defined(OBJECT_T25) || defined(OBJECT_T37)
u16 calculate_and_cache_cccap(u8 key, u16 comcap)
{
  return CALCULATE_CAP(comcap);
}
#endif

ssint tslink_read_ref_signal_cap(u8 key, /*cap_sample_value_t **/ void *cv)
{
  cap_sample_value_t *cval = (cap_sample_value_t *)cv;

  if (key >= (u8)GET_KEY_GRP_CFG_SET_i(num_key_sensors))
    return -2;

  cval->reference = tslib_get_sensor_key_reference(key);
  cval->signal = tslib_get_sensor_key_signal(key);
  cval->comcap = tslib_get_sensor_ccomp_val(key);
#if defined(OBJECT_T25) || defined(OBJECT_T37)
  cval->cccap = calculate_and_cache_cccap(key, cval->comcap);
#endif

  return 0;
}

ssint tslink_read_button_state(u8 key)
{
  u8 status;

  if (key >= (u8)GET_KEY_GRP_CFG_SET_i(num_key_sensors))
    return -2;

  if ((GET_KEY_DATA_SET_ni(key, sensor_state)) & KEY_TOUCHED_MASK)
  {
    status = 1;
  }
  else
  {
    status = 0;
  }

  return (ssint)status;
}

#ifdef TOUCH_API_SCROLLER
ssint tslink_read_slider_state(u8 index, /*t9_point_status_t */ void *sts)
{
  const qtm_scroller_data_t *const qtsd = &qtm_scroller_data1[0];

  t9_point_status_t *t9_sts = (t9_point_status_t *)sts;

  if (index >= GET_SCROLLER_GRP_CFG_i(num_scrollers))
    return -2;

  if (GET_SCROLLER_DATA_ni(index, scroller_status) & TOUCH_ACTIVE)
  {
    t9_sts->status = MXT_T9_DETECT;
    if (GET_SCROLLER_DATA_ni(index, scroller_status) & POSITION_CHANGE)
    {
      t9_sts->status |= MXT_T9_MOVE;
    }
    else
    {
      t9_sts->status |= MXT_T9_PRESS;
    }
  }
  else
  {
    t9_sts->status = 0;
  }

  t9_sts->pos.x = 0;
  t9_sts->pos.y = GET_SCROLLER_DATA_ni(index, position);

  return 0;
}
#endif

#ifdef TOUCH_API_SURFACE
ssint tslink_read_surface_state(u8 id, /*t9_point_status_t */ void *sts)
{
  t9_point_status_t *t9_sts = (t9_point_status_t *)sts;

  if (GET_SURFACE_CS_DATA_STATUS() & TOUCH_ACTIVE)
  {
    t9_sts->status = MXT_T9_DETECT;

    if (GET_SURFACE_CS_DATA_STATUS() & POSITION_CHANGE)
      t9_sts->status |= MXT_T9_MOVE;
    else
      t9_sts->status |= MXT_T9_PRESS;
  }
  else
  {
    t9_sts->status = 0;
  }

  t9_sts->pos.x = GET_SURFACE_CONTACT_DATA_ni(id, v_position);
  t9_sts->pos.y = GET_SURFACE_CONTACT_DATA_ni(id, h_position);

  return 0;
}
#endif

#ifdef OBJECT_T8
static u8 sensing_mode_to_object_translate(u8 mode)
{
  switch (mode)
  {
  case NODE_SELFCAP:
    return MXT_T8_MEASALLOW_SELFPROX;
  case NODE_SELFCAP_SHIELD:
    return MXT_T8_MEASALLOW_SELFTCH;
  case NODE_MUTUAL:
    return MXT_T8_MEASALLOW_MUTUALTCH;
  case NODE_MUTUAL_4P:
    return MXT_T8_MEASALLOW_MUTUALTCH_4P;
  case NODE_MUTUAL_8P:
    return MXT_T8_MEASALLOW_MUTUALTCH_8P;
  default:
    return 0;
  }
}

static u8 sensing_mode_to_tslib_translate(u8 mode)
{
  if (mode & MXT_T8_MEASALLOW_SELFTCH)
  {
    return NODE_SELFCAP_SHIELD;
  }
  else if (mode & MXT_T8_MEASALLOW_MUTUALTCH)
  {
    if (mode == MXT_T8_MEASALLOW_MUTUALTCH_8P)
    {
      return NODE_MUTUAL_8P;
    }
    else if (mode == MXT_T8_MEASALLOW_MUTUALTCH_4P)
    {
      return NODE_MUTUAL_4P;
    }
    else
    {
      return NODE_MUTUAL;
    }
  }
  else if (mode & MXT_T8_MEASALLOW_SELFPROX)
  {
    return NODE_SELFCAP;
  }

  return 0;
}
#endif

bool tslink_touch_adc_init(void)
{
  return (touch_adc_init() == TOUCH_SUCCESS);
}

bool tslink_touch_adc_deinit(void)
{
  return (touch_adc_deinit() == TOUCH_SUCCESS);
}

void tslink_touch_suspend(bool suspend)
{
  touch_suspend(suspend);
}

void tslink_touch_inject_event(void)
{
  touch_inject_event();
}

bool tslink_touch_state_idle(void)
{
  return (touch_state_idle() == 0);
}

bool tslink_touch_state_sleep(void)
{
  return (touch_state_sleep() == 0);
}

u8 tslink_touch_sleep(void)
{
  return touch_sleep();
}

void tslink_touch_init(void)
{
  pin_config_init();

  return touch_init();
}

void tslink_touch_process(void)
{
  return touch_process();
}

void tslink_touch_done(void)
{
  touch_done();
}

#ifdef MPTT_FUSE_CHECK
ssint tslink_fuse_check(void)
{
  return fuse_check();
}
#endif

#ifdef OBJECT_T37_DEBUG_PLATFORM_INFO
const u8 *tslink_get_signature_row_data(u8 *len_ptr)
{
  return get_signature_row_data(len_ptr);
}
#endif

#ifdef MPTT_FUSE_CHECK
const u8 *tslink_get_fuse_data(u8 *len_ptr)
{
  return get_fuse_data(len_ptr);
}
#endif

/* sensor key index to channel node/group mapping */
uint8_t tslink_get_sensor_key_mapping(uint8_t sensor_node, uint8_t group)
{
  return tslib_get_sensor_key_mapping(sensor_node, group);
}

/* channel node to sensor key index mapping */
uint8_t tslink_get_channel_node_mapping(uint8_t channel_node)
{
  return tslib_get_channel_node_mapping(channel_node);
}
