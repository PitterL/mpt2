/*
 * mutex.h
 *
 * Created: 6/8/2019 9:31:03 AM
 *  Author: A41450
 */ 


#ifndef MUTEX_H_
#define MUTEX_H_

#include <atomic.h>

#define LOCK() ENTER_CRITICAL(_irqsave);

#define UNLOCK() EXIT_CRITICAL(_irqsave);

#endif /* MUTEX_H_ */