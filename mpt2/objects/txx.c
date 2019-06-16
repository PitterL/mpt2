/*
 * txx.c
 *
 * Created: 6/15/2019 5:26:46 PM
 *  Author: A41450
 */ 

#include "../mptt.h"
#include "../tslapi.h"
#include "txx.h"

ssint object_txx_init(txx_data_t *ptr, u8 rid,  const /*sensor_config_t*/void *cfg, void *mem, void *cb)
{	
	sensor_config_t *scfg = (sensor_config_t *)cfg;
	
	ptr->mem = mem;
	ptr->cb_write = cb;
	
	ptr->rid = rid;
	ptr->matrix_xsize = scfg->matrix_xsize;
	ptr->matrix_ysize = scfg->matrix_ysize;
	
	return 0;
}

void object_txx_process(const txx_data_t *ptr, const txx_cb_param_t *params, u8 count, u8 index)
{
	cb_writeback_t cb_write = ptr->cb_write;
	u8 i;
	
	for (i = 0; i < count; i++) {
		cb_write(params[i].type, params[i].src, params[i].size, index);
	}
}