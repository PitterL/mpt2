/*+
 * pinconf.c
 *
 * Created: 12/11/2019 1:27:27 PM
 *  Author: A41450
 */ 

#include <common/include/types.h>
#include "arch/cpu.h"
#include "arch/delay.h"
#include "arch/tslink.h"
#include "arch/pinconf.h"
#include "qtouch/lib/tslib.h"
#include "drivers/adc_basic.h"
#include "drivers/gpio.h"
#include <board.h>

/**
 * Current ADC node list
 * 
 */
uu64_t uNode_mask;

#define INITIALIZE_NODELIST() (uNode_mask.value = 0)
#define GET_NODE_MASK_VALUE()	(uNode_mask.value)
#define GET_NODE_MASK_V_n(n)	(uNode_mask.byte[n])
#define SET_NODE_MASK(_new_mask)	(uNode_mask.value = (_new_mask))

/**
 * \save and clear ADC register before test 
 */
void adc_disable(void)
{	
	// FIXME: this is not a good method to operation HW level directly
}

/**
 * \recover ADC register after test 
 */
void adc_enable(void)
{
	/* disable after test finished */
}

/**
 * \reset ADC register 
 */
void adc_reset(void)
{
    /* reset */
}

void _config_pin(u8 port, u8 pin, PORT_DIR_t dir, bool level)
{
	// Cancel inverted/pull/isc
	gpio_set_pin_inverted(port, pin, false);
	gpio_set_pin_pull_mode(port, pin, PORT_PULL_OFF);
	gpio_set_pin_isc(port, pin, PORT_ISC_INPUT_DISABLE_gc);

	// Set ouput level
	gpio_set_pin_level(port, pin, level);
	
	// Set direction
	gpio_set_pin_dir(port, pin, dir);
}

void config_pin(u8 port, u8 pin, PORT_DIR_t dir, bool level)
{
	if (port < NUM_GPIOS && pin < 8) {
		_config_pin(port, pin, dir, level);
	}
}

void config_pin_id(u8 id, PORT_DIR_t dir, bool level)
{
    _config_pin(id >> 3, id & 0x7, dir, level);
}

void config_all_pins(PORT_DIR_t dir, bool level)
{
	u8 i;
    /*
    u8 j;
	for (i = 0; i < (MAXIMUM_ADC_CHANNEL_NODES / 8) ; i++) {
		for (j = 0; j < 8; j++) {
			if (TEST_BIT(GET_NODE_MASK_V_n(i), j)) {
				config_pin(i, j, PORT_DIR_IN, false);
			}
		}
	}
    */
	for (i = 0; i < MAXIMUM_ADC_CHANNEL_NODES; i++) {
       if (TEST_BIT(GET_NODE_MASK_VALUE(), i)) {
        config_pin_id(i, PORT_DIR_IN, false);
       } 
    }

}

/*! \brief configure the adc port pins to Input by automatic way
	The function will decide which pin should be configured when used
 */
#ifdef MPTT_AUTO_PINMUX
void touch_sensor_pin_config(void)
{
	config_all_pins(PORT_DIR_IN, false);
}
#endif

/**
 * @brief create the channle node list, it will store in a global buffer
 * 
 */

void adc_create_pinmap(void)
{
	u8 i;
	uu64_t ports, nodes;

	/* Get all nodes masks */
	nodes.value = 0;
  for ( i = 0; i < GET_ACQ_NODE_GRP_CFG_i(num_channel_nodes); i++ ) {
		nodes.value |= GET_SEQ_NODE_CFG_ni(i, node_xmask) | GET_SEQ_NODE_CFG_ni(i, node_ymask);
	}
	
	/* nodes to gpios map */
    ports.value = 0;

	// GPIOA: XY[0:7]
	ports.byte[GPIOA] = nodes.byte[0];

	// GPIOB: XY[8:15]
	ports.byte[GPIOB] = nodes.byte[1];

	// GPIOC: null
    // ports_m.byte[GPIOC] = 0;

	// GPIOD: XY[16:23]
	ports.byte[GPIOD] = nodes.byte[2];

	// GPIOE: XY[24: 32]
	ports.byte[GPIOE] = nodes.byte[3];

	// GPIOF: XY[32: 37]
	ports.byte[GPIOF] = nodes.byte[4]/* & 0x3F*/;

	// GPIOG: XY[40: 47]
	ports.byte[GPIOG] = nodes.byte[5];

	SET_NODE_MASK(ports.value);
}

/**
 * @brief measure the adc value of dedicated channel
 * 
 * @param reg : module address pointer
 * @param channel : the test channel
 * @param wait : acquisition time
 * @return u8 adc result
 */
