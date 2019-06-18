/*
 * txx.c
 *
 * Created: 6/15/2019 5:26:46 PM
 *  Author: A41450
 */ 

#include "../tslapi.h"
#include "txx.h"

ssint object_txx_init(txx_data_t *ptr, u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{	
	
	ptr->rid = rid;
	ptr->def = def;
	ptr->mem = mem;
	ptr->cb = cb;
	
	return 0;
}

/*
void object_txx_op(const txx_data_t *ptr, const txx_cb_param_t *params, u8 count, u8 index, u8 rw)
{
	cb_writeback_t cb_op;
	u8 i;
	
	if (rw) {
		cb_op = QTOUCH_API_CALLBACK(ptr->cb, read);
	}else {
		cb_op = QTOUCH_API_CALLBACK(ptr->cb, write);
	}
	
	for (i = 0; i < count; i++) {
		cb_op(params[i].type, params[i].src, params[i].size, index);
	}
}*/

void object_txx_op(const txx_data_t *ptr, const txx_cb_param_t *params, u8 count, u8 index, u8 rw)
{
	u8 i;
	
	for (i = 0; i < count; i++) {
		MPT_QTAPI_CALLBACK(ptr->cb, sync)(params[i].type, params[i].src, params[i].size, index, rw);
	}
}

void object_txx_readback(const txx_data_t *ptr, const txx_cb_param_t *params, u8 count, u8 index)
{
	object_txx_op(ptr, params, count, index, 1);
}

void object_txx_writeback(const txx_data_t *ptr, const txx_cb_param_t *params, u8 count, u8 index)
{
	object_txx_op(ptr, params, count, index, 0);
}