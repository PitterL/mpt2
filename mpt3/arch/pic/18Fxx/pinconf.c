/*+
 * pinconf.c
 *
 * Created: 12/11/2019 1:27:27 PM
 *  Author: A41450
 */ 

#include <common/include/types.h>

#include <pin_manager.h>
#include "arch/cpu.h"
#include "arch/delay.h"
#include "arch/tslink.h"
#include "arch/pinconf.h"
#include <mtouch/lib/tslib.h>
#include "drivers/adc_basic.h"

#define DECLARE_GPIO_SET_FUNCTION(_ACTION, _NAME, _TYPE) \
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
		default:	\
		;	\
	}	\
}

DECLARE_GPIO_SET_FUNCTION(set_pin, pull_mode, PORT_PULL_MODE_t)
DECLARE_GPIO_SET_FUNCTION(pin_set, inverted, bool)
DECLARE_GPIO_SET_FUNCTION(pin_set, isc, PORT_ISC_t)
DECLARE_GPIO_SET_FUNCTION(set_pin, dir, PORT_DIR_t)
DECLARE_GPIO_SET_FUNCTION(set_pin, level, bool)

/**
 * Current ADC node list
 * 
 */
u8 uNodelist[MAXIMUM_ADC_CHANNEL_NODES];
u8 uNodeSize;

#define GET_NODE_IN_NODELIST(_i)	((_i) < uNodeSize ? uNodelist[(_i)] : AN_NONE)
#define PUT_NODE_TO_NODELIST(_nd)	(uNodeSize < MAXIMUM_ADC_CHANNEL_NODES ? (uNodelist[uNodeSize++] = (_nd)) : 0)
#define GET_NODELIST_SIZE()	(uNodeSize)
#define INITIALIZE_NODELIST()	(uNodeSize = 0)
#define IS_NODELIST_NULL() (uNodeSize == 0)

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

/**
 * @brief the walk nodes callback
 * @node: current node walked
 * @param: pointer of the input parameters
 * @return ssint: Zero: continue to walk; Other value: stop to walk
 */
typedef ssint (*walk_nodes_cb_t)(u8 node, void *param);

/**
 * @brief Walking all the channel nodes
 * 
 * @param cb: the walk callback
 * @param param: the parameter point to the cb
 * @return ssint: Zero: walk done, other value: callback stop to walk and the returned value
 */
ssint walk_nodes(walk_nodes_cb_t cb, void *param)
{
	const u8 num_channel_nodes = tslink_get_number_channel_nodes();
	u8 i, j, node;
	ssint result = 0;

	for ( i = 0; i < num_channel_nodes; i++ ) {
		for ( j = 0; j < NUM_NODE_TYPE; j++ ) {
			node = GET_SEQ_NODE_CFG_ni(i, nodes)[j];
			result = cb(node, param);

			if (result != 0) {
				return result;
			}
		}
	}

	return result;
}

void config_node(u8 node, PORT_DIR_t dir, s8 level)
{
	u8 port = TO_AN_PORT(node);
	u8 pin = TO_AN_PIN(node);

	if (level < 0) {
		// Set input
		gpio_set_pin_dir(port, pin, PORT_DIR_IN);
		gpio_set_pin_pull_mode(port, pin, PORT_PULL_UP);
	} else {
		// Output with level

		// gpio_set_pin_inverted(port, pin, false);
		gpio_set_pin_pull_mode(port, pin, PORT_PULL_OFF);
		gpio_set_pin_isc(port, pin, PORT_ISC_INPUT_DISABLE_gc);

		//set ouput 0 in default
		gpio_set_pin_level(port, pin, level);
		gpio_set_pin_dir(port, pin, dir);
	}
}

ssint cfg_node_wrapper(u8 node, void *param)
{
	s8 level = *(s8 *)param;
	
	config_node(node, PORT_DIR_OUT, level);

	return 0;
}

/*! \brief configure the adc port pins to Input by automatic way
	The function will decide which pin should be configured when used
 */
#ifdef MPTT_AUTO_PINMUX
void touch_sensor_pin_config(void)
{
	bool level= false;

	walk_nodes(cfg_node_wrapper, &level);
}
#endif

ssint check_node_used_wrapper(u8 node, void *param)
{
	u8 target_node = *(u8 *)param;

	return (ssint)(node == target_node);
}

/**
 * @brief create the channle node list, it will store in a global buffer
 * 
 */
void adc_nodelist_create(void)
{
	u8 i, j, node;

	INITIALIZE_NODELIST();

	for (i = 0, j = 0; i < MAXIMUM_ADC_CHANNEL_NODES; i++) {
		node = ADC_CHANNEL(i);
		if (walk_nodes(check_node_used_wrapper, &node) != 0) {
			PUT_NODE_TO_NODELIST(node);
		}
	}
}

