/*
 * t5.c
 *
 * Created: 6/9/2019 10:24:52 AM
 *  Author: A41450
 */
#ifdef OBJECT_T5

#include "t5.h"

t5_data_t t5_data_status;
ssint object_t5_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	t5_data_t *ptr = &t5_data_status;
	
	ptr->mem = (object_t5_t *)mem;
	ptr->mem->reportid = MXT_RPTID_NOMSG;	/* Initialized no message by Reset */
	
	return 0;
}

#endif