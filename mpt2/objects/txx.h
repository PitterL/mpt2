/*
 * txx.h
 *
 * Created: 6/9/2019 4:09:54 PM
 *  Author: A41450
 */ 


#ifndef TXX_H_
#define TXX_H_

/* Object types */
#define MXT_DEBUG_DIAGNOSTIC_T37	37
#define MXT_GEN_MESSAGE_T5		5
#define MXT_GEN_COMMAND_T6		6
#define MXT_GEN_POWER_T7		7
#define MXT_GEN_ACQUIRE_T8		8
#define MXT_GEN_DATASOURCE_T53		53
#define MXT_TOUCH_MULTI_T9		9
#define MXT_TOUCH_KEYARRAY_T15		15
#define MXT_TOUCH_PROXIMITY_T23		23
#define MXT_TOUCH_PROXKEY_T52		52
#define MXT_PROCI_GRIPFACE_T20		20
#define MXT_PROCG_NOISE_T22		22
#define MXT_PROCI_ONETOUCH_T24		24
#define MXT_PROCI_TWOTOUCH_T27		27
#define MXT_PROCI_GRIP_T40		40
#define MXT_PROCI_PALM_T41		41
#define MXT_PROCI_TOUCHSUPPRESSION_T42	42
#define MXT_PROCI_STYLUS_T47		47
#define MXT_PROCG_NOISESUPPRESSION_T48	48
#define MXT_SPT_COMMSCONFIG_T18		18
#define MXT_SPT_GPIOPWM_T19		19
#define MXT_SPT_SELFTEST_T25		25
#define MXT_SPT_CTECONFIG_T28		28
#define MXT_SPT_USERDATA_T38		38
#define MXT_SPT_DIGITIZER_T43		43
#define MXT_SPT_MESSAGECOUNT_T44	44
#define MXT_SPT_CTECONFIG_T46		46
#define MXT_SPT_DYNAMICCONFIGURATIONCONTAINER_T71 71
#define MXT_PROCI_SYMBOLGESTUREPROCESSOR	92
#define MXT_PROCI_TOUCHSEQUENCELOGGER	93
#define MXT_TOUCH_MULTITOUCHSCREEN_T100 100
#define MXT_SPT_AUXTOUCHCONFIG_T104 104
#define MXT_SPT_SELFCAPCONFIG_T111 111
#define MXT_PROCI_ACTIVESTYLUS_T107	107

#include "t44.h"
#include "t5.h"
#include "t6.h"
#include "t7.h"
#include "t8.h"
#include "t9.h"
#include "t15.h"
#include "t18.h"
#include "t25.h"
#include "t37.h"
#include "t104.h"
#include "t111.h"

typedef struct sensor_config {
	u8 matrix_xsize;
	u8 matrix_ysize;
	u8 measallow;
} sensor_config_t;

#endif /* TXX_H_ */