/*
 * Copyright (C) 2021, Microchip Technology Inc. and its subsidiaries ("Microchip")
 * All rights reserved.
 *
 * This software is developed by Microchip Technology Inc. and its subsidiaries ("Microchip").
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *     1. Redistributions of source code must retain the above copyright notice, this list of
 *        conditions and the following disclaimer.
 *
 *     2. Redistributions in binary form must reproduce the above copyright notice, this list
 *        of conditions and the following disclaimer in the documentation and/or other
 *        materials provided with the distribution. Publication is not required when
 *        this file is used in an embedded application.
 *
 *     3. Microchip's name may not be used to endorse or promote products derived from this
 *        software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MICROCHIP "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL MICROCHIP BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING BUT NOT LIMITED TO
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWSOEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _Q10_IO_H
#define _Q10_IO_H

/* C Language Only */
#if !defined (__ASSEMBLER__)

#include <stdint.h>

typedef volatile uint8_t register8_t;
typedef volatile uint16_t register16_t;
typedef volatile uint32_t register32_t;


#ifdef _WORDREGISTER
#undef _WORDREGISTER
#endif
#define _WORDREGISTER(regname)   \
    __extension__ union \
    { \
        register16_t regname; \
        struct \
        { \
            register8_t regname ## L; \
            register8_t regname ## H; \
        }; \
    }

#ifdef _DWORDREGISTER
#undef _DWORDREGISTER
#endif
#define _DWORDREGISTER(regname)  \
    __extension__ union \
    { \
        register32_t regname; \
        struct \
        { \
            register8_t regname ## 0; \
            register8_t regname ## 1; \
            register8_t regname ## 2; \
            register8_t regname ## 3; \
        }; \
    }


/*
==========================================================================
IO Module Structures
==========================================================================
*/
/*
--------------------------------------------------------------------------
CONFIG - Device Configuration
--------------------------------------------------------------------------
*/
#define CONFIG_BASE *(CONFIG_t *)&CONFIG1

typedef struct CONFIG_struct
{
    register16_t CONFIG1;
    register16_t CONFIG2;
    register16_t CONFIG3;
    register16_t CONFIG4;
    register16_t CONFIG5;
    register16_t CONFIG6;
} CONFIG_t;

/* Configuration Word 1 Oscillators */
typedef enum CONFIG_CONFIG1_enum {
    /* Bit 13 – FCMEN Fail-Safe Clock Monitor Enable bit */
    CONFIG1_FCMEN = (1 << 13),
    /* Bit 11 – CSWEN Clock Switch Enable bit */
    CONFIG1_CSWEN = (1 << 11),
    /* Bit 8 – CLKOUTEN Clock Out Enable bit */
    CONFIG1_CLKOUTEN_DISABLED = (1 << 8),
    /* Bits 6:4 – RSTOSC[2:0] Power-up Default Value for COSC bits */
    CONFIG1_RSTOSC_HFINTOSC_64MHZ = (0 << 4),
    CONFIG1_RSTOSC_EXTOSC_4x = (2 << 4),
    CONFIG1_RSTOSC_SOSC = (4 << 4),
    CONFIG1_RSTOSC_LFINTOSC = (5 << 4),
    CONFIG1_RSTOSC_HFINTOSC_4MHZ = (6 << 4),
    CONFIG1_RSTOSC_EXTOSC = (7 << 4),
    CONFIG1_RSTOSC_MASK = (0X7 << 4),
    /* Bits 2:0 – FEXTOSC[2:0] FEXTOSC External Oscillator Mode Selection bits */
    CONFIG1_FEXTOSC_LP = (0 << 0),
    CONFIG1_FEXTOSC_XT = (1 << 0),
    CONFIG1_FEXTOSC_HS = (2 << 0),
    CONFIG1_FEXTOSC_OSC_DISABLED = (4 << 0),
    CONFIG1_FEXTOSC_ECL = (5 << 0),
    CONFIG1_FEXTOSC_ECM = (6 << 0),
    CONFIG1_FEXTOSC_ECH = (7 << 0),
    CONFIG1_FEXTOSC_MASK = (0x7 << 0)
}CONFIG_CONFIG1_t;

/* Configuration Word 2 Supervisor */
typedef enum CONFIG_CONFIG2_enum {
    /* Bit 15 – XINST Extended Instruction Set Enable bit */
    CONFIG2_XINST_DISABLED = (1 << 15),
    /* Bit 13 – DEBUG Debugger Enable bit */
    CONFIG2_DEBUG_DISABLED = (1 << 13),
    /* Bit 12 – STVREN Stack Overflow/Underflow Reset Enable bit */
    CONFIG2_STVREN_ENABLED = (1 << 12),
    /* Bit 11 – PPS1WAY PPSLOCKED bit One-Way Set Enable bit */
    CONFIG2_PPS1WAY_ENABLED = (1 << 11),
    /* Bit 10 – ZCD ZCD Disable bit */
    CONFIG2_ZCD_DISABLED = (1 << 10),
    /* Bits 9:8 – BORV[1:0] Brown-out Reset Voltage Selection bit */
    CONFIG2_BORV_2V85 = (0 << 8),
    CONFIG2_BORV_2V7 = (1 << 8),
    CONFIG2_BORV_2V45 = (2 << 8),
    CONFIG2_BORV_1V9 = (3 << 8),
    CONFIG2_BORV_MASK = (0x3 << 8),
    /* Bits 7:6 – BOREN[1:0] Brown-out Reset Enable bits When enabled, Brown-out Reset Voltage (VBOR) is set by BORV bit*/
    CONFIG2_BOREN_DISABLED = (0 << 6),
    CONFIG2_BOREN_BY_SBOREN = (1 << 6),
    CONFIG2_BOREN_ACTIVE_ONLY = (2 << 6),
    CONFIG2_BOREN_ENABLED = (3 << 6),
    CONFIG2_BOREN_MASK = (0x3 << 6),
    /* Bit 5 – LPBOREN Low-Power BOR Enable bit */
    CONFIG2_LPBOREN_DISABLED = (1 << 5),
    /* Bit 1 – PWRTE Power-up Timer Enable bit */
    CONFIG2_PWRT_DIABLED = (1 << 1),
    /* Bit 0 – MCLRE Master Clear (MCLR) Enable bit */
    CONFIG2_MCLRE_EN = (1 << 0),
    CONFIG2_MCLRE_PORT = (0 << 0),
} CONFIG_CONFIG2_t;

