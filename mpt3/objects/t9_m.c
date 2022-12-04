/*
 * t9_m.c
 *
 * Created: 10/22/2022 9:38:09 AM
 *  Author: A41450
 */ 
#ifdef OBJECT_T9_DUMMY

#ifdef OBJECT_T9
#error "Error OBJECT_T9 is defined before dummy"
#endif

#include <string.h>
#include "txx.h"

t9_data_t t9_data_status;

ssint object_t9_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb)
{
	return object_txx_init(&t9_data_status, rid, def, mem, cb);
}

void t9_set_unsupport_area(t9_data_t *ptr)
{
	qtouch_config_t *qdef = (qtouch_config_t *)ptr->def;
	object_t9_t *mem = (object_t9_t *) ptr->mem;
	
	mem->xorigin = 0;
	mem->xsize = QT_MATRIX_X_SIZE(qdef);
	mem->yorigin = 0;
	mem->ysize = QT_MATRIX_Y_SIZE(qdef);

	mem->ctrl = MXT_T9_CTRL_ENABLE;
}

void object_t9_start(u8 loaded)
{

}

void object_t9_report_status(u8 force)
{

}

void object_t9_data_sync(u8 rw)
{
	t9_data_t *ptr = &t9_data_status;

	t9_set_unsupport_area(ptr);
}

#endif