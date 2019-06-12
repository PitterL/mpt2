/*
 * mptt.h
 *
 * Created: 6/7/2019 9:48:20 PM
 *  Author: A41450
 */ 

#ifndef MPTT_H_
#define MPTT_H_

#include "types.h"
#include "crc.h"

#define MPTT_FW_FAMILY_ID 0xa6	//0x81
#define MPTT_FW_VARIANT_ID 0x08	//0x01
#define MPTT_FW_VERSION 0x10
#define MPTT_FW_BUILD 0xBB

#include "objects/txx.h"

typedef struct hal_interface_info {
	u8 matrix_xsize;
	u8 matrix_ysize;
	u8 measallow;

	int (*fn_load_cfg)(u8 *cfg, int size);
	int (*fn_save_cfg)(const u8 *cfg, int size);

	void (*fn_reset)(void);
	void (*fn_calibrate)(void);
	int (*fn_mem_write)(u16 address, u8 *data, int len);
	int (*fn_mem_read)(u16 address, u8 *data, int len);
} hal_interface_info_t;

int mpt_init(const hal_interface_info_t *hal);
void mpt_chip_reset(void);
int mpt_chip_backup(void);
void mpt_chip_calibrate(void);
void mpt_chip_start(void);
void mpt_chip_reportall(void);
void mpt_chip_get_config_crc(data_crc24_t *ptr);
int mpt_chip_load_config(void);

int mpt_write_message(const object_t5_t *msg);
int mpt_read_message(object_t5_t *msg);

u8 mpt_get_message_count(void);
int mpt_mem_read(u16 baseaddr, u16 offset, u8 *out_ptr);
int mpt_mem_write(u16 regaddr, u8 val);
int mpt_object_read(u8 regid, u8 instance, u16 offset, u8 *out_ptr, u8 size);
int mpt_object_write(u8 regid, u8 instance, u16 offset, const u8 *ptr, u8 size);

void mpt_set_sensor_data(u8 channel, u8 state, u16 reference, u16 signal, u16 cap);
void mpt_set_pointer_location(u8 id, uint8_t status, uint16_t x, uint16_t y);

/* Call init before Touch init */
int mpt_interface_init(void);

/* Call start after Touch inited */
int mpt_start(void);

/* Call process when sampling finished each time*/
void mpt_process(void);

#define OFFSET_CONFIG_IN_EEPROM 0

#endif /* MPTT_H_ */