/* Configuration Word 3 Windowed Watchdog Timer */
typedef enum CONFIG_CONFIG3_enum {
    /* Bits 13:11 – WDTCCS[2:0] WDT Input Clock Selector bits */
    CONFIG3_WDTCCS_LFINTOSC = (0 << 11),
    CONFIG3_WDTCCS_MFINTOSC = (1 << 11),
    CONFIG3_WDTCCS_SOFT_CONTROL = (7 << 11),
    CONFIG3_WDTCCS_MASK = (0x7 << 11),
    /* Bits 10:8 – WDTCWS[2:0] WDT Window Select bits */
    CONFIG3_WDTCWS_WINDOWN_12_5_POINTS = (0 << 8),
    CONFIG3_WDTCWS_WINDOWN_25_POINTS = (1 << 8),
    CONFIG3_WDTCWS_WINDOWN_37_5_POINTS = (2 << 8),
    CONFIG3_WDTCWS_WINDOWN_50_POINTS = (3 << 8),
    CONFIG3_WDTCWS_WINDOWN_62_5_POINTS = (4 << 8),
    CONFIG3_WDTCWS_WINDOWN_75_POINTS = (5 << 8),
    CONFIG3_WDTCWS_WINDOWN_82_5_POINTS = (6 << 8),
    /* Bits 6:5 – WDTE[1:0] WDT Operating Mode bits */
    CONFIG3_WDTE_WDT_DISABLED = (0 << 5),
    CONFIG3_WDTE_WDT_BY_SEN = (1 << 5),
    CONFIG3_WDTE_WDT_ENABLED_IN_ACTIVE = (2 << 5),
    CONFIG3_WDTE_WDT_ENABLED_ALWAYS = (3 << 5),
    /* Bits 4:0 – WDTCPS[4:0] WDT Period Select bits */
    CONFIG3_WDTPS_1MS = (0 << 0),
    CONFIG3_WDTPS_2MS = (1 << 0),
    CONFIG3_WDTPS_4MS = (2 << 0),
    CONFIG3_WDTPS_8MS = (3 << 0),
    CONFIG3_WDTPS_16MS = (4 << 0),
    CONFIG3_WDTPS_32MS = (5 << 0),
    CONFIG3_WDTPS_64MS = (6 << 0),
    CONFIG3_WDTPS_128MS = (7 << 0),
    CONFIG3_WDTPS_256MS = (8 << 0),
    CONFIG3_WDTPS_512MS = (9 << 0),
    CONFIG3_WDTPS_1S = (10 << 0),
    CONFIG3_WDTPS_2S = (11 << 0),
    CONFIG3_WDTPS_4S = (12 << 0),
    CONFIG3_WDTPS_8S = (13 << 0),
    CONFIG3_WDTPS_16S = (14 << 0),
    CONFIG3_WDTPS_32S = (15 << 0),
    CONFIG3_WDTPS_64S = (16 << 0),
    CONFIG3_WDTPS_128S = (17 << 0),
    CONFIG3_WDTPS_256S = (18 << 0),
    CONFIG3_WDTPS_MASK = (0x1F << 0)
} CONFIG_CONFIG3_t;

/* Configuration Word 4 Memory Write Protection */
typedef enum CONFIG_CONFIG4_enum {
    /* Bit 13 – LVP Low-Voltage Programming Enable bit */
    CONFIG4_LVP_ENABLED = (1 << 13),
    /* Bit 12 – SCANE Scanner Enable bit */
    CONFIG4_SCANE_ENABLED = (1 << 12),
    /* Bit 10 – WRTD Data EEPROM Write Protection bit */
    CONFIG4_WRTD_DISABLED = (1 << 10),
    /* Bit 9 – WRTB Boot Block Write Protection bit */
    CONFIG4_WRTB_DISABLED = (1 << 9),
    /* Bit 8 – WRTC Configuration Register Write Protection bit */
    CONFIG4_WRTC_DISABLED = (1 << 8),
    /* Bits 0, 1, 2, 3 – WRTn User NVM Self-Write Protection bits */
    CONFIG4_WRTN3_DISABLED = (1 << 3),
    CONFIG4_WRTN2_DISABLED = (1 << 2),
    CONFIG4_WRTN1_DISABLED = (1 << 1),
    CONFIG4_WRTN0_DISABLED = (1 << 0)
} CONFIG_CONFIG4_t;

