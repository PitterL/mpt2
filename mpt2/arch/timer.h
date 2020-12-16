/*
 * pinconf.h
 *
 * Created: 12/11/2019 1:31:49 PM
 *  Author: A41450
 */ 

#ifndef ARCH_TOUCH_H_
#define ARCH_TOUCH_H_

#ifdef OBJECT_T25
extern uint8_t qtlib_suspend;
#endif

void touch_start(void);

#endif /* ARCH_TOUCH_H_ */
