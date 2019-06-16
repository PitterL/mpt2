/*
 * t6.c
 *
 * Created: 6/8/2019 11:31:42 PM
 *  Author: A41450
 */ 
#include <string.h>
#include "../mptt.h"
#include "t6.h"
#ifdef OBJECT_T37
#include "t37.h"
#endif

t6_data_t t6_data_status;
ssint object_t6_init(u8 rid,  const /*sensor_config_t*/void *cfg, void *mem, void *cb)
{
	t6_data_t *ptr = &t6_data_status;
	
	ptr->rid = rid;
	ptr->status = MXT_T6_STATUS_RESET; /* Initialized by Reset */
	ptr->mem = (object_t6_t *)mem;
	
	return 0;
}

void object_t6_start(void)
{
	t6_data_t *ptr = &t6_data_status;
	
	mpt_chip_get_config_crc(&ptr->crc);
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
				ptr->status &= ~MXT_T6_STATUS_RESET;
		break;
		case MXT_COMMAND_CALIBRATE:
			if (arg)
				ptr->status |= MXT_T6_STATUS_CAL;
			else
				ptr->status &= ~MXT_T6_STATUS_CAL;
		break;
		case MXT_COMMAND_BACKUPNV:
		case MXT_COMMAND_REPORTALL:
		case MXT_COMMAND_DIAGNOSTIC:
		default:
		;
	}
	
	object_t6_report_status();
}

void chip_reset(u8 arg)
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
}

void chip_backup(u8 arg)
{	
	if (arg == MXT_BACKUP_VALUE) {
		send_chip_status(MXT_COMMAND_BACKUPNV, 1);
		/* performance config backup */
		mpt_chip_backup();	
		
		send_chip_status(MXT_COMMAND_BACKUPNV, 0);
	}
}

void chip_calibrate(u8 arg)
{
	if (arg) {
		send_chip_status(MXT_COMMAND_CALIBRATE, 1);
		/* performance calibration */
		mpt_chip_calibrate();
		
		send_chip_status(MXT_COMMAND_CALIBRATE, 0);
	}
}

void chip_reportall(u8 arg)
{
	if (arg) {
		/* performance report all */
		mpt_chip_reportall();
	}
}

#ifdef OBJECT_T37
void chip_diagnostic(u8 arg)
{
	t6_data_t *ptr = &t6_data_status;
	
	if (arg) {
		/* performance diagnostic */
		switch(arg) {
			case MXT_DIAGNOSTIC_PAGEUP:
				ptr->dbgpage++;
			break;
			case MXT_DIAGNOSTIC_PAGEDOWN:
				ptr->dbgpage--;
			break;
			case MXT_DIAGNOSTIC_MC_DELTA:
			case MXT_DIAGNOSTIC_MC_REF:
			case MXT_DIAGNOSTIC_DC_DATA:
			case MXT_DIAGNOSTIC_CAL_DATA:
			case MXT_DIAGNOSTIC_DEVICE_INFO:
			case MXT_DIAGNOSTIC_PRODUCT_DATA:
			case MXT_DIAGNOSTIC_SC_DELTA:
			case MXT_DIAGNOSTIC_SC_REF:
			case MXT_DIAGNOSTIC_SC_SIGNAL:
				ptr->dbgcmd = arg;
				ptr->dbgpage = 0;
			break;
			default:
				ptr->dbgcmd = MXT_DIAGNOSTIC_NONE;
		}
		object_t37_set_data_page(ptr->dbgcmd, ptr->dbgpage);
	}
}
#endif

ssint object_t6_handle_command(u16 cmd, u8 arg)
{
	t6_data_t *ptr = &t6_data_status;
	ssint result = 0;
	
	ptr->cmd = cmd;
	
	switch (cmd) {
		case MXT_COMMAND_RESET:
			chip_reset(arg);
		break;
		case MXT_COMMAND_BACKUPNV:
			chip_backup(arg);
		break;
		case MXT_COMMAND_CALIBRATE:
			chip_calibrate(arg);
		break;
		case MXT_COMMAND_REPORTALL:
			chip_reportall(arg);
		break;
#ifdef OBJECT_T37
		case MXT_COMMAND_DIAGNOSTIC:
			chip_diagnostic(arg);
		break;
#endif
		default:
			result = -2;
	}
	
	return result;
}