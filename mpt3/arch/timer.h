/*
 * pinconf.h
 *
 * Created: 12/11/2019 1:31:49 PM
 *  Author: A41450
 */ 

#ifndef ARCH_TOUCH_H_
#define ARCH_TOUCH_H_

void touch_start(void);
void touch_process(void);
void touch_done(void);

void Timer_set_callback(void (* handler)(void));
void Timer_set_period(const uint16_t val, const bool unused);

#endif /* ARCH_TOUCH_H_ */
