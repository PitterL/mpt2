/*
 * t6.h
 *
 * Created: 6/8/2019 11:31:53 PM
 *  Author: A41450
 */ 


#ifndef T6_H_
#define T6_H_

#include "../crc.h"

/* MXT_GEN_COMMAND_T6 field */
#define MXT_COMMAND_RESET	0
#define MXT_COMMAND_BACKUPNV	1
#define MXT_COMMAND_CALIBRATE	2
#define MXT_COMMAND_REPORTALL	3
#define MXT_COMMAND_DEBUGCTRL	4
#define MXT_COMMAND_DIAGNOSTIC	5
#define MXT_COMMAND_DEBUGCTRL2	6

/* Define for T6 status byte */
#define MXT_T6_STATUS_RESET	BIT(7)
#define MXT_T6_STATUS_OFL	BIT(6)
#define MXT_T6_STATUS_SIGERR	BIT(5)
#define MXT_T6_STATUS_CAL	BIT(4)
#define MXT_T6_STATUS_CFGERR	BIT(3)
#define MXT_T6_STATUS_COMSERR	BIT(2)

/* Define for MXT_GEN_COMMAND_T6 */
#define MXT_BOOT_VALUE		0xa5
#define MXT_RESET_VALUE		0x01
#define MXT_BACKUP_VALUE	0x55

#define MXT_DIAGNOSTIC_NONE 0x0
#define MXT_DIAGNOSTIC_PAGEUP	0x01
#define MXT_DIAGNOSTIC_PAGEDOWN 0x02
#define MXT_DIAGNOSTIC_MC_DELTA 0x10
#define MXT_DIAGNOSTIC_MC_REF	0x11
#define MXT_DIAGNOSTIC_KEY_DELTA	0x17
#define MXT_DIAGNOSTIC_KEY_REF	0x18
#define MXT_DIAGNOSTIC_KEY_SIGNAL	0x19
#define MXT_DIAGNOSTIC_CAL_DATA	0x33
#define MXT_DIAGNOSTIC_DC_DATA	0x38
#define MXT_DIAGNOSTIC_DEVICE_INFO	0x80
#define MXT_DIAGNOSTIC_PRODUCT_DATA	0x81
#define MXT_DIAGNOSTIC_SC_SIGNAL	0xF5
#define MXT_DIAGNOSTIC_SC_DELTA	0xF7
#define MXT_DIAGNOSTIC_SC_REF	0xF8

/* T6 Memory Space */
typedef struct object_t6 {
	u8 reset;
	u8 backupnv;
	u8 calibrate;
	u8 reportall;
	u8 rsv;
	u8 diagnostic;
} __attribute__ ((packed)) object_t6_t;

typedef struct t6_debug_command {
	u8 cmd;
	u8 page;
} t6_debug_command_t;

typedef struct t6_data {
	u8 status;
	u8 status_new;
	u8 cmd;
	t6_debug_command_t dbg;
	data_crc24_t crc;
	
	txx_data_t common;
} t6_data_t;

ssint object_t6_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
void object_t6_start(u8 unused);
void object_t6_report_status(u8 force);
void object_t6_process(void);

ssint object_t6_handle_command(u16 cmd, u8 arg);
u8 object_t6_get_diagnostic_status(u8 *pg);
void api_object_t6_set_status(u8 mask);
void api_object_t6_clr_status(u8 mask);

#define MXT_GEN_COMMAND_T6_RIDS 1

#endif /* T6_H_ */