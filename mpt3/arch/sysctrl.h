/*
 * sysctrl.h
 *
 * Created: 12/11/2019 1:31:49 PM
 *  Author: A41450
 */ 


#ifndef ARCH_SYSCTRL_H_
#define ARCH_SYSCTRL_H_

typedef enum sleep_mode {
	SLEEP_NONE = 0,
	SLEEP_DOZE,
	SLEEP_DEEP,
} sleep_mode_t;

void sys_reset(void);
void sys_clr_wdt(void);
uint16_t sys_get_wdt_period(void);
void sys_sleep(sleep_mode_t m);

#endif /* ARCH_SYSCTRL_H_ */