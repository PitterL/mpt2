/*
 * t6.c
 *
 * Created: 6/8/2019 11:31:42 PM
 *  Author: A41450
 */ 
#include <string.h>

#include "../mptt.h"
#include "t6.h"

t6_data_t t6_data_status;
int object_t6_init(u8 rid)
{
	t6_data_t *ptr = &t6_data_status;
	memset(ptr, 0, sizeof(*ptr));
	
	ptr->rid = rid;
	ptr->status = MXT_T6_STATUS_RESET; /* Initialized by Reset */
	
	return 0;
}

int object_t6_start(void)
{
	t6_data_t *ptr = &t6_data_status;
	
	mpt_chip_get_config_crc(&ptr->crc);
	
	return 0;
}

void object_t6_report_status(void)
{
	t6_data_t *ptr = &t6_data_status;
	object_t5_t message;
	
	memset(&message, 0, sizeof(message));

	message.reportid = ptr->rid;
	message.data[0] = ptr->status;
	message.data[1] = ptr->crc.data[0];
	message.data[2] = ptr->crc.data[1];
	message.data[3] = ptr->crc.data[2];
	
	mpt_write_message(&message);
}

void send_chip_status(u8 cmd, u8 arg)
{
	t6_data_t *ptr = &t6_data_status;
	
	switch (cmd) {
		case MXT_COMMAND_RESET:
			if (arg)
				ptr->status |= MXT_T6_STATUS_RESET;
			else
				ptr->status &= MXT_T6_STATUS_RESET;
		break;
		case MXT_COMMAND_CALIBRATE:
			if (arg)
				ptr->status |= MXT_T6_STATUS_CAL;
			else
				ptr->status &= MXT_T6_STATUS_CAL;
		break;
		case MXT_COMMAND_BACKUPNV:
		case MXT_COMMAND_REPORTALL:
		case MXT_COMMAND_DIAGNOSTIC:
		default:
		;
	}
	
	object_t6_report_status();
}

int chip_reset(u8 arg)
{
	if (arg == MXT_BOOT_VALUE) {
		/* Reboot to bootloader mode */
	}else if (arg != 0) {
		/* Normal reset */
		
		/* Update reg */
		mpt_object_write(MXT_GEN_COMMAND_T6, 0, MXT_COMMAND_RESET, &arg, 1);
		send_chip_status(MXT_COMMAND_RESET, 1);
		
		/* Do reset */
		mpt_chip_reset();
		
		/* Never return since chip will reset */
		while(1);
	}
	
	return -2;
}

int chip_backup(u8 arg)
{	
	if (arg == MXT_BACKUP_VALUE) {
		send_chip_status(MXT_COMMAND_BACKUPNV, 1);
		/* performance config backup */
		mpt_chip_backup();	
		
		send_chip_status(MXT_COMMAND_BACKUPNV, 0);
	}
	
	return 0;
}

int chip_calibrate(u8 arg)
{
	if (arg) {
		send_chip_status(MXT_COMMAND_CALIBRATE, 1);
		/* performance calibration */
		mpt_chip_calibrate();
		
		send_chip_status(MXT_COMMAND_CALIBRATE, 0);
	}
	
	return 0;
}

int chip_reportall(u8 arg)
{
	if (arg) {
		/* performance report all */
		/* ... */
	}
	
	return -2;
}

int chip_diagnostic(u8 arg)
{
	if (arg) {
		/* performance report all */
		switch(arg) {
			case MXT_DIAGNOSTIC_PAGEUP:
			break;
			case MXT_DIAGNOSTIC_PAGEDOWN:
			break;
			case MXT_DIAGNOSTIC_MC_DELTA:
			break;
			case MXT_DIAGNOSTIC_MC_REF:
			break;
			case MXT_DIAGNOSTIC_DC_DATA:
			break;
			case MXT_DIAGNOSTIC_DEVICE_INFO:
			break;
			case MXT_DIAGNOSTIC_PRODUCT_DATA:
			break;
			case MXT_DIAGNOSTIC_SC_DELTA:
			break;
			case MXT_DIAGNOSTIC_SC_REF:
			break;
			default:
				;
		}
	}
	
	return -2;
}

int object_t6_handle_command(u16 cmd, u8 arg)
{
	int result = 0;
	
	switch (cmd) {
		case MXT_COMMAND_RESET:
			result = chip_reset(arg);
		break;
		case MXT_COMMAND_BACKUPNV:
			result = chip_backup(arg);
		break;
		case MXT_COMMAND_CALIBRATE:
			result = chip_calibrate(arg);
		break;
		case MXT_COMMAND_REPORTALL:
			result = chip_reportall(arg);
		break;
		case MXT_COMMAND_DIAGNOSTIC:
			result = chip_diagnostic(arg);
		break;
		default:
			result = -2;
	}
	
	return result;
}