uint8_t gpio_get_adc_value(ADC_MUXPOS_t channel)
{
  uint8_t delay = 255;
	adc_result_t value = 0;

	// Clear the result buffer
	// ADC_0_get_conversion_result();

	ADC_0_start_conversion(channel);
	do {
		if (ADC_0_is_conversion_done())
			break;

		delay_us(10);
	} while (--delay);

	if (delay) {
		value = ADC_0_get_conversion_result();
		if (ADC_0_get_resolution() == 10) {
			return (value >> 2) & 0xFF;
		} else {
			return (value >> 4) & 0xFF;
		}
	}

	return 0;
}

/**
 * @brief the pinfault module initialize
 * 
 */
void pin_config_init(void)
{	
	adc_create_pinmap();
}


/**
 * \brief Pin fault content
 * @v: the output value of tested
 * @Return: 0 successful, other value mean failed 
 */
ssint api_avdd_test(u8 *v)
{
	u8 val;

	/* Enable FVR with 2.048v (+/-4% error) */
	/* VREF.ADC0REF = 0 << VREF_ALWAYSON_bp // Enable reference always ON for DAC0: disabled
			 | VREF_REFSEL_VDD_gc; // VDD as reference */

  ADC_0_init(ADC_SAMPNUM_ACC16_gc, ADC_RESSEL_10BIT_gc, ADC_INITDLY_DLY128_gc);

	/* 
		(1) Vmeasured = Adcval * Vref /Resolution
			=> (2) Acval = Vmeasured * Resolution /Vref
					(3) Vref = Vmeasured * Resolution /Adcval
		
		Resolution is 256 (8 bits)
		As Vmeasured is VREF(FVR), so we could simple to Adcval = du / Vref (which du = Vmeasured * 256):
	
		we use 1.024v VREF as measure target, VDD as Ref.
		du = 1.024 * 256 = 262
		du <68, 89> => 3.84 ~ 2.95v

		we use 2.048v VREF as measure target, VDD as Ref.
		du = 2.048 * 256 = 524
		du <136, 178> => 3.84 ~ 2.95v
		
		Now, Select VREF = 2.048v:
		Check range <136, 178>		
	*/

	val = gpio_get_adc_value(VREF_REFSEL_2V048_gc);
	
	ADC_0_disable();

#define AVDD_LOW (136)
#define AVDD_HIGH (178)

	if (v) {
		*v = val;
	}

	if (val > AVDD_LOW && val < AVDD_HIGH) {	// 2.95v ~ 3.84v
		return 0;
	} else {
		return -3;
	}
}

/**
 * \brief Pin fault content
 * use ADC to check value after config the pin mode
 * @seq: the test type sequence
 * @idx: test node index in node list
 * @thld: the adc threshold for pass
 * @delay: delay time before each adc sampling
 * @test_val: output the adc result
 * @reinit: whether do all pins initilization
 * @return: 0 mean pass, other failed
 */

ssint pinfault_test_cycle(const u8 seq, const u8 idx, const u8 thld, const u8 delay, u8 *test_val, bool reinit)
{
	u8 port, pin, val;
	bool walk, level, level_set;
	ssint result = 0;

	switch (seq) {
		case DRIVEN_GROUND:
			walk = level = false;
			break;
		case DRIVEN_HIGH:
			walk = false;
			level = true;
			break;
		case WALKING_1:
			walk = level = true;
			break;
		case WALKING_0:
			walk = true;
			level = false;
			break;
		default:
			walk = level = false;
	}

	// Set all pins status
	if (reinit) {
		level_set = walk ? !level : level;
		config_all_pins(PORT_DIR_OUT, level_set);
	}

	port = idx >> 3;
	pin = idx & 0x7;
	if (walk) {
		if (level) {
			gpio_set_pin_dir(port, pin, PORT_DIR_IN);
			gpio_set_pin_pull_mode(port, pin, PORT_PULL_UP);
		} else {
			gpio_set_pin_level(port, pin, false);	//FIXME: No Pull down mode in Tiny chip, use driver instead
		}
	}

#ifdef __OPTIMIZE__
// Table 39-10. I/O and CLKOUT Timing Specifications, 70ns maxium
#define GPIO_SETUP_TIME_US 2
	delay_us(GPIO_SETUP_TIME_US);
#endif

	if (port >= GPIOD && port <= GPIOF) {
		val = gpio_get_adc_value((ADC_MUXPOS_t)(idx - (GPIOD << 3)));
	} else {
		val = gpio_get_pin_level(port, pin) ? (u8)(-1) : 0;
	}
	// Recover the pin config
	if (walk) {
		gpio_set_pin_dir(port, pin, PORT_DIR_OUT);
		gpio_set_pin_level(port, pin, walk ? !level : level);
	}

	if ((level && val > thld) || (!level && val < 256 - thld)) {
		result = 0;
	} else {
		if (test_val)
			*test_val = val;
		result = -2;
	}

	return result;
}

