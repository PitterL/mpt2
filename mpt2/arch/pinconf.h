/*
 * pinconf.h
 *
 * Created: 12/11/2019 1:31:49 PM
 *  Author: A41450
 */ 


#ifndef ARCH_PINCONFIG_H_
#define ARCH_PINCONFIG_H_

uint8_t avdd_test(void);
uint8_t pinfault_test(uint8_t delay,uint8_t thld, uint8_t *test_pin, uint8_t *test_val);

void gpio_assert_chg(void);
void gpio_release_chg(void);

#endif /* ARCH_PINCONFIG_H_ */