/* Configuration Word 5 Code Protection */
typedef enum CONFIG_CONFIG5_enum {
    /* Bit 1 – CPD Data NVM (DFM) Memory Code Protection bit */
    CONFIG5_CPD_DISABLED = (1 << 1),
    /* Bit 0 – CP User NVM Program Memory Code Protection bit */
    CONFIG5__CP_DSIABLED = (1 << 0)
} CONFIG_CONFIG5_t;

/* Configuration Word 6 Memory Read Protection */ 
typedef enum CONFIG_CONFIG6_enum {
    /* Bit 9 – EBTRB Table Read Protection bit, memory Boot Block NOT protected from table reads executed in other blocks */
    CONFIG6_EBTRB_DISABLED = (1 << 1),
    /* Bits 0, 1, 2, 3 – EBTRn Table Read Protection bits, Corresponding Memory Block NOT protected from table reads executed in other blocks */
    CONFIG6_EBTR3_DSIABLED = (1 << 3),
    CONFIG6_EBTR2_DSIABLED = (1 << 2),
    CONFIG6_EBTR1_DSIABLED = (1 << 1),
    CONFIG6_EBTR0_DSIABLED = (1 << 0)
} CONFIG_CONFIG6_t;

/* DEVICE ID */
typedef enum DEVICIE_ID_enum {
    DEVICIE_ID_PIC18F26Q10 = 0x7180,
    DEVICIE_ID_PIC18F45Q10 = 0x7140,
    DEVICIE_ID_PIC18F46Q10 = 0x7120 
} DEVICIE_ID_t;

/* REVISION ID */
typedef enum REVISION_ID_enum {
    /* Bits 15:12 – 1010[3:0] Read as ‘1010’ */
    REVISION_ID_TAG = (0xA << 12),
    /* Bits 11:6 – MJRREV[5:0] Major Revision ID bits */
    REVISION_IDMJRREV_MASK = (0x3F << 6),
    REVISION_IDMJRREV_SHIFT = 6,
    /* Bits 5:0 – MNRREV[5:0] Minor Revision ID bits */
    REVISION_MNRREV_MASK = (0x3F << 0),
    REVISION_MNRREV_SHIFT = 0
} REVISION_ID_t;

/*
--------------------------------------------------------------------------
WWDT - Windowed Watchdog Timer
--------------------------------------------------------------------------
*/
/* Windowed Watchdog Timer */
#define WDT_BASE *(WDT_t *)&WDTCON0

typedef struct WDT_struct
{
    register8_t WDTCON0;
    register8_t WDTCON1;

    /* WWDT Prescale Select Low Register (Read-Only) */
    register8_t WDTPSL;

    /* WWDT Prescale Select High Register (Read-Only) */
    register8_t WDTPSH;

    /* WDT Timer Register (Read-Only) */
    register8_t WDTTMR;
} WDT_t;

/* Watchdog Timer Control Register 0 */
typedef enum WDT_WDTCON0_enum {
    /* Bits 5:1 – WDTPS[4:0] Watchdog Timer Prescale Select bits */
    WDTCON0_WDTPS_1MS = (0 << 1),
    WDTCON0_WDTPS_2MS = (1 << 1),
    WDTCON0_WDTPS_4MS = (2 << 1),
    WDTCON0_WDTPS_8MS = (3 << 1),
    WDTCON0_WDTPS_16MS = (4 << 1),
    WDTCON0_WDTPS_32MS = (5 << 1),
    WDTCON0_WDTPS_64MS = (6 << 1),
    WDTCON0_WDTPS_128MS = (7 << 1),
    WDTCON0_WDTPS_256MS = (8 << 1),
    WDTCON0_WDTPS_512MS = (9 << 1),
    WDTCON0_WDTPS_1S = (10 << 1),
    WDTCON0_WDTPS_2S = (11 << 1),
    WDTCON0_WDTPS_4S = (12 << 1),
    WDTCON0_WDTPS_8S = (13 << 1),
    WDTCON0_WDTPS_16S = (14 << 1),
    WDTCON0_WDTPS_32S = (15 << 1),
    WDTCON0_WDTPS_64S = (16 << 1),
    WDTCON0_WDTPS_128S = (17 << 1),
    WDTCON0_WDTPS_256S = (18 << 1),
    WDTCON0_WDTPS_MASK = (0x1F << 1),

    /* Bit 0 – SEN Software Enable/Disable for Watchdog Timer bit */
    WDTCON0_SEN_ON = (1 << 0)
} WDT_WDTCON0_t;

#define WDTPS_V(_r) ((register8_t)((_r) >> 1))
#define WDTPS_TO_MS(_v) (1 << (_v))
#define WDTPS_V_TO_MS(_r) (1 << WDTPS_V(_r))

/* Watchdog Timer Control Register 1 */
typedef enum WDT_WDTCON1_enum {
    /* Bits 6:4 – WDTCS[2:0] Watchdog Timer Clock Select bits */
    WDTCON1_WDTCS_LFINTOSC = (0 << 4),
    WDTCON1_WDTCS_MFINTOSC = (1 << 4),

    /* Bits 2:0 – WINDOW[2:0] Watchdog Timer Window Select bits */
    WDTCON1_WINDOWN_12_5_POINTS = (0 << 0),
    WDTCON1_WINDOWN_25_POINTS = (1 << 0),
    WDTCON1_WINDOWN_37_5_POINTS = (2 << 0),
    WDTCON1_WINDOWN_50_POINTS = (3 << 0),
    WDTCON1_WINDOWN_62_5_POINTS = (4 << 0),
    WDTCON1_WINDOWN_75_POINTS = (5 << 0),
    WDTCON1_WINDOWN_82_5_POINTS = (6 << 0),
} WDT_WDTCON1_t;

