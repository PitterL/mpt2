/*
 * pinconf.h
 *
 * Created: 12/11/2019 1:31:49 PM
 *  Author: A41450
 */ 


#ifndef ARCH_BUS_H_
#define ARCH_BUS_H_

enum {
	BUS_STOP = 0,	//Default is STOP status, this is not real STOP signal at BUS
	BUS_READ,
	BUS_WRITE,
	BUS_COLLISION,
	BUS_ERROR,
	NUM_BUS_STATES
};

typedef struct bus_monitor {
	size_t counter[NUM_BUS_STATES];
	union {
		struct {
			u8 low;
			u8 high;
		} bytes;
		u8 val[2];
		u16 value;
	} regaddr;

	u8 state;

	/* current bit heart ticks: 1ms unit */
	u8 current;
} bus_monitor_t;

void bus_state_change(u8 state);
void bus_assert_irq(u8 assert, bool retrigger);

#include "i2c.h"
/* query current bus state */
u8 mptt_get_bus_state(void);

#endif /* ARCH_BUS_H_ */