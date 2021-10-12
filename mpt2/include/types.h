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

#ifndef u8
typedef uint8_t u8;
#endif

#ifndef u16
typedef uint16_t u16;
#endif

#ifndef u32
typedef uint32_t u32;
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

#define VALID_PTR(_ptr) ((_ptr) && (size_t)(_ptr) != ERROR_PTR)

/* Note: Below operation is not atomic, using in notice the mutex in multi thread */
#if defined(UTILS_COMPILER_H_INCLUDED)
#define SET_BIT(_x, _bit) Set_bits((_x), (1 << (_bit)))
#define CLR_BIT(_x, _bit) Clr_bits((_x), (1 << (_bit)))
#define TEST_BIT(_x, _bit) Tst_bits((_x), (1 << (_bit)))
#else
#define SET_BIT(_x, _bit) ((_x) |= (1 << (_bit)))
#define CLR_BIT(_x, _bit) ((_x) &= ~(1 << (_bit)))
#define TEST_BIT(_x, _bit) ((_x) & (1 << (_bit)))
#endif
#define BIT(_bit) (1 << (_bit))
#define SET_AND_CLR_BIT(_x, _sbit, _cbit) (SET_BIT((_x), (_sbit)), CLR_BIT((_x), (_cbit)))

#define BIT32(_bit) ((u32)1 << (_bit))

#define L8_TO_LT16(__v0, __v1) ((((short)(__v1)) << 8) | ((short)(__v0)))
#define L16_TO_LT32(__v0, __v1) ((((short)(__v1)) << 16) | ((short)(__v0)))

#ifdef __GNUC__
#include <utils.h>
#else
/**
 * \brief Retrieve array size
 */
#define ARRAY_SIZE(_a) (sizeof(_a) / sizeof(_a[0]))
#endif

#include <stdbool.h>

#endif /* TYPES_H_ */