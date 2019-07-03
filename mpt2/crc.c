/*
 * crc.c
 *
 * Created: 6/7/2019 9:48:06 PM
 *  Author: A41450
 */ 
#include "types.h"
#include "crc.h"

/*
calculate one byte input value with CRC 8 Bit
    @crc: last crc value
    @data: data input
    @returns calculated crc value
*/
u8 crc8(u8 crc, u8 data)
{
    static const u8 crcpoly = 0x8C;
    u8 index;
    u8 fb;
    index = 8;
    
    do
    {
        fb = (crc ^ data) & 0x01;
        data >>= 1;
        crc >>= 1;
        if (fb)
            crc ^= crcpoly;
    } while (--index);

    return crc;
}

/*
Calculate buffer with crc8
    @base: buffer input
    @size: data size
    @returns calculated crc value
*/
u8 calc_crc8(const u8 *base, size_t size)
{
    u8 crc = 0;
    const u8 *ptr = base;
    const u8 *last_val = base + size - 1;

    while (ptr <= last_val) {
        crc = crc8(crc, *ptr);
        ptr++;
    }

    return crc;
}

/*
calculate two byte input value with CRC 24 Bit
    @crc: last crc value
    @firstbyte: byte 1
    @secondbyte: byte 2
    @returns calculated crc value
*/
u32 crc24(u32 crc, u8 firstbyte, u8 secondbyte)
{
    const u32 crcpoly = 0x80001B;
    u32 data_word;

    data_word = ((u16)secondbyte << 8) | firstbyte;
    crc = ((crc << 1) ^ data_word);

    if (crc & 0x1000000)
        crc ^= crcpoly;

    return crc;
}

/*
Calculate multi block crc24
    @base: buffer input
    @size: data size
    @returns calculated crc value, only bit[0~23] is valid
*/
u32 calc_blocks_crc24(const crc_data_blocks_t *blk, size_t count)
{
	u8 firstbyte, secondbyte, odd = 0;
	u32 crc = 0;
	size_t i, j, sum;
	const u8 *ptr;
	
	for (i = 0, sum = 0; i < count; i++) {
		ptr = blk[i].base;
		
		for (j = 0; j + 1 < blk[i].size; j += 2) {
			if (sum & 0x1) {
				firstbyte = odd;
				secondbyte = *ptr++;
				odd = *ptr++;
			}else {
				firstbyte = *ptr++;
				secondbyte = *ptr++;
			}
			
			crc = crc24(crc, firstbyte, secondbyte);
		}

		if ((blk[i].size + sum) & 0x1) {	//	sum is odd
			if (blk[i].size & 0x1)	//	only odd will left one
				odd = *ptr;
			
			if (i == count - 1) {	//last block
				/* if len is odd, fill the last byte with 0 */
				crc = crc24(crc, odd, 0);
			}
		}else {
			if (sum & 0x1) {	//sum is even, but both blocks are odd
				crc = crc24(crc, odd, *ptr);		
			}
		}
		
		sum += blk[i].size;
	}
	
    /* Mask to 24-bit */
    crc &= 0x00FFFFFF;

    return crc;
}

/*
Calculate buffer with crc24
    @base: buffer input
    @size: data size
    @returns calculated crc value, only bit[0~23] is valid
*/
u32 calc_crc24(const u8 *base, size_t size)
{
	crc_data_blocks_t block = {base, size};
	
    return calc_blocks_crc24(&block, 1);
}