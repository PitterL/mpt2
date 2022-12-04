/*
 * flash.h
 *
 * Created: 12/11/2019 1:31:49 PM
 *  Author: A41450
 */

#ifndef ARCH_FLASH_H_
#define ARCH_FLASH_H_

#include <nvmctrl_basic.h>
#include <common/include/crc.h>

#ifndef EEPROM_SIZE
#define EEPROM_SIZE 256
#endif

#define CONFIG_TAG 'C'
#define CONFIG_VER 3

typedef struct cfg_head
{
	u8 tag;
	u8 ver;
	u16 len;
} __attribute__((packed)) cfg_head_t;

typedef data_crc24_t cfg_tail_t;

#define OFFSET_CONFIG_IN_EEPROM (sizeof(cfg_head_t))
#define CONFIG_TAIL_SIZE (sizeof(cfg_tail_t))

ssint inf_load_cfg(u8 *data, size_t len);
ssint inf_save_cfg(const u8 *data, size_t len, u8 ccp);

#endif /* ARCH_FLASH_H_ */