pinfault_test_setting_t sTestdata;
/**
 * @brief set the pinfault test parameters
 * 
 * @param buf the parameter pointer
 * @param len parameters size
 * @return ssint result
 */
ssint api_pinfault_set_param(u8 *buf, u8 len)
{
	if (buf && len) {
		memcpy(&sTestdata, buf, min(len, sizeof(sTestdata)));
	}

	return 0;
}

/**
 * @brief request or release adc resource
 * 
 * @param request : request or release
 * @param len : buf size
 * @return ssint result, 0 indicated success
 */
ssint api_request_adc_resource(bool request)
{
	ssint result = -1;

	if (request) {
		if (tslink_touch_state_idle()) {
			if (tslink_touch_adc_deinit()) {
				tslink_touch_suspend(true);
				result = 0;
			}
		}
	} else {
		if (tslink_touch_adc_init()) {
			tslink_touch_suspend(false);
			result = 0;
		}
	}

	return result;
}

u8 next_idx(u8 idx)
{
	u8 i;

	for (i = idx; i < MAXIMUM_ADC_CHANNEL_NODES; i++) {
		if (TEST_BIT(GET_NODE_MASK_VALUE(), i)) {
			return i;
		}

		i++;
	}

	return MAXIMUM_ADC_CHANNEL_NODES;
}

/**
 * @brief pinfault test as input parameters
 * 
 * @param buf: input parameters' buffer
 * @param len: parameters size
 * @return ssint: Zero: test complete, other value failed
 */
ssint api_pinfault_test(u8 *buf, u8 len)
{
	u8 seq, idx, thld, delay, count, val;
	s8 done = 0;
	bool reinit = true;
    
	ssint result = 1;

	seq = sTestdata.sequence;
	idx = sTestdata.index;
	thld = sTestdata.threshold;
	delay = sTestdata.delay;
	count = sTestdata.count;
	val = 0;

	if (seq < DRIVEN_GROUND || seq >= PIN_FAULT_TEST_END) {
		seq = DRIVEN_GROUND;
	}

	if (idx >= MAXIMUM_ADC_CHANNEL_NODES) {
		idx = next_idx(0);
	}

	ADC_0_init(ADC_SAMPNUM_ACC16_gc, ADC_RESSEL_10BIT_gc, ADC_INITDLY_DLY128_gc);

	do {
		if (idx >= MAXIMUM_ADC_CHANNEL_NODES) {
			seq++;	//Next sequence
			if (seq >= PIN_FAULT_TEST_END) {
				done = 1;
				break;	// test end
			}
			idx = next_idx(0);
			reinit = true;
		} else {
			result = pinfault_test_cycle(seq, idx, thld, delay, &val, reinit);
			if (result) {
				done = -1;
				break;	// fault found
			}

			idx = next_idx(idx + 1);
			reinit = false;
		
			// If count is zero or 255, there will do all pins test with all sequences
			if (count != 255) {
				count--;
			}
		}
	} while(count);

	ADC_0_disable();

	sTestdata.sequence = seq;
	sTestdata.index = idx;
	sTestdata.count = count;
	sTestdata.value = val;
	sTestdata.done = done;

	if (buf && len) {
		memcpy(buf, &sTestdata, min(len, sizeof(sTestdata)));
	}

	touch_sensor_pin_config();

	return result;
}

void gpio_assert_chg(void)
{
	gpio_set_pin_level(CHG_PORT, CHG_PIN, false);
	gpio_set_pin_dir(CHG_PORT, CHG_PIN, PORT_DIR_OUT);
}

void gpio_toggle_chg(void)
{
	gpio_toggle_pin_level(CHG_PORT, CHG_PIN);
	gpio_set_pin_dir(CHG_PORT, CHG_PIN, PORT_DIR_OUT);
}

void gpio_release_chg(void)
{
	gpio_set_pin_pull_mode(CHG_PORT, CHG_PIN, PORT_PULL_UP);
	gpio_set_pin_dir(CHG_PORT, CHG_PIN, PORT_DIR_IN);
}

void gpio_bus_pin_config(void)
{
	_config_pin(TWI_PORT, TWI_SCL_PIN, PORT_DIR_IN, false);
	_config_pin(TWI_PORT, TWI_SDA_PIN, PORT_DIR_IN, false);

	PORTMUX.TWIROUTEA |= PORTMUX_TWI01_bm;
}

bool gpio_get_bus_line_level(void)
{
  return gpio_get_pin_level(TWI_PORT, TWI_SCL_PIN) && gpio_get_pin_level(TWI_PORT, TWI_SDA_PIN);
}