/*
 * pinconf.h
 *
 * Created: 12/11/2019 1:31:49 PM
 *  Author: A41450
 */ 


#ifndef ARCH_PINCONFIG_H_
#define ARCH_PINCONFIG_H_

typedef struct pinfault_test_setting {
	u8 sequence;
	u8 index;	// the node index in node list
	u8 threshold;
	u8 delay;
	u8 count;
	u8 value;
	s8 done;	// 0: in progress; 1: completion; -1: fail
} pinfault_test_setting_t;

enum PIN_FAULT_SEQ_CODE {
	DRIVEN_GROUND = 1,
	DRIVEN_HIGH,
	WALKING_1,
	WALKING_0,
	PIN_FAULT_TEST_END,
	HIGH_VOLTAGE = 0x7,
};

void pin_config_init(void);
ssint api_request_adc_resource(bool request);
ssint api_avdd_test(u8 *v);
ssint api_pinfault_set_param(u8 *buf, u8 len);
ssint api_pinfault_test(u8 *buf, u8 len);

void gpio_assert_chg(void);
void gpio_toggle_chg(void);
void gpio_release_chg(void);

void touch_sensor_pin_config(void);

void gpio_bus_pin_config(void);
bool gpio_get_bus_line_level(void);

#endif /* ARCH_PINCONFIG_H_ */