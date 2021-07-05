/*
 * txx.c
 *
 * Created: 6/15/2019 5:26:46 PM
 *  Author: A41450
 */ 

#include <string.h>
#include "arch/tslapi.h"
#include "txx.h"

ssint object_txx_init(txx_data_t *ptr, u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{	
	
	ptr->rid = rid;
	ptr->def = def;
	ptr->mem = mem;
	ptr->cb = cb;
	
	return 0;
}

void object_txx_report_msg(const txx_data_t *ptr, const void *data, u8 size)
{
#ifdef OBJECT_T5
	object_t5_t message;

	memset(&message, 0, sizeof(message));
	
	if (size > sizeof(message.data))
		size = sizeof(message.data);

	message.reportid = ptr->rid;
	memcpy(message.data, data, size);

	MPT_API_CALLBACK(ptr->cb, write_message)(&message);
#endif
}

void object_txx_op(const txx_data_t *ptr, const txx_cb_param_t *params, u8 count, u8 index, u8 rw)
{
	u8 i;
	
	for (i = 0; i < count; i++) {
		MPT_QTAPI_CALLBACK(ptr->cb, sync)(params[i].type, params[i].src, params[i].size, index, rw);
	}
}

void object_txx_readback(const txx_data_t *ptr, const txx_cb_param_t *params, u8 count, u8 index)
{
	object_txx_op(ptr, params, count, index, OP_READ);
}

void object_txx_writeback(const txx_data_t *ptr, const txx_cb_param_t *params, u8 count, u8 index)
{
	object_txx_op(ptr, params, count, index, OP_WRITE);
}

bool object_ts_state_idle(const txx_data_t *ptr)
{
	return MPT_QTAPI_CALLBACK(ptr->cb, idle)();
}

void object_ts_suspend(const txx_data_t *ptr, bool suspend)
{
	MPT_QTAPI_CALLBACK(ptr->cb, suspend)(suspend);
}