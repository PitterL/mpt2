/*
 * Fuse.c
 *
 * Created: 7/12/2021 5:26:29 PM
 *  Author: A41450
 */ 

#include <memory.h>
#include <board.h>
#include <common/include/types.h>
#include "arch/cpu.h"
#include "arch/fuse.h"

#ifdef MPTT_FUSE_CHECK

static const u16 fuse_target[] = FUSES_CONTENT;
static u16 fusebuf[FUSE_COUNT];
static u16 sigbuf[2];

/**
 * \Check the fuse whether mathed in current runtime fw and target code, the FUSE content is set in pack.h, and will be packed into firmware by `cupdi` command
 * @return: Zero mean pass, otherwise failed
 */
ssint fuse_check(void)
{
	u8 count;
	const u16 * const fv = (const u16 *)get_fuse_data(&count);

	if (count > ARRAY_SIZE(fuse_target)) {
		count = ARRAY_SIZE(fuse_target);
	}

	for (u8 i = 0; i < count; i++) {
		if (fv[i] != fuse_target[i])
			return -1;
	}
	
	return 0;	
}

/**
 * \Get the fuse data information pointer
 * @return the pointer
 */
const u8 *get_fuse_data(u8 *len_ptr)
{	
	for (u8 i = 0; i < ARRAY_SIZE(fusebuf); i++) {
		fusebuf[i] = FLASH_ReadWord((u32)((u16 *)FUSE_ADDRESS + i));
	}

	if (len_ptr) {
		*len_ptr = ARRAY_SIZE(fusebuf);
	}

	return (const u8 *)fusebuf;
}
#endif

#ifdef OBJECT_T37_DEBUG_PLATFORM_INFO
/**
 * \Get the signature row information pointer
 * @return the pointer
 */
const u8 *get_signature_row_data(u8 *len_ptr)
{
	// Revision ID
	sigbuf[0] = FLASH_ReadWord(REVISION_ID_ADDRESS);

	// Device ID
	sigbuf[1] = FLASH_ReadWord(DEVICE_ID_ADDRESS);

	if (len_ptr) {
		*len_ptr = sizeof(sigbuf);
	}

	return (u8 *)sigbuf;
}

#endif