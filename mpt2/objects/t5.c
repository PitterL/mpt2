/*
 * t5.c
 *
 * Created: 6/9/2019 10:24:52 AM
 *  Author: A41450
 */ 
#include <string.h>
#include "t5.h"

t5_data_t t5_data_status;
ssint object_t5_init(u8 rid,  const /*sensor_config_t*/void *cfg, void *mem, void *cb)
{
	t5_data_t *ptr = &t5_data_status;
	memset(ptr, 0, sizeof(*ptr));
	
	ptr->mem = (object_t5_t *)mem;
	ptr->mem->reportid = MXT_RPTID_NOMSG;	/* Initialized no message by Reset */
	
	return 0;
}