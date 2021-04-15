/*
 * crc.h
 *
 * Created: 6/7/2019 9:48:20 PM
 *  Author: A41450
 */ 


#ifndef CRC_H_
#define CRC_H_

typedef union data_crc24 {
	struct {
		u8 data[3];
		u8 rsv;
	};
	u32 value;
} __attribute__ ((packed)) data_crc24_t;

typedef struct crc_data_blocks {
	const u8 *base;
	size_t size;
} crc_data_blocks_t;

u8 calc_crc8(const u8 *base, size_t size);
u32 calc_blocks_crc24(const crc_data_blocks_t *blocks, size_t count);
u32 calc_crc24(const u8 *base, size_t size);

#endif