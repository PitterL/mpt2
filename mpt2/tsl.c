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
	COMMON_RW,
	BIT_RW,
	ARRAY_MEM_RW,
	NUM_RW_OP
};

typedef struct tch_config_callback {
	u8 type;
	void *buf;
	size_t size;	//For common_write, this is data size, for array_member_write, this is struct size
	u8 trunk_size;
	u8 mask;
	u8 wbflag;
} tch_config_callback_t;

tch_config_callback_t touch_config_list[] ={
#ifdef OBJECT_T7
	/* { DEF_TOUCH_MEASUREMENT_PERIOD_MS } Not support untill put it into a global 'varible qtlib_time_elapsed_since_update' */
	{DEF_TOUCH_MEASUREMENT_PERIOD_MS, &qtlib_time_elapsed_since_update, sizeof(qtlib_time_elapsed_since_update), 0, 0, COMMON_RW },
#endif
	{DEF_SEL_FREQ_INIT, &ptc_qtlib_acq_gen1.freq_option_select, sizeof(ptc_qtlib_acq_gen1.freq_option_select), 0, 0, COMMON_RW },
	
	{NODE_PARAMS_CSD, &ptc_seq_node_cfg1[0].node_csd, sizeof(ptc_seq_node_cfg1[0].node_csd), sizeof(ptc_seq_node_cfg1[0]), 0, ARRAY_MEM_RW },
	{NODE_PARAMS_RESISTOR_PRESCALER, &ptc_seq_node_cfg1[0].node_rsel_prsc, sizeof(ptc_seq_node_cfg1[0].node_rsel_prsc), sizeof(ptc_seq_node_cfg1[0]), 0, ARRAY_MEM_RW },
	{NODE_PARAMS_GAIN, &ptc_seq_node_cfg1[0].node_gain, sizeof(ptc_seq_node_cfg1[0].node_gain), sizeof(ptc_seq_node_cfg1[0]), 0, ARRAY_MEM_RW },
	{NODE_PARAMS_ADC_OVERSAMPLING, &ptc_seq_node_cfg1[0].node_oversampling, sizeof(ptc_seq_node_cfg1[0].node_oversampling), sizeof(ptc_seq_node_cfg1[0]), 0, ARRAY_MEM_RW },
	
	{KEY_PARAMS_THRESHOLD, &qtlib_key_configs_set1[0].channel_threshold, sizeof(qtlib_key_configs_set1[0].channel_threshold), sizeof(qtlib_key_configs_set1[0]), 0, ARRAY_MEM_RW},
	{KEY_PARAMS_HYSTERESIS, &qtlib_key_configs_set1[0].channel_hysteresis, sizeof(qtlib_key_configs_set1[0].channel_hysteresis), sizeof(qtlib_key_configs_set1[0]), 0, ARRAY_MEM_RW},
	{KEY_PARAMS_AKS_GROUP, &qtlib_key_configs_set1[0].channel_aks_group, sizeof(qtlib_key_configs_set1[0].channel_aks_group), sizeof(qtlib_key_configs_set1[0]), 0, ARRAY_MEM_RW},
	
	{DEF_TOUCH_DET_INT, &qtlib_key_grp_config_set1.sensor_touch_di, sizeof(qtlib_key_grp_config_set1.sensor_touch_di), 0, 0, COMMON_RW},
	{DEF_MAX_ON_DURATION, &qtlib_key_grp_config_set1.sensor_max_on_time, sizeof(qtlib_key_grp_config_set1.sensor_max_on_time), 0, 0, COMMON_RW},
	{DEF_ANTI_TCH_DET_INT, &qtlib_key_grp_config_set1.sensor_anti_touch_di, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_di), 0, 0, COMMON_RW},
	{DEF_ANTI_TCH_RECAL_THRSHLD, &qtlib_key_grp_config_set1.sensor_anti_touch_recal_thr, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_recal_thr), 0, 0, COMMON_RW},
	{DEF_TCH_DRIFT_RATE, &qtlib_key_grp_config_set1.sensor_touch_drift_rate, sizeof(qtlib_key_grp_config_set1.sensor_touch_drift_rate), 0, 0, COMMON_RW},
	{DEF_ANTI_TCH_DRIFT_RATE, &qtlib_key_grp_config_set1.sensor_anti_touch_drift_rate, sizeof(qtlib_key_grp_config_set1.sensor_anti_touch_drift_rate), 0, 0, COMMON_RW},
	{DEF_DRIFT_HOLD_TIME, &qtlib_key_grp_config_set1.sensor_drift_hold_time, sizeof(qtlib_key_grp_config_set1.sensor_drift_hold_time), 0, 0, COMMON_RW},
				
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

