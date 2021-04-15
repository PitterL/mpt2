/*
 * mutex.h
 *
 * Created: 6/8/2019 9:31:03 AM
 *  Author: A41450
 */ 


#ifndef MUTEX_H_
#define MUTEX_H_
#include <xc.h> // include processor files - each processor file is guarded.  

#define ENTER_CRITICAL(P)  bool P = GIE; GIE = 0;
#define EXIT_CRITICAL(P)  GIE = P;

#define DISABLE_INTERRUPTS()   GIE = 0;
#define ENABLE_INTERRUPTS()    GIE = 1;

#define LOCK() ENTER_CRITICAL(_irqsave);

#define UNLOCK() EXIT_CRITICAL(_irqsave);

#endif /* MUTEX_H_ */