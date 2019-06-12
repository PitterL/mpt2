/*
 * t9.c
 *
 * Created: 6/9/2019 9:38:09 AM
 *  Author: A41450
 */ 

#include <string.h>
#include "../mptt.h"
#include "t9.h"

t9_data_t t9_status_data[MXT_TOUCH_MULTI_T9_RIDS];

int object_t9_init(u8 rid)
{
	t9_data_t *ptr = t9_status_data;
	int i;
	
	for ( i = 0; i < MXT_TOUCH_MULTI_T9_RIDS; i++) {
		ptr->rid = rid + i;
	}
	
	return 0;
}

int object_t9_start(void)
{
	return 0;
}

void t9_report_status(const t9_data_t *ptr)
{
	object_t5_t message;
	
	memset(&message, 0, sizeof(message));
		
	message.reportid = ptr->rid;
	message.data[0] = ptr->status ? MXT_T9_DETECT : 0;
	message.data[1] = (ptr->pos.x >> 4);
	message.data[2] = (ptr->pos.y >> 4);
	message.data[3] = ((ptr->pos.x & 0xF) << 4) | (ptr->pos.y & 0xF);
		
	mpt_write_message(&message);
}

void object_t9_report_status(void)
{
	t9_data_t *ptr = t9_status_data;
	int i;
	
	for (i = 0; i < MXT_TOUCH_MULTI_T9_RIDS; i++) {
		t9_report_status(ptr + i);
	}
}

int t9_set_pointer_location(u8 id, uint8_t status, uint16_t x, uint16_t y)
{
	t9_data_t *ptr = t9_status_data;

	if (id >= MXT_TOUCH_MULTI_T9_RIDS)
		return -2;
	
	ptr = &t9_status_data[id];
	if (ptr->status != status || ptr->pos.x != x || ptr->pos.y != y) {	
		ptr->status = status;
		ptr->pos.x = x;
		ptr->pos.y = y;
		
		t9_report_status(ptr);
	}
	
	return 0;
}