/*
 * trans.c
 *
 * Created: 6/14/2019 10:14:51 PM
 *  Author: A41450
 */ 

#include <string.h>
#include <touch_api_ptc.h>
#include "mptt.h"
#include "tsl.h"

//extern qtm_surface_cs_control_t qtm_surface_cs_control1;
/* Node status, signal, calibration values */
//extern qtm_acq_node_data_t ptc_qtlib_node_stat1[DEF_NUM_CHANNELS];

/* Node configure parameters */
extern qtm_acq_t161x_node_config_t ptc_seq_node_cfg1[];

/* Key Configurations */
extern qtm_touch_key_config_t qtlib_key_configs_set1[];

/* Key data */
extern qtm_touch_key_data_t qtlib_key_data_set1[];

/* Keys set 1 - General settings */
extern qtm_touch_key_group_config_t qtlib_key_grp_config_set1;

/* Container structure for sensor group */
extern qtm_acquisition_control_t qtlib_acq_set1;

/* Acquisition set 1 - General settings */
extern qtm_acq_node_group_config_t ptc_qtlib_acq_gen1;

/* Surface Config */
extern qtm_surface_cs_config_t qtm_surface_cs_config1;

/* Surface Data */
extern qtm_surface_contact_data_t qtm_surface_cs_data1;

/* Acquisition timer schedule interval */
#ifdef OBJECT_T7
extern uint8_t qtlib_time_elapsed_since_update;
#endif

enum {
	COMMON_WRITE,
	BIT_WRITE,
	ARRAY_MEM_WRITE,
	NUM_WRITEBACK
};

typedef struct tch_config_callback {
	u8 type;
	void *dst;
	size_t size;	//For common_write, this is data size, for array_member_write, this is struct size
	u8 trunk_size;
	u8 mask;
	u8 wbflag;
} tch_config_callback_t;

tch_config_callback_t touch_config_list[] ={
#ifdef OBJECT_T7
	/* { DEF_TOUCH_MEASUREMENT_PERIOD_MS } Not support untill put it into a global 'varible qtlib_time_elapsed_since_update' */
	{DEF_TOUCH_MEASUREMENT_PERIOD_MS, &qtlib_time_elapsed_since_update, sizeof(qtlib_time_elapsed_since_update), 0, 0, COMMON_WRITE },
#endif
	{DEF_SEL_FREQ_INIT, &ptc_qtlib_acq_gen1.freq_option_select, sizeof(ptc_qtlib_acq_gen1.freq_option_select), 0, 0, COMMON_WRITE },
	
	{NODE_PARAMS_CSD, &ptc_seq_node_cfg1[0].node_csd, sizeof(ptc_seq_node_cfg1[0].node_csd), sizeof(ptc_seq_node_cfg1[0]), 0, ARRAY_MEM_WRITE },
	{NODE_PARAMS_RESISTOR_PRESCALER, &ptc_seq_node_cfg1[0].node_rsel_prsc, sizeof(ptc_seq_node_cfg1[0].node_rsel_prsc), sizeof(ptc_seq_node_cfg1[0]), 0, ARRAY_MEM_WRITE },
	{NODE_PARAMS_GAIN, &ptc_seq_node_cfg1[0].node_gain, sizeof(ptc_seq_node_cfg1[0].node_gain), sizeof(ptc_seq_node_cfg1[0]), 0, ARRAY_MEM_WRITE },
	{NODE_PARAMS_ADC_OVERSAMPLING, &ptc_seq_node_cfg1[0].node_gain, sizeof(ptc_seq_node_cfg1[0].node_gain), sizeof(ptc_seq_node_cfg1[0]), 0, ARRAY_MEM_WRITE },
	
	{KEY_PARAMS_THRESHOLD, &qtlib_key_configs_set1[0].channel_threshold, sizeof(qtlib_key_configs_set1[0].channel_threshold), sizeof(qtlib_key_configs_set1[0]), 0, ARRAY_MEM_WRITE},
	{KEY_PARAMS_HYSTERESIS, &qtlib_key_configs_set1[0].channel_hysteresis, sizeof(qtlib_key_configs_set1[0].channel_hysteresis), sizeof(qtlib_key_configs_set1[0]), 0, ARRAY_MEM_WRITE},
	{KEY_PARAMS_AKS_GROUP, &qtlib_key_configs_set1[0].channel_aks_group, sizeof(qtlib_key_configs_set1[0].channel_aks_group), sizeof(qtlib_key_configs_set1[0]), 0, ARRAY_MEM_WRITE},
	
	{DEF_TOUCH_DET_INT, &qtlib_key_grp_config_set1.sensor_touch_di, sizeof(qtlib_key_grp_config_set1.sensor_touch_di), 0, 0, COMMON_WRITE},
	{DEF_MAX_ON_DURATION, &qtlib_key_grp_config_set1.sensor_max_on_time, sizeof(qtlib_key_grp_config_set1.sensor_max_on_time), 0, 0, COMMON_WRITE},
	{DEF_ANTI_TCH_DET_INT, &qtlib_key_grp_config_set1.sensor_anti_touch_di, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_di), 0, 0, COMMON_WRITE},
	{DEF_ANTI_TCH_RECAL_THRSHLD, &qtlib_key_grp_config_set1.sensor_anti_touch_recal_thr, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_recal_thr), 0, 0, COMMON_WRITE},
	{DEF_TCH_DRIFT_RATE, &qtlib_key_grp_config_set1.sensor_touch_drift_rate, sizeof(qtlib_key_grp_config_set1.sensor_touch_drift_rate), 0, 0, COMMON_WRITE},
	{DEF_ANTI_TCH_DRIFT_RATE, &qtlib_key_grp_config_set1.sensor_anti_touch_drift_rate, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_drift_rate), 0, 0, COMMON_WRITE},
	{DEF_DRIFT_HOLD_TIME, &qtlib_key_grp_config_set1.sensor_drift_hold_time, sizeof(qtlib_key_grp_config_set1.sensor_drift_hold_time), 0, 0, COMMON_WRITE},
				
};

