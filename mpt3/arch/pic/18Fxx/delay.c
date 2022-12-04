
#include <common/include/types.h>
#include "arch/cpu.h"
#include "arch/delay.h"

void delay_us_n(u8 n)
{
	while(n--) {
		__delay_us(1);
	}
}

void delay_ms_n(u8 n)
{
	while(n--) {
		__delay_ms(1);
	}
}