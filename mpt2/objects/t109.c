/*
 * t109.c
 *
 * Created: 6/16/2019 2:48:07 AM
 *  Author: A41450
 */ 


#include "../mptt.h"
#include "../tslapi.h"
#include "t109.h"

t109_data_t t109_data_status;
ssint object_t109_init(u8 rid,  const /*sensor_config_t*/void *cfg, void *mem, void *cb)
{
	return object_txx_init(&t109_data_status, rid, cfg, mem, cb);
}
