/*
 * gpio.c
 *
 * Created: 12/2/2022 9:32:18 AM
 *  Author: A41450
 */ 

 #include <common/include/types.h>
 #include "gpio.h"

#define DECLARE_GPIO_SET_PARAM0_FUNCTION(_ACTION) \
void gpio_##_ACTION (const u8 port, const u8 pin)	\
{	\
	switch(port) {	\
		case GPIOA:	\
		  PORTA_##_ACTION(pin);	\
		break;	\
		case GPIOB:	\
		  PORTB_##_ACTION(pin);	\
		break;	\
		case GPIOC:	\
		  PORTC_##_ACTION(pin);	\
		break;	\
		case GPIOD:	\
			PORTD_##_ACTION(pin);	\
		break;	\
		case GPIOE:	\
			PORTE_##_ACTION(pin);	\
		break;	\
		case GPIOF:	\
			PORTF_##_ACTION(pin);	\
		break;	\
		case GPIOG:	\
			PORTF_##_ACTION(pin);	\
		break;	\
		default:	\
			;	\
	}	\
}
DECLARE_GPIO_SET_PARAM0_FUNCTION(toggle_pin_level)

#define DECLARE_GPIO_SET_PARAM1_FUNCTION(_ACTION, _NAME, _TYPE) \
void gpio_set_pin_##_NAME (const u8 port, const u8 pin, const _TYPE _NAME)	\
{	\
	switch(port) {	\
		case GPIOA:	\
		    PORTA_##_ACTION##_##_NAME(pin, _NAME);	\
		break;	\
		case GPIOB:	\
		    PORTB_##_ACTION##_##_NAME(pin, _NAME);	\
		break;	\
		case GPIOC:	\
		    PORTC_##_ACTION##_##_NAME(pin, _NAME);	\
		break;	\
		case GPIOD:	\
			PORTD_##_ACTION##_##_NAME(pin, _NAME);	\
		break;	\
		case GPIOE:	\
			PORTE_##_ACTION##_##_NAME(pin, _NAME);	\
		break;	\
		case GPIOF:	\
			PORTF_##_ACTION##_##_NAME(pin, _NAME);	\
		break;	\
		case GPIOG:	\
			PORTF_##_ACTION##_##_NAME(pin, _NAME);	\
		break;	\
		default:	\
		;	\
	}	\
}
DECLARE_GPIO_SET_PARAM1_FUNCTION(set_pin, pull_mode, PORT_PULL_MODE_t)
DECLARE_GPIO_SET_PARAM1_FUNCTION(pin_set, inverted, bool)
DECLARE_GPIO_SET_PARAM1_FUNCTION(pin_set, isc, PORT_ISC_t)
DECLARE_GPIO_SET_PARAM1_FUNCTION(set_pin, dir, PORT_DIR_t)
DECLARE_GPIO_SET_PARAM1_FUNCTION(set_pin, level, bool)

#define DECLARE_GPIO_GET_PARAM0_FUNCTION(_ACTION) \
u8 gpio_##_ACTION (const u8 port, const u8 pin)	\
{	\
	switch(port) {	\
		case GPIOA:	\
		    return PORTA_##_ACTION(pin);	\
		break;	\
		case GPIOB:	\
		    return PORTB_##_ACTION(pin);	\
		break;	\
		case GPIOC:	\
		    return PORTC_##_ACTION(pin);	\
		break;	\
		case GPIOD:	\
			return PORTD_##_ACTION(pin);	\
		break;	\
		case GPIOE:	\
			return PORTE_##_ACTION(pin);	\
		break;	\
		case GPIOF:	\
			return PORTF_##_ACTION(pin);	\
		break;	\
		case GPIOG:	\
			return PORTF_##_ACTION(pin);	\
		break;	\
		default:	\
			return (u8)(-1);	\
	}	\
}
DECLARE_GPIO_GET_PARAM0_FUNCTION(get_pin_level)
