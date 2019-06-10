/*
 * mutex.c
 *
 * Created: 6/8/2019 9:31:20 AM
 *  Author: A41450
 */ 

#include <string.h>
#include <atomic.h>
#include "mutex.h"

void mutex_init(struct mutex *mtx) 
{
	memset(mtx, 0, sizeof(*mtx));
}

int lock(struct mutex *mtx) 
{
	if (!mtx->bits) {
		//ENTER_CRITICAL(mtx->flag);
	}
	
	mtx->bits++;
	
	return 0;
}

void unlock(struct mutex *mtx)
{
	mtx->bits--;

	if (!mtx->bits) {	
		//EXIT_CRITICAL(mtx->flag);
	}
}