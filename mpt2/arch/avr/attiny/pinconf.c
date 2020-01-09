/*
 * pinconf.c
 *
 * Created: 12/11/2019 1:27:27 PM
 *  Author: A41450
 */ 

#include <string.h>
#include <atmel_start_pins.h>
#include <adc_basic2.h>
#include <port.h>
#include <utils.h>
#include <clock_config.h>
#include <util/delay.h>
#include "arch/cpu.h"
#include "arch/pinconf.h"

typedef struct ptc_pin_map{
	unsigned ptc_channel: 4;
	unsigned port: 4;
	unsigned pin: 3;
	unsigned adc: 1;
	unsigned adc_channel: 4;
} ptc_pin_map_t;

enum { GPIOA, GPIOB, GPIOC, GPIOD};

ptc_pin_map_t attiny_xx17_ptc_pin_map[] = {
	{	/*ptc_channel*/0, /*port*/GPIOA, /*pin*/4, /*adc*/1, /*adc channel*/0	},
	{	/*ptc_channel*/1, /*port*/GPIOA, /*pin*/5, /*adc*/1, /*adc channel*/1	},
	{	/*ptc_channel*/2, /*port*/GPIOA, /*pin*/6, /*adc*/1, /*adc channel*/2	},
	{	/*ptc_channel*/3, /*port*/GPIOA, /*pin*/7, /*adc*/1, /*adc channel*/3	},
	{	/*ptc_channel*/4, /*port*/GPIOB, /*pin*/1, /*adc*/0, /*adc channel*/10	},
	{	/*ptc_channel*/5, /*port*/GPIOB, /*pin*/0, /*adc*/0, /*adc channel*/11	},
	{	/*ptc_channel*/6, /*port*/GPIOC, /*pin*/0, /*adc*/1, /*adc channel*/6	},
	{	/*ptc_channel*/7, /*port*/GPIOC, /*pin*/1, /*adc*/1, /*adc channel*/7	},
	{	/*ptc_channel*/8, /*port*/GPIOC, /*pin*/2, /*adc*/1, /*adc channel*/8	},
	{	/*ptc_channel*/9, /*port*/GPIOC, /*pin*/3, /*adc*/1, /*adc channel*/9	},
	{	/*ptc_channel*/10, /*port*/GPIOC, /*pin*/4, /*adc*/1, /*adc channel*/10	},
	{	/*ptc_channel*/11, /*port*/GPIOC, /*pin*/5, /*adc*/1, /*adc channel*/11	},
	{	/*ptc_channel*/12, /*port*/GPIOB, /*pin*/5, /*adc*/0, /*adc channel*/8	},
	{	/*ptc_channel*/13, /*port*/GPIOB, /*pin*/4, /*adc*/0, /*adc channel*/9	},
};

#define PTC_CHANNLE_COUNT ARRAY_SIZE(attiny_xx17_ptc_pin_map)

/* Node configure parameters */
extern qtm_acq_t321x_node_config_t ptc_seq_node_cfg1[];

/* Acquisition set 1 - General settings */
extern qtm_acq_node_group_config_t ptc_qtlib_acq_gen1;

#define DECLARE_GPIO_SET_FUNCTION(_ACTION, _NAME, _TYPE) \
void gpio_set_pin_##_NAME (const uint8_t port, const uint8_t pin, const _TYPE _NAME)	\
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
		default:	\
		;	\
	}	\
}

DECLARE_GPIO_SET_FUNCTION(set_pin, pull_mode, enum port_pull_mode)
DECLARE_GPIO_SET_FUNCTION(pin_set, inverted, bool)
DECLARE_GPIO_SET_FUNCTION(pin_set, isc, PORT_ISC_t)
DECLARE_GPIO_SET_FUNCTION(set_pin, dir, enum port_dir)
DECLARE_GPIO_SET_FUNCTION(set_pin, level, bool)

void ptc_disable(void)
{
	*(uint8_t *)(&ADC0 + 1) = 0;
}

bool ptc_channel_used(uint8_t channel)
{
	uint8_t i;

	for ( i = 0; i < ptc_qtlib_acq_gen1.num_sensor_nodes; i++) {
		if (((ptc_seq_node_cfg1[i].node_xmask | ptc_seq_node_cfg1[i].node_ymask) >> channel) & 0x1) {
			return true;
		}
	}

	return false;
}

#ifdef USE_MPTT_WRAPPER
void touch_ptc_pin_config(void)
{
	const ptc_pin_map_t *ptc_map = attiny_xx17_ptc_pin_map;
	uint8_t i;

	for ( i = 0; i < PTC_CHANNLE_COUNT; i++) {
		if (ptc_channel_used(ptc_map[i].ptc_channel)) {
			gpio_set_pin_pull_mode(ptc_map[i].port, ptc_map[i].pin, PORT_PULL_OFF);
			gpio_set_pin_isc(ptc_map[i].port, ptc_map[i].pin, PORT_ISC_INPUT_DISABLE_gc);
			//FIXME: set input in default ?
			gpio_set_pin_dir(ptc_map[i].port, ptc_map[i].pin, PORT_DIR_IN);
		}
	}
}
#endif

#define delay_us(_v)	_delay_us(_v)
#define delay_ms(_v)	_delay_ms(_v)

uint16_t gpio_get_adc_value(uint8_t adc, ADC_MUXPOS_t channel, uint8_t vrshift)
{
	uint8_t delay = 50;
	ADC_t *reg;

	if (adc == 0)
		reg = &ADC0;
	else
		reg = &ADC1;

	ADC_start_conversion(reg, channel);
	do {
		if (ADC_is_conversion_done(reg))
			break;

		delay_us(10);

	} while (--delay);

	return (uint16_t)(ADC_get_conversion_result(reg) >> vrshift);
}