#define TCH_CONFIG_WRITEBACK_NUM ARRAY_SIZE(touch_config_list)

void common_write(void *dst, const void *src, size_t size)
{
	memcpy(dst, src, size);
}

void bit_write(void *dst, const void *src, u8 mask)
{
	*(u8 *)dst = ((*(u8 *)dst) & ~mask) | ((*(u8 *)src) & mask);
}

void array_member_write(void *dst, const void *src, size_t size, u8 struct_size, u8 index)
{
	memcpy(dst + (struct_size * index), src, size);
}

static inline ssint tch_write(const tch_config_callback_t *param, const void *src, size_t size, u8 index)
{
	if (size != param->size)
		return -2;
	
	switch (param->wbflag) {
		case COMMON_WRITE:
			common_write(param->dst, src, param->size);
		break;
		case BIT_WRITE:
			bit_write(param->dst, src, param->mask);
		break;
		case ARRAY_MEM_WRITE:
			array_member_write(param->dst, src, param->size, param->trunk_size, index);
		break;
		default:
		;
	}
	
	return 0;
}

ssint tch_config_write(u8 type, const void *src, size_t size, u8 index)
{
	const tch_config_callback_t *tccs = &touch_config_list[0];
	u8 i;
	ssint result = -1;
	
	for (i = 0; i < TCH_CONFIG_WRITEBACK_NUM; i++) {
		if (tccs[i].type == type) {
			result = tch_write(&tccs[i], src, size, index);
			break;
		}
	}
	
	return result;
}

void tch_calibrate(void)
{
	qtm_acq_node_group_config_t *qtacq = &ptc_qtlib_acq_gen1;
	u8 i;
	
	for (i = 0; i < (u8)qtacq->num_sensor_nodes; i++) {
		calibrate_node(i);
	}
}

tsl_interface_info_t interface_tsl = {
	.fn_calibrate = tch_calibrate,
	.fn_writeback = tch_config_write,
};

void tsl_init(const hal_interface_info_t *hal)
{
	const qtm_surface_cs_config_t *qtcfg = &qtm_surface_cs_config1;
	qtm_acq_node_group_config_t *qtacq = &ptc_qtlib_acq_gen1;
	tsl_interface_info_t *tsl = &interface_tsl;
	
	// Save HAL interface
	memcpy(&tsl->hal, hal, sizeof(*hal));
	
	// x start first, y follow up
	if (qtcfg->start_key_h < qtcfg->start_key_v) {
		tsl->matrix_xsize = qtcfg->number_of_keys_h + qtcfg->start_key_h;
		tsl->matrix_ysize = qtcfg->number_of_keys_v + qtcfg->start_key_v - tsl->matrix_xsize;
	} else {
		tsl->matrix_xsize = qtcfg->number_of_keys_v + qtcfg->start_key_v;
		tsl->matrix_ysize = qtcfg->number_of_keys_h + qtcfg->start_key_h - tsl->matrix_xsize;
	}
	
	switch (qtacq->acq_sensor_type) {
		case NODE_SELFCAP:
		case NODE_SELFCAP_SHIELD:
			tsl->measallow = MXT_T8_MEASALLOW_SELFTCH;
		break;
		case NODE_MUTUAL:
		case NODE_MUTUAL_4P:
		case NODE_MUTUAL_8P:
		default:
			tsl->measallow = MXT_T8_MEASALLOW_MUTUALTCH;
	}

	mpt_chip_init(tsl);
}

void tsl_start(void)
{
	mpt_chip_start();
	
	mpt_chip_reportall();
}

void tsl_process(void)
{
	const qtm_surface_contact_data_t *qtsf = &qtm_surface_cs_data1;
	const qtm_touch_key_data_t *qtkds = &qtlib_key_data_set1[0];
	const tsl_interface_info_t *tsl = &interface_tsl;
	const qtm_touch_key_group_config_t *qttkg = &qtlib_key_grp_config_set1;
	u8 i, t9_status = 0;
	
	
	for (i = 0; i < qttkg->num_key_sensors; i++) {
		mpt_api_set_sensor_data(i, qtkds->sensor_state, qtkds[i].channel_reference, qtkds[i].node_data_struct_ptr->node_acq_signals, qtkds[i].node_data_struct_ptr->node_comp_caps);
	}
	
	if (qtsf->qt_surface_status & TOUCH_ACTIVE)
		t9_status = MXT_T9_DETECT | MXT_T9_PRESS;
	else if(qtsf->qt_surface_status & POSITION_CHANGE)
		t9_status = MXT_T9_DETECT | MXT_T9_MOVE;

	mpt_api_set_pointer_location(0, t9_status,  qtsf->h_position, qtsf->v_position);
	
	mpt_api_request_irq(tsl->hal.fn_set_chg);
	
	mpt_api_process();
}

ssint tsl_write_message(const void *msg)
{
	return mpt_write_message(msg);
}

ssint tsl_read_message(void *msg)
{
	return mpt_read_message(msg);
}

ssint tsl_mem_read(u16 baseaddr, u16 offset, u8 *out_ptr)
{
	return mpt_mem_read(baseaddr, offset, out_ptr);
}

ssint tsl_mem_write(u16 regaddr, u8 val)
{
	return mpt_mem_write(regaddr, val);
}