/**
 * \file
 *
 * \brief ADC Basic driver implementation.
 *
 (c) 2018 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms,you may use this software and
    any derivatives exclusively with Microchip products.It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/**
 * \defgroup doc_driver_adc_basic ADC Basic Driver
 * \ingroup doc_driver_adc
 *
 * \section doc_driver_adc_rev Revision History
 * - v0.0.0.1 Initial Commit
 *
 *@{
 */
#include "adc_basic.h"

/**
 * \brief Initialize ADC interface
 * If module is configured to disabled state, the clock to the ADC is disabled
 * if this is supported by the device's clock system.
 *	@reg: module address pointer
		@ref: the positive reference source
		@count: how many times of adc accumulating 
		@crs: right shift in control 2
		@acq: acquisition time
 * \return Initialization status.
 * \retval 0 the ADC init was successful
 * \retval 1 the ADC init was not successful
 */
int8_t ADC_init(ADC_t *reg, adc_ref_t ref, uint8_t count, uint8_t shift_r, uint8_t acq, uint8_t clk_src)
{
	ADCON0 = ADCON0_ADFM_RIGHT_JUSTIFIED	// Data left aligned --- for 8-bits result
			|clk_src /* ADCON0_ADCS_CLK_FOSC, ADCON0_ADCS_CLK_FRC */		// Clock source
			|ADCON0_ADCONT_TRIGGER_ONCE	// Non-countinous mode
			|ADCON0_ADON_ENABLE;		// Enabled

	ADACT = ADC_ADACT_DISABLED;	// Disable auto conversion trigger

	ADCLK = ADC_ADCLK_ADCS_DIV_N(XTAL_FREQ_MHZ);	// FCLK Div to 1Mhz when `ADCON0_ADCS_CLK_FOSC` is set(maximum clock for ADC)

	ADREF = ref;

	ADCON1 = ADCON1_ADDSEN_NONE // Single sampling
		| ADCON1_ADGPOL_LOW;	// DS level low

	ADCON2 = ADCON2_ADMD_BURST_AVG_MODE /*ADCON2_ADMD_BASIC_MODE*/		// AVG Burst mode
		| ADCON2_ADACLR_CLEAR		// Clear the result
		| shift_r		// Result right shift
		| ADCON2_ADPSIS_ADRES;	// Using Res as Prev

	ADCON3 = ADCON3_ADTMD_IRQ_NONE;	// Disable IRQ

	ADACQ = acq;	// Acq time to default

	ADCAP = ADCAP_COMPENSATION_NONE;	// Compensation Zero

	ADPRE = ADC_ADPRE_NONE; // Precharge to default

	// ADPCH = ADC_ADPCH_VSS;	// Target sampling channel

	ADRPT = count;	// Repeat <accu> times

	ADCNT = 0;

	return 0;
}

/**
 * \brief Enable ADC_0
 * 1. If supported by the clock system, enables the clock to the ADC
 * 2. Enables the ADC module by setting the enable-bit in the ADC control register
 *
 * \return Nothing
 */
void ADC_enable(ADC_t *reg)
{
	ADCON0bits.ADON = 1;
}
/**
 * \brief Disable ADC_0
 * 1. Disables the ADC module by clearing the enable-bit in the ADC control register
 * 2. If supported by the clock system, disables the clock to the ADC
 *
 * \return Nothing
 */
void ADC_disable(ADC_t *reg)
{
	ADCON0bits.ADON = 0;
}

/**
 * \brief Start a conversion on ADC_0
 *
 * \param[in] channel The ADC channel to start conversion on
 *
 * \return Nothing
 */
void ADC_start_conversion(ADC_t *reg, adc_channel_t channel)
{
	ADCON2bits.ADACLR = 1;
	while(ADCON0bits.ADON && ADCON2bits.ADACLR);

	ADPCH  = channel;

	ADCON0bits.ADGO = 1;
}

/**
 * \brief Check if the ADC conversion is done
 *
 * \return The status of ADC converison done check
 * \retval true The ADC conversion is done
 * \retval false The ADC converison is not done
 */
bool ADC_is_conversion_done(ADC_t *reg)
{
	/* COMMAND must be clear before result will be valid */
	return ((!(ADCON0bits.ADGO || ADSTATbits.ADSTAT)) && ADSTATbits.ADMATH);
}

/**
 * \brief Read a conversion result from ADC_0
 *
 * \return Conversion result read from the ADC_0 ADC module
 */
adc_result8_t ADC_get_conversion_result(ADC_t *reg)
{
	return ADFLTRL;
}

void ADC_clear_accumulator(ADC_t *reg)
{
    //Reset the ADCON2bits.ADACLR bit.
		ADCON2bits.ADACLR = 1;
}

/**
 * \brief Start a conversion, wait until ready, and return the conversion result
 *
 * \return Conversion result read from the ADC_0 ADC module
 */
adc_result8_t ADC_get_conversion(ADC_t *reg, adc_channel_t channel)
{
	adc_result8_t res;

	ADC_start_conversion(reg, channel);
	while (!ADC_is_conversion_done(reg))
		;
	res = ADC_get_conversion_result(reg);
	return res;
}

/**
 * \brief Return the number of bits in the ADC conversion result
 *
 * \return The number of bits in the ADC conversion result
 */
uint8_t ADC_get_resolution(ADC_t *reg)
{
	return 8;
}