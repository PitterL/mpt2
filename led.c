/*
 * Led.c
 *
 * Created: 7/19/2021 5:10:53 PM
 *  Author: A41450
 */ 

#include <stdbool.h>
#include <port.h>
#include <utils/utils.h>
#include <arch/pinconf.h>

typedef struct GPIO_STATE_LIST {
	PORT_T port;
	uint8_t pin;
	bool level;
} GPIO_STATE_LIST_T;

#ifdef OBJECT_T15_USE_STATE_CB
#define NUM_BUTTON_LEDS ARRAY_SIZE(button_led_gpios)
static GPIO_STATE_LIST_T button_led_gpios[] = {
	// Button 0
	{GPIOB, 4, false}, 
	
	// Button 1
	//{GPIOA, 1, false},	//Invalid, used by I2C interface	
};


static void init_button_led(void)
{
	const GPIO_STATE_LIST_T *gpio = button_led_gpios;
	uint8_t i;
	
	for ( i = 0; i < NUM_BUTTON_LEDS; i++ ) {
		gpio_set_pin_level(gpio[i].port, gpio[i].pin, !gpio[i].level);
		gpio_set_pin_dir(gpio[i].port, gpio[i].pin, PORT_DIR_OUT);
	}
}

void button_led_state_change(uint8_t groupid, uint32_t status)
{
	const GPIO_STATE_LIST_T *gpio = button_led_gpios;
	uint8_t i;
	bool level;

	for ( i = 0; i < NUM_BUTTON_LEDS; i++ ) {
		level = ((status >> i) & 0x1) ?  gpio[i].level : !gpio[i].level;
		gpio_set_pin_level(gpio[i].port, gpio[i].pin, level);
	}
}
#endif

#ifdef OBJECT_T9_USE_STATE_CB
#define NUM_SLIDER_LEDS ARRAY_SIZE(slider_led_gpios)
static GPIO_STATE_LIST_T slider_led_gpios[] = {
	{GPIOC, 0, false},
	{GPIOC, 1, false},
	{GPIOC, 2, false},
	{GPIOC, 3, false},
	//{GPIOA, 2, false},	//Invalid, used by CHG Line
	{GPIOB, 7, false},
};

static void init_slider_led(void)
{
	const GPIO_STATE_LIST_T *gpio = slider_led_gpios;
	uint8_t i;
	
	for (i = 0; i < NUM_SLIDER_LEDS; i++) {
		gpio_set_pin_level(gpio[i].port, gpio[i].pin, !gpio[i].level);
		gpio_set_pin_dir(gpio[i].port, gpio[i].pin, PORT_DIR_OUT);
	}
}

void slider_led_state_change(uint8_t groupid, uint8_t fingerid, uint8_t status, uint16_t x, uint16_t y, uint16_t max_resol)
{
	const GPIO_STATE_LIST_T *gpio = slider_led_gpios;
	
	uint8_t step = max_resol / ARRAY_SIZE(slider_led_gpios);
	uint16_t acc = 0;
	uint8_t i;

	// Y axis: most left 255, most right 0, so reverse y here
	y = max_resol - y;

	for  (i = 0; i < ARRAY_SIZE(slider_led_gpios); i++) {
		if ((status & 0x80) && acc < y) {	//Pressed, light the LED
			gpio_set_pin_level(gpio[i].port, gpio[i].pin, gpio[i].level);
		} else {
			gpio_set_pin_level(gpio[i].port, gpio[i].pin, !gpio[i].level);
		}
		
		acc += step;
	}
}
#endif

#if defined(OBJECT_T15_USE_STATE_CB) || defined(OBJECT_T9_USE_STATE_CB)
void leds_init(void) 
{
#ifdef OBJECT_T15_USE_STATE_CB
	init_button_led();
#endif
#ifdef OBJECT_T9_USE_STATE_CB
	init_slider_led();
#endif
}
#endif

