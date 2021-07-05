/*
 * t6.c
 *
 * Created: 6/8/2019 11:31:42 PM
 *  Author: A41450
 */ 
#ifdef OBJECT_T6

#include <string.h>
#include "arch/tslapi.h"
#include "txx.h"

t6_data_t t6_data_status;
ssint object_t6_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	t6_data_t *ptr = &t6_data_status;

	object_txx_init(&ptr->common, rid, def, mem, cb);

	ptr->status.value = MXT_T6_STATUS_RESET;	// Initialized as Reset

	return 0;
}

void object_t6_start(u8 unused)
{
	t6_data_t *ptr = &t6_data_status;
	
	MPT_API_CALLBACK(ptr->common.cb, get_config_crc)(&ptr->status.crc);
}

void t6_pulse_status(t6_data_t *ptr, u8 mask, u8 set)
{
	if (set) {
		ptr->status.value |= mask;
	} else {
		ptr->status.value &= ~mask;
	}
	object_txx_report_msg(&ptr->common, &ptr->status, sizeof(ptr->status));
}

void object_t6_report_status(u8 force)
{
	t6_data_t *ptr = &t6_data_status;
	
	if (!force)
		return;
	
	object_txx_report_msg(&ptr->common, &ptr->status, sizeof(ptr->status));
}

u8 object_t6_check_chip_critical(void)
{
	t6_data_t *ptr = &t6_data_status;
	object_t6_t *mem = (object_t6_t *)ptr->common.mem;
	
	return mem->reset;
}

void chip_reset(u8 arg)
{
	t6_data_t *ptr = &t6_data_status;

	if (arg == MXT_BOOT_VALUE) {
		/* Reboot to bootloader mode */
	} else if (arg != 0) {
		/* Normal reset */

		MPT_API_CALLBACK(ptr->common.cb, assert_irq)(0, false);
		
		/* Do reset */
		MPT_API_CALLBACK(ptr->common.cb, reset)();
		
		/* Never return since chip will reset */
		while(1);
	}
}

ssint chip_backup(u8 arg)
{
	t6_data_t *ptr = &t6_data_status;
	ssint result = -1;
	
	switch(arg) {
		case MXT_BACKUP_VALUE:
			/* performance config backup */
			result = MPT_API_CALLBACK(ptr->common.cb, backup)(&ptr->status.crc);
		break;
		default:
			;
	}
	
	//Report CRC
	object_t6_report_status(1);
	
	return result;
}

void chip_calibrate(u8 arg)
{
	t6_data_t *ptr = &t6_data_status;
	
	if (arg) {
		
		/* performance calibration */
		MPT_API_CALLBACK(ptr->common.cb, calibrate)();
	}
}

void chip_reportall(u8 arg)
{
	t6_data_t *ptr = &t6_data_status;
	
	if (arg) {
		
		/* performance report all */
		MPT_API_CALLBACK(ptr->common.cb, report_all)();
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
				ptr->dbg.page++;
			break;
			case MXT_DIAGNOSTIC_PAGEDOWN:
				ptr->dbg.page--;
			break;
			case MXT_DIAGNOSTIC_MC_DELTA:
			case MXT_DIAGNOSTIC_MC_REF:
			case MXT_DIAGNOSTIC_MC_SIGNAL:
			case MXT_DIAGNOSTIC_PTC_DELTA:
			case MXT_DIAGNOSTIC_PTC_REF:
			case MXT_DIAGNOSTIC_PTC_SIGNAL:
			case MXT_DIAGNOSTIC_KEY_DELTA:
			case MXT_DIAGNOSTIC_KEY_REF:
			case MXT_DIAGNOSTIC_KEY_SIGNAL:
			case MXT_DIAGNOSTIC_DC_DATA:
			case MXT_DIAGNOSTIC_CAL_DATA:
			case MXT_DIAGNOSTIC_DEVICE_INFO:
			case MXT_DIAGNOSTIC_PRODUCT_DATA:
			case MXT_DIAGNOSTIC_SC_DELTA:
			case MXT_DIAGNOSTIC_SC_REF:
			case MXT_DIAGNOSTIC_SC_SIGNAL:
			case MXT_DIAGNOSTIC_LOW_POWER_MODE:
				ptr->dbg.cmd = arg;
				ptr->dbg.page = 0;
			break;
			default:
				ptr->dbg.cmd = MXT_DIAGNOSTIC_NONE;
		}
		object_api_t37_set_data_page(ptr->dbg.cmd, ptr->dbg.page);
	}
}
#endif

ssint t6_handle_command(u16 cmd, u8 arg)
{
	ssint result = 0;
	
	switch (cmd) {
		case MXT_COMMAND_RESET:
			chip_reset(arg);
		break;
		case MXT_COMMAND_BACKUPNV:
			result = chip_backup(arg);
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
			result = -127;
	}
	
	return result;
}

void object_api_t6_handle_command(void)
{
	t6_data_t *ptr = &t6_data_status;
	u8 *mem_ptr = (u8 *)ptr->common.mem;
	u8 i;
	ssint ret;
	
	for (i = 0; i < sizeof(object_t6_t); i++) {
		if (mem_ptr[i]) {
			ret = t6_handle_command(i, mem_ptr[i]);
			if (!ret)
				mem_ptr[i] = 0;
		}
	}
}

void object_api_t6_set_status(u8 mask)
{
	t6_data_t *ptr = &t6_data_status;
	
	if (!(ptr->status.value & mask)) {
		ptr->status.value |= mask;
		object_txx_report_msg(&ptr->common, &ptr->status, sizeof(ptr->status));
	}
}

void object_api_t6_clr_status(u8 mask)
{
	t6_data_t *ptr = &t6_data_status;

	if (ptr->status.value & mask) {
		ptr->status.value &= ~mask;
		object_txx_report_msg(&ptr->common, &ptr->status, sizeof(ptr->status));
	}
}

u8 object_api_t6_get_status(void)
{
	t6_data_t *ptr = &t6_data_status;

	return ptr->status.value;
}
#endif