void array_member_write(void *dst, const void *src, size_t size)
{
	memcpy(dst, src, size);
}

static inline ssint tch_config_rw(const tch_config_callback_t *param, void *buf, size_t size, u8 index, u8 rw)
{
	void *dst, *src;
	size_t trunk_size = 0;
	
	if (size != param->size)
		return -2;
	
	if (param->wbflag == ARRAY_MEM_RW)
		trunk_size = param->trunk_size ? param->trunk_size * index : 0;
		
	if (rw) {	//read: 1, write: 0
		dst = buf;
		src = param->buf + trunk_size;
	}else {
		dst = param->buf + trunk_size;
		src = buf;
	}
	
	switch (param->wbflag) {
		case COMMON_RW:
			common_write(dst, src, param->size);
		break;
		case BIT_RW:
			bit_write(dst, src, param->mask);
		break;
		case ARRAY_MEM_RW:
			common_write(dst, src, param->size);
		break;
		default:
		;
	}
	
	return 0;
}

ssint tch_config_op(u8 type, void *buf, size_t size, u8 index, u8 rw)
{
	const tch_config_callback_t *tccs = &touch_config_list[0];
	u8 i;
	ssint result = -1;
	
	for (i = 0; i < TCH_CONFIG_WRITEBACK_NUM; i++) {
		if (tccs[i].type == type) {
			result = tch_config_rw(&tccs[i], buf, size, index, rw);
			break;
		}
	}
	
	return result;
}

ssint tch_config_write(u8 type, void *src, size_t size, u8 index) 
{
	return tch_config_op(type, src, size, index, 0);
}

ssint tch_config_read(u8 type, void *dst, size_t size, u8 index) 
{
	return tch_config_op(type, dst, size, index, 1);
}

void tch_calibrate(void)
{
	qtm_acq_node_group_config_t *qtacq = &ptc_qtlib_acq_gen1;
	u8 i;
	
	for (i = 0; i < (u8)qtacq->num_sensor_nodes; i++) {
		calibrate_node(i);
	}
}

qtouch_api_callback_t tsl_api_info =
{
	.write = tch_config_write,
	.read = tch_config_read,
	.sync = tch_config_op,
	.calibrate = tch_calibrate,
};

tsl_interface_info_t interface_tsl = 
{
	.api = &tsl_api_info,
};

void tsl_init(const hal_interface_info_t *hal)
{
	const qtm_surface_cs_config_t *qtcfg = &qtm_surface_cs_config1;
	qtm_acq_node_group_config_t *qtacq = &ptc_qtlib_acq_gen1;
	tsl_interface_info_t *tsl = &interface_tsl;
	
	// Save HAL interface
	//memcpy(&tsl->hal, hal, sizeof(*hal));
	tsl->hal = hal;
	
	// x start first, y follow up
	if (qtcfg->start_key_h < qtcfg->start_key_v) {
		tsl->qtdef.matrix_xsize = qtcfg->number_of_keys_h + qtcfg->start_key_h;
		tsl->qtdef.matrix_ysize = qtcfg->number_of_keys_v + qtcfg->start_key_v - tsl->qtdef.matrix_xsize;
	} else {
		tsl->qtdef.matrix_xsize = qtcfg->number_of_keys_v + qtcfg->start_key_v;
		tsl->qtdef.matrix_ysize = qtcfg->number_of_keys_h + qtcfg->start_key_h - tsl->qtdef.matrix_xsize;
	}
	
	switch (qtacq->acq_sensor_type) {
		case NODE_SELFCAP:
		case NODE_SELFCAP_SHIELD:
			tsl->qtdef.measallow = MXT_T8_MEASALLOW_SELFTCH;
		break;
		case NODE_MUTUAL:
		case NODE_MUTUAL_4P:
		case NODE_MUTUAL_8P:
		default:
			tsl->qtdef.measallow = MXT_T8_MEASALLOW_MUTUALTCH;
	}

	mpt_chip_init(tsl);
}

void tsl_start(void)
{
	mpt_chip_start();
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
	
	mpt_api_request_irq(tsl->hal->fn_set_chg);
	
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