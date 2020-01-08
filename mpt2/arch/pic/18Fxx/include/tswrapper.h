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

typedef enum tag_recal_threshold_t {
	RECAL_100,
	RECAL_50,
	RECAL_25,
	RECAL_12_5,
	RECAL_6_25,
	MAX_RECAL
} recal_threshold_t;

#define SURFACE_RESOLUTION(m) SCR_RESOLUTION(m)
#define SURFACE_DEADBAND(m) SCR_DEADBAND(m)

/*----------------------------------------------------------------------------
 *     Structure Declarations
 *----------------------------------------------------------------------------*/

/* Node group configuration */
typedef struct qtm_acq_node_group_config_type {
	uint16_t num_sensor_nodes;    /* Number of sensor nodes */
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