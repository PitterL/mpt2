/*
 * types.h
 *
 * Created: 6/7/2019 11:27:39 PM
 *  Author: A41450
 */ 


#ifndef OS_TYPES_H_
#define OS_TYPES_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifndef u8
typedef uint8_t u8;
#endif

#ifndef u16
typedef uint16_t u16;
#endif

#ifndef u32
typedef uint32_t u32;
#endif

#ifndef u64
typedef uint64_t u64;
#endif

#ifndef s8
typedef int8_t s8;
#endif

#ifndef s16
typedef int16_t s16;
#endif

#ifndef s32
typedef int32_t s32;
#endif

#ifndef s64
typedef int64_t s64;
#endif

#ifndef ssint
typedef s8 ssint;
#endif

#ifndef uint
typedef unsigned int uint;
#endif

typedef union {
	struct {
		u8 lo: 4;
		u8 hi: 4;
	};
	u8 value;
} nibble_t;

typedef union {
	u8 byte[8];
	u64 value;
} __attribute__ ((packed)) uu64_t;

#define VALID_PTR(_ptr) ((_ptr) && (size_t)(_ptr) != ERROR_PTR)

#ifdef __GNUC__
#include <utils.h>
#include <arch/utils/compiler_included.h>
#else
#include <arch/utils/utils.h>
#include <arch/utils/compiler_included.h>
#endif

#include <stdbool.h>

#endif /* TYPES_H_ */