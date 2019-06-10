/*
 * led_driver.h
 *
 * Created: 18-09-2017 11:45:02
 *  Author: I41681
 */

#ifndef LED_DRIVER_H_
#define LED_DRIVER_H_

#include "touch.h"

#define ENABLE_LED 0u

#if ENABLE_LED == 1u

#include "i2c_master.h"

#define SLAVE_ADDR 0x20

#define LED_HOR 0
#define LED_VER 1

void init_led_driver(void);
void led_gpio_update(uint8_t data, uint8_t ver_or_hor);
void led_decode_position(void);
void led_reset(void);

#endif

#endif /* LED_DRIVER_H_ */
