/*
 * flash.h
 *
 * Created: 12/11/2019 1:31:49 PM
 *  Author: A41450
 */ 


#ifndef ARCH_FLASH_H_
#define ARCH_FLASH_H_

#ifndef EEPROM_SIZE
#define EEPROM_SIZE 256
#endif

#define OFFSET_CONFIG_IN_EEPROM 0

ssint inf_load_cfg(u8 *data, size_t len);
ssint inf_save_cfg(const u8 *data, size_t len);

#endif /* ARCH_FLASH_H_ */