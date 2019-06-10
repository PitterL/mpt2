/*
 * mutex.h
 *
 * Created: 6/8/2019 9:31:03 AM
 *  Author: A41450
 */ 


#ifndef MUTEX_H_
#define MUTEX_H_

#include "types.h"

typedef struct mutex {
	s8 bits;
	u8 flag;
} mutex_t;



void mutex_init (mutex_t *mtx);
int lock(mutex_t *mtx);
void unlock(mutex_t *mtx);

#endif /* MUTEX_H_ */