/**
 * @brief measure the adc value of dedicated channel
 * 
 * @param reg : module address pointer
 * @param channel : the test channel
 * @param wait : acquisition time
 * @return u8 adc result
 */
u8 gpio_get_adc_value(ADC_t *reg, adc_channel_t channel, uint8_t wait)
{
	// Clear buffer
	ADC_clear_accumulator(reg);

	ADC_start_conversion(reg, channel);
	do {
		if (ADC_is_conversion_done(reg))
			break;

#define ADC_CONVERSION_TIME_US 0
		delay_us(ADC_CONVERSION_TIME_US) ;

	} while (--wait);

	return ADC_get_conversion_result(reg);
}

/**
 * @brief the pinfault module initialize
 * 
 */
void pin_config_init(void)
{	
	adc_nodelist_create();
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
#define FVR_SETUP_TIME_US 25
	if (FVRCONbits.ADFVR != ADFVR_GAIN_2 || !FVRCONbits.FVREN) {
		FVRCONbits.ADFVR = ADFVR_GAIN_2;
		FVRCONbits.FVREN = 1;
		delay_us(FVR_SETUP_TIME_US);
	}

	/* Waiting for FVR ready */
	while((!FVRCONbits.FVRRDY) && FVRCONbits.FVREN);

	ADC_init(&ADC_BASE, ADREF_ADPREF_VDD, 4, ADCON2_ADCRS_RSHIFT_4, ADACQ_ACQUISITION_FOSC_2us, ADCON0_ADCS_CLK_FOSC);	// 8 samples,  for 8-bits result, shift 4(2+2)

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

	val = gpio_get_adc_value(&ADC_BASE, ADC_ADPCH_FVR/*ADC_ADPCH_VSS*/, 255);
	
	ADC_disable(&ADC_BASE);

	/* Turn off FVR */ 
	// FVRCONbits.ADFVR = 0;

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
	u8 node, port, pin, val;
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
		walk_nodes(cfg_node_wrapper, &level_set);
	}

	node = GET_NODE_IN_NODELIST(idx);
	if (NODE_UNUSED(node)) {
		return -1;
	}

	port = TO_AN_PORT(node);
	pin = TO_AN_PIN(node);

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

	val = gpio_get_adc_value(&ADC_BASE, node, 255);
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

/**
 * @brief pinfault test as input parameters
 * 
 * @param buf: input parameters' buffer
 * @param len: parameters size
 * @return ssint: Zero: test complete, other value failed
 */
ssint api_pinfault_test(u8 *buf, u8 len)
{
	u8 seq, idx, thld, delay, count, val, nodes_count;
	s8 done = 0;
	bool reinit = true;
    
	ssint result = 1;

	seq = sTestdata.sequence;
	idx = sTestdata.index;
	thld = sTestdata.threshold;
	delay = sTestdata.delay;
	count = sTestdata.count;
	val = 0;
	nodes_count = GET_NODELIST_SIZE();

	if (seq < DRIVEN_GROUND || seq >= PIN_FAULT_TEST_END) {
		seq = DRIVEN_GROUND;
	}

	if (idx >= nodes_count) {
		idx = 0;
	}

	ADC_init(&ADC_BASE, ADREF_ADPREF_VDD, 4, ADCON2_ADCRS_RSHIFT_4, ADACQ_ACQUISITION_N(delay), ADCON0_ADCS_CLK_FOSC);	// 4 samples, for 8-bits result, shift 4(2+2)

	do {
		// If count is zero, there will do all pins test
		count--;
		
		result = pinfault_test_cycle(seq, idx, thld, delay, &val, reinit);
		if (result) {
			done = -1;
			break;	// fault found
		}

		idx++;
		reinit = false;
		if (idx >= nodes_count) {
			seq++;	//Next sequence
			if (seq >= PIN_FAULT_TEST_END) {
				done = 1;
				break;	// test end
			}
			idx = 0;
			reinit = true;
		}
	} while(count);

	ADC_disable(&ADC_BASE);

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
    CHG_SetLow();
    CHG_SetDigitalOutput();
}

void gpio_release_chg(void)
{
    CHG_SetPullup();
    CHG_SetDigitalInput();
		// CHG_SetOpenDrain();
}

void gpio_toggle_chg(void)
{
	CHG_SetDigitalOutput();
	CHG_Toggle();
}

bool gpio_get_bus_line_level(void)
{
	return SDA_GetValue() && SCL_GetValue();
}