/**
 * \file
 *
 * \brief ADC Basic driver declarations.
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

#ifndef ADC_BASIC_H_INCLUDED
#define ADC_BASIC_H_INCLUDED

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <arch/cpu.h>

typedef void (*adc_irq_cb_t)(void);

/* Datatype for the result of the ADC conversion */
typedef uint8_t adc_result8_t;

/* Analog channel selection */
typedef ADC_ADPCH_t adc_channel_t;

/* Reference source */
typedef ADC_ADREF_t adc_ref_t;


int8_t ADC_init(ADC_t *reg, adc_ref_t ref, uint8_t count, uint8_t shift_r, uint8_t acq, uint8_t clk_src);

void ADC_enable(ADC_t *reg);

void ADC_disable(ADC_t *reg);

void ADC_start_conversion(ADC_t *reg,adc_channel_t channel);

bool ADC_is_conversion_done(ADC_t *reg);

adc_result8_t ADC_get_conversion_result(ADC_t *reg);

void ADC_clear_accumulator(ADC_t *reg);

adc_result8_t ADC_get_conversion(ADC_t *reg,adc_channel_t channel);

uint8_t ADC_get_resolution(ADC_t *reg);

#ifdef __cplusplus
}
#endif

#endif /* ADC_BASIC_H_INCLUDED */