/* WDT Timer Register (Read-Only) */
typedef enum WDT_WDTMR_enum {
    /* Bits 7:3 – WDTTMR[4:0] Watchdog Window Value bits */
    WDTTMR_STATE_WDTTMR_MASK = (0x3F << 3),

    /* Bit 2 – STATE WDT Armed Status bit */
    WDTTMR_STATE_ARMED = (1 << 2)

    /* Bits 1:0 – PSCNT[1:0] Prescale Select Upper bits(*/
} WDT_WDTMR_t;

/*
--------------------------------------------------------------------------
PPS - Peripheral Pin Select Module
--------------------------------------------------------------------------
*/
/* 17.4 PPS Lock */
#define PPSLOCK_SEQ0 0x55
#define PPSLOCK_SEQ1 0xAA


/* Peripheral xxx Input Selection */

/* Bits 2:0 – PIN[2:0] Peripheral xxx Input Pin Selection bits */
#define _PPS_PIN(_n) ((_n)& 0x7)

/* Bits 4:3 – PORT[1:0] Peripheral xxx Input PORT Selection bits */
#define _PPS_GRP(_g) (((_g) & 3) << 3)

#define PPSA(_n)  (_PPS_GRP(0) | _PPS_PIN(_n))
#define PPSB(_n)  (_PPS_GRP(1) | _PPS_PIN(_n))
#define PPSC(_n)  (_PPS_GRP(2) | _PPS_PIN(_n))
#define PPSD(_n)  (_PPS_GRP(3) | _PPS_PIN(_n))

#define TO_PPS_PORT(_an)  (((_an) >> 3) & 0x3)
#define TO_PPS_PIN(_an)  ((_an) & 0x7)

/* Pin Rxy Output Source Selection Register */

typedef enum PPS_RxyOUT_enum {
    /* Bits 4:0 – RxyPPS[4:0] Pin Rxy Output Source Selection bits */
    RxyOUT_LATxy = (0x0 << 0),
    RxyOUT_CWG1A = (0x1 << 0),
    RxyOUT_CWG1B = (0x2 << 0),
    RxyOUT_CWG1C = (0x3 << 0),
    RxyOUT_CWG1D = (0x4 << 0),
    RxyOUT_CCP1 = (0x5 << 0),
    RxyOUT_CCP2 = (0x6 << 0),
    RxyOUT_PWM3 = (0x7 << 0),
    RxyOUT_PWM4 = (0x8 << 0),
    RxyOUT_EUSART1_TX_CK = (0x9 << 0),
    RxyOUT_EUSART1_DT = (0xA << 0),
    RxyOUT_EUSART2_TX_CK = (0xB << 0),
    RxyOUT_EUSART2_DT = (0xC << 0),
    RxyOUT_CMP1 = (0xD << 0),
    RxyOUT_CMP2 = (0xE << 0),
    RxyOUT_MSSP1_SCK_SCL = (0xF << 0),
    RxyOUT_MSSP1_SDO_SDA = (0x10 << 0),
    RxyOUT_MSSP2_SCK_SCL = (0x11 << 0),
    RxyOUT_MSSP2_SDO_SDA = (0x12 << 0),
    RxyOUT_TMR0 = (0x13 << 0),
    RxyOUT_CLKR = (0x14 << 0),
    RxyOUT_DSM = (0x15 << 0),
    RxyOUT_ADGRDA = (0x16 << 0),
    RxyOUT_ADGRDB = (0x17 << 0),
    RxyOUT_CLC1OUT = (0x18 << 0),
    RxyOUT_CLC2OUT = (0x19 << 0),
    RxyOUT_CLC3OUT = (0x20 << 0),
    RxyOUT_CLC4OUT = (0x21 << 0),
    RxyOUT_CLC5OUT = (0x22 << 0),
    RxyOUT_CLC6OUT = (0x23 << 0),
    RxyOUT_CLC7OUT = (0x24 << 0),
    RxyOUT_CLC8OUT = (0x25 << 0),
} PPS_RxyOUT_t;

/*
--------------------------------------------------------------------------
FVR - Fixed Voltage Reference
--------------------------------------------------------------------------
*/

#define FVR_BASE *(FVR_t *)&FVRCON

/* FVR Fixed Voltage Reference */
typedef struct FVR_struct
{
    register8_t FVRCON;
} FVR_t;

/* Fixed Voltage Reference Control Register */
typedef enum FVR_FVRCON_enum
{
    /* Bits 1:0 – ADFVR[1:0] ADC FVR Buffer Gain Selection bit */
    ADFVR_GAIN_1 = 1,
    ADFVR_GAIN_2 = 2,
    ADFVR_GAIN_4 =3,

    FVRCON_ADFVR_OFF = (0 << 0),
    FVRCON_ADFVR_GAIN_1 = (1 << 0),
    FVRCON_ADFVR_GAIN_2 = (2 << 0),
    FVRCON_ADFVR_GAIN_4 = (3 << 0),

    /* Bits 3:2 – CDAFVR[1:0] Comparator FVR Buffer Gain Selection bits */
    FVRCON_CDAFVR_OFF = (0 << 2),
    FVRCON_CDAFVR_GAIN_1 = (1 << 2),
    FVRCON_CDAFVR_GAIN_2 = (2 << 2),
    FVRCON_CDAFVR_GAIN_4 = (3 << 2),

    /* Bit 4 – TSRNG Temperature Indicator Range Selection bit(2) */
    FVRCON_TSRNG_VDD_2VT = (0 << 4),
    FVRCON_TSRNG_VDD_4VT = (1 << 4),

    /* Bit 5 – TSEN Temperature Indicator Enable bit(2) */
    FVRCON_TSEN_ENABLE = (1 << 5),

    /* Bit 6 – FVRRDY Fixed Voltage Reference Ready Flag bit */
    FVRCON_FVRRDY_READY = (1 << 6),

    /* Bit 7 – FVREN Fixed Voltage Reference Enable bit */
    FVRCON_FVREN_ENABLE = (1 << 7)
} FVR_FVRCON_t;

