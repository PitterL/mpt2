/*
 * t109.c
 *
 * Created: 6/16/2019 2:48:07 AM
 *  Author: A41450
 */ 


#include "../tslapi.h"
#include "../mptt.h"
#include "t109.h"

t109_data_t t109_data_status;
ssint object_t109_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	return object_txx_init(&t109_data_status, rid, def, mem, cb);
}
