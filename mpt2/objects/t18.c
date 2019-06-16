/*
 * T18.c
 *
 * Created: 6/9/2019 10:44:13 AM
 *  Author: A41450
 */ 


#include "../mptt.h"
#include "../tslapi.h"
#include "t18.h"

t18_data_t t18_data_status;
ssint object_t18_init(u8 rid,  const /*sensor_config_t*/void *cfg, void *mem, void *cb)
{
	return object_txx_init(&t18_data_status, rid, cfg, mem, cb);
}

void t18_set_unsupport_area(object_t18_t *mem)
{
	mem->cmd = 0;
}

void object_t18_process(void)
{
	t18_data_t *ptr = &t18_data_status;
	object_t18_t *mem = (object_t18_t *)ptr->mem;
	
	t18_set_unsupport_area(mem);
}

bool object_t18_check_retrigger(void)
{
	t18_data_t *ptr = &t18_data_status;
	object_t18_t *mem = (object_t18_t *)ptr->mem;
	
	if(mem->cmd & MXT_COMMS_CTRL_RETRIGEN)
		return true;
	
	return false;
}