/*
--------------------------------------------------------------------------
ADC - Analog to Digital Converter
--------------------------------------------------------------------------
*/

#define ADC_BASE *(ADC_t *)&ADACT

/* Analog to Digital Converter */
typedef struct ADC_struct
{
    register8_t ADACT;  /* ADC AUTO Conversion Trigger Source Selection Register */
    register8_t ADCLK;  /* ADC Clock Selection Register */
    register8_t ADREF;  /* ADC Reference Selection Register */
    register8_t ADCON1;  /* ADC Control Register 1 */
    register8_t ADCON2;  /* ADC Control Register 2 */
    register8_t ADCON3;  /* ADC Control Register 3 */
    register8_t ADACQ;  /* ADC Acquisition Time Control Register */
    register8_t ADCAP;  /* ADC Additional Sample Capacitor Selection Register */
    register8_t ADPRE;  /* ADC Repeat Setting Register */
    register8_t ADPCH;  /* ADC Positive Channel Selection Register */
    register8_t ADCON0;  /* ADC Control Register 0 */

    /* ADC Previous Result Register */
    /* Bits 7:0 – ADPREVL[7:0] Previous ADC Result Least Significant bits */
    /* Bits 15:8 – ADPREVH[7:0] Previous ADC Result Most Significant bits */
    _WORDREGISTER(ADPREV);  /* ADC Previous Result Register */
    
    /* ADC Result Register */
    /* Bits 7:0 – ADRESL[7:0] ADC Result Register bits. Lower bits */
    /* Bits 15:8 – ADRESH[7:0] ADC Result Register bits. High bits */
    _WORDREGISTER(ADRES);  /* ADC Result Registerl */

    register8_t ADSTAT;  /* ADC Status Register */

    /* ADC Repeat Setting Register */
    /* Bits 7:0 – ADRPT[7:0] ADC Repeat Threshold bits */
    //  Determines the number of times that the ADC is triggered for a threshold check. When ADCNT reaches this value the
    //  error threshold is checked. Used when the computation mode is Low-pass Filter, Burst Average, or Average.
    register8_t ADRPT;  /* ADC Repeat Setting Register */

    /* ADC Repeat Counter Register */
    /* Bits 7:0 – ADCNT[7:0] ADC Repeat Count bits */
    //  Counts the number of times that the ADC is triggered before the threshold is checked. When this value reaches
    //  ADPRT then the threshold is checked. Used when the computation mode is Low-pass Filter, Burst Average, or Average.
    register8_t ADCNT;  /* ADC Repeat Counter Register */

    /* ADC Threshold Setpoint Register */
    //  Depending on ADCALC, may be used to determine ADERR.
    /* Bits 15:8 – ADSTPTH[7:0] */
    //  ADC Threshold Setpoint Most Significant Byte.
    /* Bits 7:0 – ADSTPTL[7:0] */
    //  ADC Threshold Setpoint Least Significant Byte.
    _WORDREGISTER(ADSTPT);  /* ADC Threshold Setpoint Register */

    /* ADC Lower Threshold Register */
    //  ADLTH and ADUTH are compared with ADERR to set the ADUTHR and ADLTHR bits of ADSTAT. Depending on the
    //  setting of ADTMD, an interrupt may be triggered by the results of this comparison.
    /* Bits 15:8 – ADLTHH[7:0] ADC Lower Threshold MSB */
    /* Bits 7:0 – ADLTHL[7:0] ADC Lower Threshold LSB */
    _WORDREGISTER(ADLTH);  /* ADC Lower Threshold Register */

    /* ADC Upper Threshold Register */
    //  ADLTH and ADUTH are compared with ADERR to set the ADUTHR and ADLTHR bits of ADSTAT. Depending on the
    //  setting of ADTMD, an interrupt may be triggered by the results of this comparison.
    /* Bits 15:8 – ADUTHH[7:0] ADC Upper Threshold MSB */
    /* Bits 7:0 – ADUTHL[7:0] ADC Upper Threshold LSB */
    _WORDREGISTER(ADUTH);  /* ADC Upper Threshold Register */
 
    /* ADC Setpoint Error Register. ADC Setpoint Error calculation is determined by the ADCALC bits. */
    /* Bits 15:8 – ADERRH[7:0] */
    //  ADC Setpoint Error MSB
    /* Bits 7:0 – ADERRL[7:0] */
    //  ADC Setpoint Error LSB
    _WORDREGISTER(ADERR);  /* ADC Setpoint Error Register. */

    /* ADC Accumulator Register */
    /* Bits 15:8 – ADACCH[7:0] */
    //  ADC Accumulator Most Significant Byte.
    /* Bits 7:0 – ADACCL[7:0] */
    //  ADC Accumulator Least Significant Byte.
    _WORDREGISTER(ADACC);   /* ADC Accumulator Register */

    /* ADC Filter Register. */
    //  In Accumulate, Average, and Burst Average mode, this is equal to ADACC right shifted by the
    //  ADCRS bits of ADCON2. In LPF mode, this is the output of the low-pass filter.
    /* Bits 15:8 – ADFLTRH[7:0] ADC Filter Output Most Significant bits */
    /* Bits 7:0 – ADFLTRL[7:0] ADC Filter Output Least Significant bits */
    _WORDREGISTER(ADFLTR);  /* ADC Filter Register. */
} ADC_t;

