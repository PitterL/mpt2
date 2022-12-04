/**
 * \file
 *
 * \brief flash interface wrapper.
 *
 (c) 2018 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms,you may use this software and
    any derivatives exclusively with Microchip products.It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

#include <ccp.h>
#include "arch/flash.h"

#ifdef MPTT_SAVE_CONFIG

/**
 * \brief Write a block to eeprom
 *
 * \param[in] eeprom_adr The byte-address in eeprom to write to
 * \param[in] data The buffer to write
 *
 * \return Status of write operation
 */
nvmctrl_status_t FLASH_0_ccp_write_eeprom_block(eeprom_adr_t eeprom_adr, const uint8_t *data, size_t size, uint8_t ccp)
{
	uint8_t *write = (uint8_t *)(EEPROM_START + eeprom_adr);

	/* Wait for completion of previous operation */
	while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm))
		;

	/* Program the EEPROM with desired value(s) */
	ccp_write_spm((void *)&NVMCTRL.CTRLA, /* NVMCTRL_CMD_EEERWR_gc */ ccp);

	do {
		/* Write byte to EEPROM */
		*write++ = *data++;
		size--;
	} while (size != 0);

	/* Clear the current command */
	ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);

	return NVM_OK;
}

ssint inf_load_cfg(u8 *data, size_t len)
{
    cfg_head_t header = {0};
    cfg_tail_t tail;
    u32 crc;

    /* the nvm lib doesn't judge the length, so we should check it here */
    if (!len || len > EEPROM_SIZE - OFFSET_CONFIG_IN_EEPROM - CONFIG_TAIL_SIZE)
        return -2;

    /* Read Header */
    FLASH_0_read_eeprom_block(0, (u8 *)&header, sizeof(header));

    if (header.tag != CONFIG_TAG ||
        header.ver != CONFIG_VER)
    {
        return -3;
    }

    /* Read Data */
    FLASH_0_read_eeprom_block(OFFSET_CONFIG_IN_EEPROM, data, len);
    crc = calc_crc24(data, len);

    /* Read Tail */
    FLASH_0_read_eeprom_block(OFFSET_CONFIG_IN_EEPROM + len, (u8 *)&tail, sizeof(tail));

    if (tail.value != crc)
    {
        return -4;
    }

    return 0;
}

ssint inf_save_cfg(const u8 *data, size_t len, u8 ccp)
{
    cfg_head_t header = {CONFIG_TAG, CONFIG_VER, (u16)len};
    /* cfg_tail_t */ u32 crc;
    nvmctrl_status_t result;

    /* the nvm lib doesn't judge the length, so we should check it here */
    if (!len || len > EEPROM_SIZE - OFFSET_CONFIG_IN_EEPROM - CONFIG_TAIL_SIZE)
        return -2;

    /* Write Header */
    result = FLASH_0_ccp_write_eeprom_block(0, (u8 *)&header, sizeof(header), ccp);
    if (NVM_OK != result)
    {
        return -3;
    }

    /* Write Data */
    result = FLASH_0_ccp_write_eeprom_block(OFFSET_CONFIG_IN_EEPROM, data, len, ccp);
    if (NVM_OK != result)
    {
        return -4;
    }

    /* Write Tail */
    crc = calc_crc24(data, len);
    result = FLASH_0_ccp_write_eeprom_block(OFFSET_CONFIG_IN_EEPROM + len, (u8 *)&crc, sizeof(crc), ccp);
    if (NVM_OK != result)
    {
        return -5;
    }

    return 0;
}
#endif