extern void tsl_suspend(uint8_t suspend);
void pinfault_test_init(void)
{
	tsl_suspend(1);

	ptc_disable();

	ADC_init(&ADC0, ADC_REFSEL_VDDREF_gc, ADC_SAMPNUM_ACC16_gc, ADC_RESSEL_8BIT_gc);
	ADC_init(&ADC1, ADC_REFSEL_VDDREF_gc, ADC_SAMPNUM_ACC16_gc, ADC_RESSEL_8BIT_gc);
}

bool pinfault_test_cycle(uint8_t delay,uint8_t thld, bool walk, bool level, uint8_t *test_pin, uint8_t *pin_val)
{
	const ptc_pin_map_t *ptc_map = attiny_xx17_ptc_pin_map;
	uint8_t i;
	uint16_t val;
	bool result = true;

	// Set all pins status
	for ( i = 0; i < PTC_CHANNLE_COUNT; i++) {
		if (ptc_channel_used(ptc_map[i].ptc_channel)) {
			gpio_set_pin_dir(ptc_map[i].port, ptc_map[i].pin, PORT_DIR_OUT);
			gpio_set_pin_level(ptc_map[i].port, ptc_map[i].pin, walk ? !level : level);
			gpio_set_pin_isc(ptc_map[i].port, ptc_map[i].pin, PORT_ISC_INTDISABLE_gc);
		}
	}

	delay_ms(10);

	for ( i = 0; i < PTC_CHANNLE_COUNT; i++) {
		if (ptc_channel_used(ptc_map[i].ptc_channel)) {
			if (walk) {
				if (level) {
					gpio_set_pin_dir(ptc_map[i].port, ptc_map[i].pin, PORT_DIR_IN);
					gpio_set_pin_pull_mode(ptc_map[i].port, ptc_map[i].pin, PORT_PULL_UP);
				} else {
					gpio_set_pin_level(ptc_map[i].port, ptc_map[i].pin, false);	//FIXME: No Pull down mode in Tiny chip, use driver instead
				}
			}

			delay_us(delay);
			val = (uint8_t)gpio_get_adc_value(ptc_map[i].adc, ptc_map[i].adc_channel, 4);
			
			if (walk) {
				gpio_set_pin_dir(ptc_map[i].port, ptc_map[i].pin, PORT_DIR_OUT);
				gpio_set_pin_level(ptc_map[i].port, ptc_map[i].pin, walk ? !level : level);
			}

			if ((level && val > thld) || (!level && val < 256 - thld)) {
				result = true;
			} else {
				if (test_pin)
					*test_pin = ptc_map[i].ptc_channel;

				if (pin_val)
					*pin_val = val;
				result = false;
				break;
			}
		}
	}

	return result;
}

void pinfault_test_end(void)
{
	ADC_disable(&ADC0);
	ADC_disable(&ADC1);
	
	touch_ptc_pin_config();

	tsl_suspend(0);
}

uint8_t avdd_test(void)
{
	uint16_t val;

	VREF.CTRLC = VREF_ADC1REFSEL_1V5_gc;

	ADC_init(&ADC1, ADC_REFSEL_VDDREF_gc, ADC_SAMPNUM_ACC16_gc, ADC_RESSEL_8BIT_gc);
	/* 
		Vmeasured = adcval * Vref /256 (8bit)
			=> (1) adcval = Vmeasured * 256 /Vref
				(2) Vref = Vmeasured *256 /adcval
			
		adcval = du / Vref£º
	
		we use default internal VREF (voltage 0.55v) as measure target, VDD as Ref. 
		default VREF = 0.55v, for 8 bit sampling, numerator is du = 0.55 *256 = 140.8
		du <40, 46> => 3.52 ~ 3.06v

		we use 2.5v VREF as measure target, VDD as Ref.
		default VREF = 2.5v, for 8 bit sampling, numerator is du = 2.5 *256 = 640
		du <181, 209> => 3.52 ~ 3.06v
		
	*/

	val = (uint8_t)gpio_get_adc_value(1, ADC_MUXPOS_INTREF_gc, 4);
	ADC_disable(&ADC1);

	// standard 116
	if (val > 100 && val < 130)	// 2.9v ~3.8v
		return 0;

	return val;
}

uint8_t pinfault_test(uint8_t delay,uint8_t thld, uint8_t *test_pin, uint8_t *test_val)
{
	uint8_t result;
	uint8_t seq = 0;

	pinfault_test_init();

	//Driven ground
	result = pinfault_test_cycle(delay, thld, 0, 0, test_pin, test_val);
	if (!result) {
		seq = 1;
	} else {
		//Driven high
		result = pinfault_test_cycle(delay, thld, 0, 1, test_pin, test_val);
		if (!result) {
			seq = 2;
		} else {
			//Walk 1
			result = pinfault_test_cycle(delay, thld, 1, 1, test_pin, test_val);
			if (!result) {
				seq = 3;
			} else {
				//Walk 0
				result = pinfault_test_cycle(delay, thld, 1, 0, test_pin, test_val);
				if (!result)
					seq = 4;
			}
		}
	}

	pinfault_test_end();

	return seq;
}

void gpio_assert_chg(void)
{
	CHG_set_level(0);
	CHG_set_dir(PORT_DIR_OUT);
}

void gpio_release_chg(void)
{
	CHG_set_pull_mode(PORT_PULL_UP);
	CHG_set_dir(PORT_DIR_IN);
}