/* ADC Auto-Conversion Trigger Sources */
typedef enum ADC_ADACT_enum
{
    ADC_ADACT_DISABLED = 0,
    ADC_ADACT_ADACTPPS,
    ADC_ADACT_TMR0,
    ADC_ADACT_TMR1,
    ADC_ADACT_TMR2,
    ADC_ADACT_TMR3,
    ADC_ADACT_TMR4,
    ADC_ADACT_TMR5,
    ADC_ADACT_TMR6,
    ADC_ADACT_CCP1,
    ADC_ADACT_CCP2,
    ADC_ADACT_PWM3,
    ADC_ADACT_PWM4,
    ADC_ADACT_C1,
    ADC_ADACT_C2,
    ADC_ADACT_IOC,
    ADC_ADACT_CLC1,
    ADC_ADACT_CLC2,
    ADC_ADACT_CLC3,
    ADC_ADACT_CLC4,
    ADC_ADACT_CLC5,
    ADC_ADACT_CLC6,
    ADC_ADACT_CLC7,
    ADC_ADACT_CLC8,
    ADC_ADACT_READ_ADERRH = 0x1C,
    ADC_ADACT_READ_ADRESH,
    ADC_ADACT_WRITE_ADPCH = 0x1F
} ADC_ADACT_t;

/* Clock Pre-scaler select */
#define ADCS_MIN 2
#define ADCS_MAX 128
#define ADC_ADCLK_ADCS_DIV_N(_n)    (((_n) >= ADCS_MIN) ? (((_n) <= ADCS_MAX) ? (((_n) >> 1) - 1) : 63) : 0)

/* ADC Reference Selection Register */
typedef enum ADC_ADREF_enum
{
    /* Bits 1:0 – ADPREF[1:0] ADC Positive Voltage Reference Selection bits */
    ADREF_ADPREF_VDD = (0x0 << 0),
    ADREF_ADPREF_VREF_P = (0x2 << 0),
    ADREF_ADPREF_FVR =    (0x3 << 0),

    /* Bit 4 – ADNREF ADC Negative Voltage Reference Selection bit */
    ADREF_ADNREF_VSS = (0x0 << 4),
    ADREF_ADNREF_VREF_N = (0x1 << 4)
} ADC_ADREF_t;

/* ADC Control Register 1 */
typedef enum ADC_ADCON1_enum
{
    /* Bit 0 – ADDSEN Double-Sample Enable bit */
    ADCON1_ADDSEN_NONE = (0x0 << 0),
    ADCON1_ADDSEN_DOUBLE = (0x1 << 0),

    /* Bit 5 – ADGPOL Guard Ring Polarity Selection bit */
    ADCON1_ADGPOL_HIGH = (0x1 << 5),
    ADCON1_ADGPOL_LOW = (0x0 << 5),

    /* Bit 6 – ADIPEN A/D Inverted Precharge Enable bit */
    ADCON1_ADIPEN_NONE = (0x0 << 6),
    ADCON1_ADIPEN_INVERT = (0x1 << 6),

    /* Bit 7 – ADPPOL Precharge Polarity bit */
    ADCON1_ADPPOL_VSS = (0x0 << 7),
    ADCON1_ADPPOL_VDD = (0x1 << 7),
} ADC_ADCON1_t;

/* ADC Control Register 2 */
typedef enum ADC_ADCON2_enum
{
    /* Bits 2:0 – ADMD[2:0] ADC Operating Mode Selection bits(4) */
    ADCON2_ADMD_BASIC_MODE = (0x0 << 0),
    ADCON2_ADMD_ACC_MODE = (0x1 << 0),
    ADCON2_ADMD_AVG_MODE = (0x2 << 0),
    ADCON2_ADMD_BURST_AVG_MODE = (0x3 << 0),
    ADCON2_ADMD_LOW_PASS_MODE = (0x4 << 0),

    /* Bit 3 – ADACLR A/D Accumulator Clear Command bit(3) */
    ADCON2_ADACLR_DONE = (0x0 << 3), 
    ADCON2_ADACLR_CLEAR = (0x1 << 3),
    
    /* Bits 6:4 – ADCRS[2:0] ADC Accumulated Calculation Right Shift Select bits */
    // ADCON2_ADCRS(n)
    // 1 to 6: ADMD = 'b100, Low-pass filter time constant is 2ADCRS, filter gain is 1:1(2)
    // 1 to 6: ADMD =' b011 to 'b001, The accumulated value is right-shifted by ADCRS (divided by 2ADCRS)(1,2)
    ADCON2_ADCRS_RSHIFT_0   = (0x0 << 4),
    ADCON2_ADCRS_RSHIFT_1   = (0x1 << 4),
    ADCON2_ADCRS_RSHIFT_2   = (0x2 << 4),
    ADCON2_ADCRS_RSHIFT_3   = (0x3 << 4),
    ADCON2_ADCRS_RSHIFT_4   = (0x4 << 4),
    ADCON2_ADCRS_RSHIFT_5   = (0x5 << 4),
    ADCON2_ADCRS_RSHIFT_6   = (0x6 << 4),

    /* Bit 7 – ADPSIS ADC Previous Sample Input Select bits */
    ADCON2_ADPSIS_ADRES = (0x0 << 7),
    ADCON2_ADPSIS_ADFLTR = (0x1 << 7)
} ADC_ADCON2_t;

