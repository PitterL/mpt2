/*
 * t38.c
 *
 * Created: 3/18/2021 10:41:00 AM
 *  Author: A41450
 */ 
#ifdef OBJECT_T38
#include <string.h>
#include "txx.h"

t38_data_t t38_data_status;

ssint object_t38_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	t38_data_t *t38_ptr = &t38_data_status;

	object_txx_init(&t38_ptr->common, rid, def, mem, cb);

	return 0;
}

void object_t38_start(void) 
{

}

#endif