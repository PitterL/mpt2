/*
 * fuse.h
 *
 * Created: 7/12/2021 5:31:19 PM
 *  Author: A41450
 */ 


#ifndef FUSE_H_
#define FUSE_H_

#ifdef MPTT_FUSE_CHECK
ssint fuse_check(void);
const u8 *get_fuse_data(u8 *len_ptr);
#endif

#ifdef OBJECT_T37_DEBUG_PLATFORM_INFO
#define SIGNATURE_ROW_LENGTH 0x26

const u8 *get_signature_row_data(u8 *len_ptr);
#endif

#endif /* FUSE_H_ */