#define ADCON2_ADCRS_MAX_VALUE 7
#define ADCON2_ADCRS(_n)  (uint8_t)(((_n) & 0x7) << 4)


/* ADC Control Register 3 */
typedef enum ADC_ADCON3_enum
{
    /* Bits 2:0 – ADTMD[2:0] Threshold Interrupt Mode Select bits */
    ADCON3_ADTMD_IRQ_NONE = (0x0 << 0),
    ADCON3_ADTMD_IRQ_WHEN_ADERR_LESS_THAN_ADLTH = (0x1 << 0),
    ADCON3_ADTMD_IRQ_WHEN_ADERR_MORE_OR_EQUAL_THAN_ADLTH = (0x2 << 0),
    ADCON3_ADTMD_IRQ_WHEN_ADERR_MORE_THAN_ADLTH_AND_LESS_THAN_ADUTH = (0x3 << 0),
    ADCON3_ADTMD_IRQ_WHEN_ADERR_LESS_THAN_ADLTH_OR_MORE_THAN_ADUTH = (0x4 << 0),
    ADCON3_ADTMD_IRQ_WHEN_ADERR_LESS_OR_EQUAL_THAN_ADUTH = (0x5 << 0),
    ADCON3_ADTMD_IRQ_WHEN_ADERR_MORE_THAN_ADUTH = (0x6 << 0),
    ADCON3_ADTMD_IRQ_ANY = (0x7 << 0),

    /* Bit 3 – ADSOI ADC Stop-on-Interrupt bit */
    ADCON3_ADSOI_ADGO_ALWAYS_ON = (0x0 << 3),
    ADCON3_ADSOI_ADGO_STOP_ON_IRQ = (0x1 << 3),

    /* Bits 6:4 – ADCALC[2:0] ADC Error Calculation Mode Select bits */
    ADCON3_ADCALC_RESULT_VS_PREV = (0x0 << 4),
    ADCON3_ADCALC_RESULT_VS_SETPOINT = (0x1 << 4),
    ADCON3_ADCALC_RESULT_VS_FILTER = (0x2 << 4),
    ADCON3_ADCALC_PREV_VS_FILTER = (0x4 << 4),
    ADCON3_ADCALC_FILTER_VS_SETPOINT = (0x5 << 4),
    ADCON3_ADCALC_RESULT = (0x6 << 4),
    ADCON3_ADCALC_FILTER = (0x7 << 4)
} ADC_ADCON3_t;

/* ADC Acquisition Time Control Register */
typedef enum ADC_ADACQ_enum
{
    /* Bits 7:0 – ADACQ[7:0] Acquisition (charge share time) Select bits */
    //  0x00: Acquisition time is not included in the data conversion cycle */
    //  0x01 to 0xFF: Number of ADC clock periods in the acquisition time */
    ADACQ_ACQUISITION_NONE = (0x0 << 0),
    ADACQ_ACQUISITION_FOSC_1us = (CPU_uS_COUNT(0) << 0),
    ADACQ_ACQUISITION_FOSC_1_5us = (CPU_100nS_COUNT(15) << 0),
    ADACQ_ACQUISITION_FOSC_2us = (CPU_uS_COUNT(2) << 0)
} ADC_ADACQ_t;

#define ADACQ_ACQUISITION_N(_n) ((_n)/* << 0 */)

/* ADC Additional Sample Capacitor Selection Register */
typedef enum ADC_ADCAP_enum
{
    /* Bits 4:0 – ADCAP[4:0] ADC Additional Sample Capacitor Selection bits */
    //  0x00: No additional capacitance
    //  1 to 31: Number of pF in the additional capacitance
    ADCAP_COMPENSATION_NONE = (0x0 << 0)
} ADC_ADCAP_t;

/* ADC Precharge Time Control Register */
typedef enum ADC_ADPRE_enum
{
    /* Bits 7:0 – ADPRE[7:0] Precharge Time Select bits */
    //  0: Precharge time is not included in the data conversion cycle
    //  1 to 255: Number of ADC clocks in the precharge time.
    ADC_ADPRE_NONE = (0x0 << 0),
    ADC_ADPRE_DEFAULT = (0x2 << 0)
} ADC_ADPRE_t;

