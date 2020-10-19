/*
 * T18.c
 *
 * Created: 6/9/2019 10:44:13 AM
 *  Author: A41450
 */ 
#ifdef OBJECT_T18

#include "arch/tslapi.h"
#include "txx.h"

t18_data_t t18_data_status;
ssint object_t18_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	return object_txx_init(&t18_data_status, rid, def, mem, cb);
}

void t18_set_unsupport_area(object_t18_t *mem)
{
	//mem->cmd = 0;
}

void t18_data_sync(const txx_data_t *ptr, u8 rw)
{
	object_t18_t *mem = (object_t18_t *)ptr->mem;
	
	// Not paramenter to hw
		
	t18_set_unsupport_area(mem);
}

void object_t18_start(u8 loaded)
{
	t18_data_t *ptr = &t18_data_status;
	
	t18_data_sync(ptr, loaded ? OP_WRITE : OP_READ);
}

void object_t18_data_sync(u8 rw)
{
	t18_data_t *ptr = &t18_data_status;

	t18_data_sync(ptr, rw);
}

bool object_t18_check_retrigger(void)
{
	t18_data_t *ptr = &t18_data_status;
	object_t18_t *mem = (object_t18_t *)ptr->mem;
	
	if(mem->cmd & MXT_COMMS_CTRL_RETRIGEN)
		return true;
	
	return false;
}

bool object_t18_check_dismntr(void)
{
	t18_data_t *ptr = &t18_data_status;
	object_t18_t *mem = (object_t18_t *)ptr->mem;
	
	if(mem->ctrl & MXT_COMMS_CTRL_DISMNTR)
		return true;
	
	return false;
}
#endif