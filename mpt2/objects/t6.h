/*
 * t6.h
 *
 * Created: 6/8/2019 11:31:53 PM
 *  Author: A41450
 */ 


#ifndef T6_H_
#define T6_H_

/* MXT_GEN_COMMAND_T6 field */
#define MXT_COMMAND_RESET	0
#define MXT_COMMAND_BACKUPNV	1
#define MXT_COMMAND_CALIBRATE	2
#define MXT_COMMAND_REPORTALL	3
#define MXT_COMMAND_DIAGNOSTIC	5

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

#define MXT_DIAGNOSTIC_PAGEUP	0x01
#define MXT_DIAGNOSTIC_PAGEDOWN 0x02
#define MXT_DIAGNOSTIC_MC_DELTA 0x10
#define MXT_DIAGNOSTIC_MC_REF	0x11
#define MXT_DIAGNOSTIC_DC_DATA	0x38
#define MXT_DIAGNOSTIC_DEVICE_INFO	0x80
#define MXT_DIAGNOSTIC_PRODUCT_DATA	0x81
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
#define MXT_GEN_COMMAND_T6_RIDS 1

typedef struct t6_data {
	u8 rid;
	u8 status;
	data_crc24_t crc;
} t6_data_t;

int object_t6_init(u8 rid);
int object_t6_start(void);
void object_t6_report_status(void);

int object_t6_handle_command(u16 cmd, u8 arg);

#endif /* T6_H_ */