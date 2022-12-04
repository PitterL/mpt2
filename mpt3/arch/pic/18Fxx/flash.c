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

#include <memory.h>
#include <common/include/types.h>
#include "arch/flash.h"

#ifdef MPTT_SAVE_CONFIG
ssint inf_load_cfg(u8 *data, size_t len)
{
    size_t i;
    
	if (len >  EEPROM_SIZE - OFFSET_CONFIG_IN_EEPROM)
		return -2;

	/* Read EEPROM */
    for (i = 0; i < len; i++) {
        data[i] = DATAEE_ReadByte(OFFSET_CONFIG_IN_EEPROM + i);
    }

	return 0;
}

ssint inf_save_cfg(const u8 *data, size_t len)
{
    size_t i;
    int8_t result = 0;

	if (len >  EEPROM_SIZE - OFFSET_CONFIG_IN_EEPROM)
		return -2;

	/* Write EEPROM */
    for (i = 0; i < len && result; i++) {
        result = DATAEE_WriteByte(OFFSET_CONFIG_IN_EEPROM + i, data[i]);
    }
    
    return result;
}
#endif
