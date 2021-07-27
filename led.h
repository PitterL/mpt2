/*
 * led.h
 *
 * Created: 7/19/2021 5:11:24 PM
 *  Author: A41450
 */ 


#ifndef LED_H_
#define LED_H_

void leds_init(void);
void button_led_state_change(uint8_t groupid, uint32_t status);
void slider_led_state_change(uint8_t groupid, uint8_t fingerid, uint8_t status, uint16_t x, uint16_t y, uint16_t max_resol);

#endif /* LED_H_ */