/*
 * gpio.h
 *
 * Created: 12/2/2022 9:32:29 AM
 *  Author: A41450
 */ 


#ifndef GPIO_H_
#define GPIO_H_

#include <port.h>

typedef enum port_pull_mode PORT_PULL_MODE_t;
typedef enum port_dir PORT_DIR_t;

typedef enum PORT {
	GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, NUM_GPIOS
} PORT_T;
	
void gpio_set_pin_pull_mode(const uint8_t port, const uint8_t pin, const PORT_PULL_MODE_t pull_mode);
void gpio_set_pin_inverted(const uint8_t port, const uint8_t pin, const bool inverted);
void gpio_set_pin_isc(const uint8_t port, const uint8_t pin, const PORT_ISC_t isr);
void gpio_set_pin_dir(const uint8_t port, const uint8_t pin, const PORT_DIR_t dir);
void gpio_set_pin_level(const uint8_t port, const uint8_t pin, const bool level);
void gpio_toggle_pin_level(const uint8_t port, const uint8_t pin);

u8 gpio_get_pin_level(const uint8_t port, const uint8_t pin);

#endif /* GPIO_H_ */