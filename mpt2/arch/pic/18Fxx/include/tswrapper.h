/*============================================================================
Filename : qtm_touch_key_api.h
Project : QTouch Modular Library
Purpose : Structs and definitions for use within modules
------------------------------------------------------------------------------
Copyright (c) 2019 Microchip. All rights reserved.
------------------------------------------------------------------------------
============================================================================*/

#ifndef TOUCH_API_TSWRAPPER_H
#define TOUCH_API_TSWRAPPER_H

#include <mtouch/mtouch.h>

#ifdef OBJECT_T15
#ifdef MTOUCH_BUTTON_H
#define TOUCH_API_BUTTON
#endif
#endif

#ifdef OBJECT_T9
#ifdef __MTOUCH_SLIDER_H__
#define TOUCH_API_SCROLLER
#endif
#ifdef __MTOUCH_SURFACE_H__
#define TOUCH_API_SURFACE
#endif
#endif

/* ---------------------------------------------------------------------------------------- */
/* Acquisition type */
/* ---------------------------------------------------------------------------------------- */
/* Acquisition set config */
#define NODE_SELFCAP 0x80u
#define NODE_SELFCAP_SHIELD 0x81u
#define NODE_MUTUAL 0x40u
#define NODE_MUTUAL_4P 0x41u
#define NODE_MUTUAL_8P 0x42u

typedef enum tag_filter_level_t {
	FILTER_LEVEL_1,
	FILTER_LEVEL_2,
	FILTER_LEVEL_4,
	FILTER_LEVEL_8,
	FILTER_LEVEL_16,
	FILTER_LEVEL_32,
	FILTER_LEVEL_64
} filter_level_t;

/* Touch library GAIN setting */
typedef enum tag_gain_t { GAIN_1, GAIN_2, GAIN_4, GAIN_8, GAIN_16, GAIN_32 } gain_t;
/* PTC clock prescale setting.
 * Example: if Generic clock input to PTC = 4MHz, then:
 * PRSC_DIV_SEL_1 sets PTC Clock to 4MHz
 * PRSC_DIV_SEL_2 sets PTC Clock to 2MHz
 * PRSC_DIV_SEL_4 sets PTC Clock to 1MHz
 * PRSC_DIV_SEL_8 sets PTC Clock to 500KHz
 *
 */
typedef enum tag_prsc_div_sel_t {
	// PRSC_DIV_SEL_1,
	PRSC_DIV_SEL_2,
	PRSC_DIV_SEL_4,
	PRSC_DIV_SEL_8,
	PRSC_DIV_SEL_16,
	PRSC_DIV_SEL_32,
	PRSC_DIV_SEL_64,
	PRSC_DIV_SEL_128
} prsc_div_sel_t;

typedef enum tag_recal_threshold_t {
	RECAL_100,
	RECAL_50,
	RECAL_25,
	RECAL_12_5,
	RECAL_6_25,
	MAX_RECAL
} recal_threshold_t;

/**
 * PTC series resistor setting. For Mutual cap mode, this series
 * resistor is switched internally on the Y-pin. For Self cap mode,
 * thes series resistor is switched internally on the Sensor pin.
 *
 * Example:
 * RSEL_VAL_0 sets internal series resistor to 0ohms.
 * RSEL_VAL_20 sets internal series resistor to 20Kohms.
 * RSEL_VAL_50 sets internal series resistor to 50Kohms.
 * RSEL_VAL_100 sets internal series resistor to 100Kohms.
 */
typedef enum tag_rsel_val_t {
	RSEL_VAL_0,
	RSEL_VAL_20,
	RSEL_VAL_50,
	RSEL_VAL_70,
	RSEL_VAL_100,
	RSEL_VAL_200
} rsel_val_t;

#define SCR_RESOLUTION(m) SURFACE_RESOLUTION(m)
#define SCR_DEADBAND(m) SURFACE_DEADBAND(m)

/*----------------------------------------------------------------------------
 *     Structure Declarations
 *----------------------------------------------------------------------------*/

/* Node group configuration */
typedef struct qtm_acq_node_group_config_type {
	uint16_t num_sensor_nodes;    /* Errata name, it's number of channel nodes actually */
	uint8_t  acq_sensor_type;     /* Self or mutual sensors */
} qtm_acq_node_group_config_t;
extern qtm_acq_node_group_config_t ptc_qtlib_acq_gen1;

#define GET_ACQ_NODE_GRP_CFG(_n)   (ptc_qtlib_acq_gen1._n)

/* Key process module */
/* Sensor group config */
typedef struct {
	uint8_t  num_key_sensors;              /* Number of sensors */
	uint8_t  sensor_touch_di;              /* Count in to Detect */
	uint8_t  sensor_max_on_time;           /* Max on duration Counts */
	uint8_t  sensor_anti_touch_di;         /* Counts in to Anti-touch recal */
	uint8_t  sensor_anti_touch_recal_thr;  /* Anti-touch recal threshold % */
	uint8_t  sensor_touch_drift_rate;      /* Drift counts */
	uint8_t  sensor_anti_touch_drift_rate; /* Anti touch drift Counts, Unused */
	uint8_t  sensor_drift_hold_time;       /* Drift hold cycles, Unused */
	uint8_t  sensor_reburst_mode;          /* Unused */

    uint8_t  node_gain;         /* Bits 7:4 = Reading gain, Bits 3:0 = Digital gain */
} qtm_touch_key_group_config_t;
extern qtm_touch_key_group_config_t qtlib_key_grp_config_set1;

#define PARAM_SAMPLING_LOOP_COUNT_SHIFT 4
#define PARAM_SAMPLING_LOOP_COUNT (1 << PARAM_SAMPLING_LOOP_COUNT_SHIFT)

#define GET_TOUCH_KEY_GRP_CFG(_n)   (qtlib_key_grp_config_set1._n)
#define GET_TOUCH_KEY_GRP_SHIFT_CFG(_n, _rt)     ((_rt)GET_TOUCH_KEY_GRP_CFG(_n) << PARAM_SAMPLING_LOOP_COUNT_SHIFT)
#define GET_TOUCH_KEY_GRP_CFG_BITS(_n, _s, _w)	((GET_TOUCH_KEY_GRP_CFG(_n) >> (_s)) & ((1 << (_w)) - 1))

/* Configuration - Group of scrollers */
typedef struct {
	uint8_t               num_scrollers;
} qtm_scroller_group_config_t;
qtm_scroller_group_config_t qtm_scroller_group_config1;
#define GET_QTM_SCROLLER_GRP_CFG(_n)   (qtm_scroller_group_config1._n)

#define CALCULATE_CAP(_v) (_v)
#define SENSOR_BASE_REF_VALUE 512

#endif /* TOUCH_API_TSWRAPPER_H */