/* ADC Positive Channel Selection Register */
typedef enum ADC_ADPCH_enum
{
    /* Bits 5:0 – ADPCH[5:0] ADC Positive Input Channel Selection bits */
    // RA[n] (value as ANx[n])
    ADC_ADPCH_RA0 = (0x0 << 0),
    ADC_ADPCH_RA1 = (0x1 << 0),
    ADC_ADPCH_RA2 = (0x2 << 0),
    ADC_ADPCH_RA3 = (0x3 << 0),
    ADC_ADPCH_RA4 = (0x4 << 0),
    ADC_ADPCH_RA5 = (0x5 << 0),
    ADC_ADPCH_RA6 = (0x6 << 0),
    ADC_ADPCH_RA7 = (0x7 << 0),

    // RB[n]
    ADC_ADPCH_RB0 = (0x8 << 0),
    ADC_ADPCH_RB1 = (0x9 << 0),
    ADC_ADPCH_RB2 = (0x10 << 0),
    ADC_ADPCH_RB3 = (0x11 << 0),
    ADC_ADPCH_RB4 = (0x12 << 0),
    ADC_ADPCH_RB5 = (0x13 << 0),
    ADC_ADPCH_RB6 = (0x14 << 0),
    ADC_ADPCH_RB7 = (0x15 << 0),

    // RC[n]
    ADC_ADPCH_RC0 = (0x10 << 0),
    ADC_ADPCH_RC1 = (0x11 << 0),
    ADC_ADPCH_RC2 = (0x12 << 0),
    ADC_ADPCH_RC3 = (0x13 << 0),
    ADC_ADPCH_RC4 = (0x14 << 0),
    ADC_ADPCH_RC5 = (0x15 << 0),
    ADC_ADPCH_RC6 = (0x16 << 0),
    ADC_ADPCH_RC7 = (0x17 << 0),

    // RD[x]
    ADC_ADPCH_RD0 = (0x18 << 0),
    ADC_ADPCH_RD1 = (0x19 << 0),
    ADC_ADPCH_RD2 = (0x1A << 0),
    ADC_ADPCH_RD3 = (0x1B << 0),
    ADC_ADPCH_RD4 = (0x1C << 0),
    ADC_ADPCH_RD5 = (0x1D << 0),
    ADC_ADPCH_RD6 = (0x1E << 0),
    ADC_ADPCH_RD7 = (0x1F << 0),

    // RE[x]
    ADC_ADPCH_RE0 = (0x20 << 0),
    ADC_ADPCH_RE1 = (0x21 << 0),
    ADC_ADPCH_RE2 = (0x22 << 0),

    // Others
    ADC_ADPCH_VSS = (0x3C << 0),
    ADC_ADPCH_TEMP = (0x3D << 0),
    ADC_ADPCH_DAC1 = (0x3E << 0),
    ADC_ADPCH_FVR = (0x3F << 0),
} ADC_ADPCH_t;

#define FIRST_ADC_CHANNEL ADC_ADPCH_RA0
#define LAST_ADC_CHANNEL ADC_ADPCH_RE2
#define ADC_CHANNEL(_i) (ADC_ADPCH_RA0 + (_i))
#define MAXIMUM_ADC_CHANNEL_NODES	(LAST_ADC_CHANNEL - FIRST_ADC_CHANNEL + 1)


/* ADC Control Register 0 */
typedef enum ADC_ADCON0_enum
{
    /* Bit 0 – ADGO ADC Conversion Status bit */
    ADCON0_ADGO_EN = (0x1 << 0),

    /* Bit 2 – ADFM ADC results Format/alignment Selection */
    ADCON0_ADFM_LEFT_JUSTIFIED = (0x0 << 2),
    ADCON0_ADFM_RIGHT_JUSTIFIED = (0x1 << 2),

    /* Bit 4 – ADCS ADC Clock Selection bit */
    ADCON0_ADCS_CLK_FOSC = (0x0 << 4),
    ADCON0_ADCS_CLK_FRC = (0x1 << 4),

    /* Bit 6 – ADCONT ADC Continuous Operation Enable bit */
    //  0: ADC is cleared upon completion of each conversion trigger
    //  1: ADGO is retriggered upon completion of each conversion trigger until ADTIF is set (if ADSOI is set) or until ADGO is cleared (regardless of the value of ADSOI)
    ADCON0_ADCONT_TRIGGER_ONCE = (0x0 << 6),
    ADCON0_ADCONT_TRIGGER_CONTINOUS = (0x1 << 6),

    /* Bit 7 – ADON ADC Enable bit */
    ADCON0_ADON_ENABLE = (1 << 7)
} ADC_ADCON0_t;

/* ADC Status Register */
typedef enum ADC_ADSTAT_enum
{
    /* Bits 2:0 – ADSTAT[2:0] */
    ADSTAT_STAT_NOT_CONVERTING = (0x0 << 0),
    ADSTAT_STAT_1ST_STAGE_PRECHARGING = (0x1 << 0),
    ADSTAT_STAT_1ST_STAGE_ACQ = (0x2 << 0),
    ADSTAT_STAT_1ST_STAGE_CONVERSION = (0x3 << 0),

    ADSTAT_STAT_2ST_STAGE_PRECHARGING = (0x5 << 0),
    ADSTAT_STAT_2ST_STAGE_ACQ = (0x6 << 0),
    ADSTAT_STAT_2ST_STAGE_CONVERSION = (0x7 << 0),
    ADSTAT_STAT_MASK = (0x7 << 0),

    /* Bit 4 – ADMATH ADC Module Computation Status bit */
    //  Registers ADACC, ADFLTR, ADUTH, ADLTH and the ADAOV bit are updating or have already updated
    ADSTAT_ADMATH_DATA_CHANGED = (0x1 << 4),

    /* Bit 5 – ADLTHR ADC Module Less-than Lower Threshold Flag bit */
    ADSTAT_ADLTHR_ADERR_LESS_THAN_ADLTH = (1 << 5),

    /* Bit 6 – ADUTHR ADC Module Greater-than Upper Threshold Flag bit */
    ADSTAT_ADUTHR_ADERR_MORE_THAN_ADUTH = (1 << 6),

    /* Bit 7 – ADAOV ADC Accumulator Overflow bit */
    ADSTAT_ADAOV_ACCU_OVERFLOW = (1 << 7)
} ADC_ADSTAT_t;

/* ADC Repeat Setting Register */
/* Bits 7:0 – ADRPT[7:0] ADC Repeat Threshold bits */

#endif /* !defined (__ASSEMBLER__) */

#endif  /* _Q10_IO_H */