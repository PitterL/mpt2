/*
 * Fuse.c
 *
 * Created: 7/12/2021 5:26:29 PM
 *  Author: A41450
 */ 

#include <port.h>
#include <pack.h>

#include "include/types.h"
#include "arch/fuse.h"

#ifdef MPTT_FUSE_CHECK
static u8 fuse_target[] = FUSES_CONTENT;

/**
 * \Check the fuse whether mathed in current runtime fw and target code, the FUSE content is set in pack.h, and will be packed into firmware by `cupdi` command
 * @return: Zero mean pass, otherwise failed
 */
ssint fuse_check(void)
{
	const u8 *fv;

	for (u8 i = 0; i < ARRAY_SIZE(fuse_target); i++) {
		fv = (const u8 *)&FUSE;
		if (fv[i] != fuse_target[i])
			return -1;
	}
	
	return 0;
	
}
#endif

#ifdef OBJECT_T37_DEBUG_PLATFORM_INFO
/**
 * \Get the signature row information pointer
 * @return the pointer
 */
const u8 *get_signature_row_data(u8 *len_ptr)
{	
	if (len_ptr) {
		*len_ptr = SIGNATURE_ROW_LENGTH;
	}
	return (const u8 *)&SIGROW;
}

#endif