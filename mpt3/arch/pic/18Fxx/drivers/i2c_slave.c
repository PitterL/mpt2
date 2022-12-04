/**
  I2C1 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    i2c1_slave.c

  @Summary
    This is the generated driver implementation file for the I2C1 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides implementations for driver APIs for I2C1.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC18F26Q10
        Driver Version    :  2.0.1
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.31 and above or later
        MPLAB             :  MPLAB X 5.45
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip software and any
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party
    license terms applicable to your use of third party software (including open source software) that
    may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS
    FOR A PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS
    SOFTWARE.
*/

#include <xc.h>
#include "arch/cpu.h"
#include "i2c_slave.h"

/**
 Section: Global Variables
 */
typedef enum
{
    I2C1_IDLE,
    I2C1_START,
    I2C1_ADDR,
    I2C1_DATA,
    I2C1_COLLISION
} i2c1_slave_state_t;

/**
 Section: Global Variables
 */
volatile uint8_t i2c1WrData;
volatile uint8_t i2c1RdData;
volatile uint8_t i2c1SlaveAddr;
static volatile i2c1_slave_state_t i2c1SlaveState = I2C1_IDLE;

/**
 Section: Functions declaration
 */
static void I2C1_Isr(void);
static void I2C1_SlaveDefRdInterruptHandler(void);
static void I2C1_SlaveDefWrInterruptHandler(void);
static void I2C1_SlaveDefAddrInterruptHandler(void);
static void I2C1_SlaveDefWrColInterruptHandler(void);
static void I2C1_SlaveDefBusColInterruptHandler(void);

static void I2C1_SlaveRdCallBack(void);
static void I2C1_SlaveWrCallBack(void);
static void I2C1_SlaveAddrCallBack(void);
static void I2C1_SlaveWrColCallBack(void);
static void I2C1_SlaveBusColCallBack(void);
#ifdef USE_MPTT_WRAPPER
static void I2C1_SlaveStopCallBack(void);
#endif

static inline bool I2C1_SlaveOpen();
static inline void I2C1_SlaveClose();
static inline void I2C1_SlaveSetSlaveAddr(uint8_t slaveAddr);
static inline void I2C1_SlaveSetSlaveMask(uint8_t maskAddr);
static inline void I2C1_SlaveEnableIrq(void);
static inline bool I2C1_SlaveIsAddr(void);
static inline bool I2C1_SlaveBusIsRead(void);
#ifdef USE_MPTT_WRAPPER
static inline bool I2C1_SlaveIsStart(void);
static inline bool I2C1_SlaveIsStop(void);
#endif
static inline void I2C1_SlaveClearBuff(void);
static inline void I2C1_SlaveClearIrq(void);
static inline void I2C1_SlaveReleaseClock(void);
static inline bool I2C1_SlaveIsWriteCollision(void);
static inline void I2C1_SlaveIsClearCollision();
static inline bool I2C1_SlaveIsTxBufEmpty(void);
static inline bool I2C1_SlaveIsData(void);
static inline void I2C1_SlaveRestart(void);
static inline bool I2C1_SlaveIsRxBufFull(void);
static inline void I2C1_SlaveSendTxData(uint8_t data);
static inline uint8_t I2C1_SlaveGetRxData(void);
static inline uint8_t I2C1_SlaveGetAddr(void);
static inline uint8_t I2C1_SlaveGetMask();
static inline void I2C1_SlaveSendAck(void);
static inline void I2C1_SlaveSendNack(void);
static inline bool I2C1_SlaveIsOverFlow(void);

void I2C1_Initialize()
{
    // SMP: High speed(400Khz);CKE: Disables SMBus-specific inputs
    SSP1STAT = 0x00;
    // SSPEN disabled; CKP hold; SSPM: I2C 7bit with Start/Stop interrupt
    SSP1CON1 |= 0x0E;
    // GCEN: Enabled; ACKEN: disable
    SSP1CON2 = 0x81;

    // PPS
    SSP1CLKPPS = PPSB(4);
    SSP1DATPPS = PPSB(5);
    RB4PPS = RxyOUT_MSSP1_SCK_SCL;
    RB5PPS = RxyOUT_MSSP1_SDO_SDA;
}

void I2C1_Open()
{
    I2C1_SlaveSetSlaveAddr(I2C1_SLAVE_ADDRESS);
    I2C1_SlaveSetSlaveMask(I2C1_SLAVE_MASK);
    I2C1_SlaveSetIsrHandler(I2C1_Isr);
#ifndef USE_MPTT_WRAPPER
    I2C1_SlaveSetBusColIntHandler(I2C1_SlaveDefBusColInterruptHandler);
    I2C1_SlaveSetWriteIntHandler(I2C1_SlaveDefWrInterruptHandler);
    I2C1_SlaveSetReadIntHandler(I2C1_SlaveDefRdInterruptHandler);
    I2C1_SlaveSetAddrIntHandler(I2C1_SlaveDefAddrInterruptHandler);
    I2C1_SlaveSetWrColIntHandler(I2C1_SlaveDefWrColInterruptHandler);
#endif
    I2C1_SlaveEnableIrq();
    I2C1_SlaveOpen();
}

void I2C1_Close()
{
    I2C1_SlaveClose();
}

uint8_t I2C1_Read()
{
    return I2C1_SlaveGetRxData();
}

void I2C1_Write(uint8_t data)
{
    I2C1_SlaveSendTxData(data);
}

bool I2C1_IsRead()
{
    return I2C1_SlaveBusIsRead();
}

void I2C1_Enable()
{
    I2C1_Initialize();
}

void I2C1_SendAck()
{
    I2C1_SlaveSendAck();
}

void I2C1_SendNack()
{
    I2C1_SlaveSendNack();
}

static void I2C1_Isr()
{
    I2C1_SlaveClearIrq();

    /* There will report the Collision in production build */
    if (I2C1_SlaveIsWriteCollision())
    {
        i2c1SlaveState = I2C1_COLLISION;

        I2C1_SlaveIsClearCollision();
        I2C1_SlaveBusColCallBack();
    }
    
    if (I2C1_SlaveIsStart())
    {
        if (i2c1SlaveState == I2C1_IDLE)
        {
            i2c1SlaveState = I2C1_START;
        }
    }
    else if (I2C1_SlaveIsStop())
    {
        i2c1SlaveState = I2C1_IDLE;
#ifdef USE_MPTT_WRAPPER
        I2C1_SlaveStopCallBack();
#endif
    }

    /* When Host wrote 2bytes(register address) and read back 1 byte */

    // [0x8, 0x9, 0x29, 0x29, 0x28, (0x0D, 0x15), (0x2C, 0x34)]
    // 0x8: START -> Start
    // 0x9: START + BF
    // 0x29: DATA + START + (W) + BF
    // 0x29: DATA + START + (W) + BF
    // 0x28: DATA + START + (W) -> Restart
    //  Refused:
    //  -> 0xD: START + R + BF
    //  -> 0x15: STOP + R + BF
    //  Repond:
    //  -> 0x2C: DATA + START + R
    //  -> 0x34: DATA + STOP + R

    if (i2c1SlaveState >= I2C1_START)
    {
        if (I2C1_SlaveIsData())
        {
            if (i2c1SlaveState >= I2C1_ADDR)
            {
                i2c1SlaveState = I2C1_DATA;
                // I2C BUS Read
                if (I2C1_SlaveBusIsRead())
                {
                    if (I2C1_SlaveIsTxBufEmpty())
                    {
                        I2C1_SlaveWrCallBack();
                    }
                }
                else
                {
                    if (I2C1_SlaveIsRxBufFull())
                    {
                        I2C1_SlaveRdCallBack();
                    }
                    else
                    {
                        i2c1SlaveState = I2C1_START; // Restart
                    }
                }
            }
        }
        else
        {
            if (i2c1SlaveState == I2C1_START)
            {
                if (I2C1_SlaveIsRxBufFull())
                {
                    if ((I2C1_SlaveGetRxData() & I2C1_SlaveGetMask()) == I2C1_SlaveGetAddr())
                    {
                        i2c1SlaveState = I2C1_ADDR;
                        I2C1_SlaveAddrCallBack();
                    }
                }

                if (i2c1SlaveState == I2C1_ADDR)
                {
                    if (I2C1_SlaveBusIsRead())
                    {
                        if (I2C1_SlaveIsTxBufEmpty())
                        {
                            I2C1_SlaveWrCallBack();
                        }
                    }
                }
            }
        }
    }

    I2C1_SlaveReleaseClock();
}

// Common Event Interrupt Handlers
void I2C1_SlaveSetIsrHandler(i2c1InterruptHandler handler)
{
    MSSP1_InterruptHandler = handler;
}

// Read Event Interrupt Handlers
void I2C1_SlaveSetReadIntHandler(i2c1InterruptHandler handler)
{
    I2C1_SlaveRdInterruptHandler = handler;
}

static void I2C1_SlaveRdCallBack()
{
    // Add your custom callback code here
    if (I2C1_SlaveRdInterruptHandler)
    {
        I2C1_SlaveRdInterruptHandler();
    }
}

static void I2C1_SlaveDefRdInterruptHandler()
{
    i2c1RdData = I2C1_SlaveGetRxData();
}

// Write Event Interrupt Handlers
void I2C1_SlaveSetWriteIntHandler(i2c1InterruptHandler handler)
{
    I2C1_SlaveWrInterruptHandler = handler;
}

// Stop Event Interrupt Handlers
#ifdef USE_MPTT_WRAPPER
void I2C1_SlaveSetStopIntHandler(i2c1InterruptHandler handler)
{
    I2C1_SlaveStopInterruptHandler = handler;
}
#endif

static void I2C1_SlaveWrCallBack()
{
    // Add your custom callback code here
    if (I2C1_SlaveWrInterruptHandler)
    {
        I2C1_SlaveWrInterruptHandler();
    }
}

static void I2C1_SlaveDefWrInterruptHandler()
{
    I2C1_SlaveSendTxData(i2c1WrData);
}

// ADDRESS Event Interrupt Handlers
void I2C1_SlaveSetAddrIntHandler(i2c1InterruptHandler handler)
{
    I2C1_SlaveAddrInterruptHandler = handler;
}

static void I2C1_SlaveAddrCallBack()
{
    // Add your custom callback code here
    if (I2C1_SlaveAddrInterruptHandler)
    {
        I2C1_SlaveAddrInterruptHandler();
    }
}

static void I2C1_SlaveDefAddrInterruptHandler()
{
    i2c1SlaveAddr = I2C1_SlaveGetRxData();
}

// Write Collision Event Interrupt Handlers
void I2C1_SlaveSetWrColIntHandler(i2c1InterruptHandler handler)
{
    I2C1_SlaveWrColInterruptHandler = handler;
}

static void I2C1_SlaveWrColCallBack()
{
    // Add your custom callback code here
    if (I2C1_SlaveWrColInterruptHandler)
    {
        I2C1_SlaveWrColInterruptHandler();
    }
}

static void I2C1_SlaveDefWrColInterruptHandler()
{
}

// Bus Collision Event Interrupt Handlers
void I2C1_SlaveSetBusColIntHandler(i2c1InterruptHandler handler)
{
    I2C1_SlaveBusColInterruptHandler = handler;
}

static void I2C1_SlaveBusColCallBack()
{
    // Add your custom callback code here
    if (I2C1_SlaveBusColInterruptHandler)
    {
        I2C1_SlaveBusColInterruptHandler();
    }
}

static void I2C1_SlaveDefBusColInterruptHandler()
{
}

#ifdef USE_MPTT_WRAPPER
static void I2C1_SlaveStopCallBack()
{
    // Add your custom callback code here
    if (I2C1_SlaveStopInterruptHandler)
    {
        I2C1_SlaveStopInterruptHandler();
    }
}
#endif

static inline bool I2C1_SlaveOpen()
{
    if (!SSP1CON1bits.SSPEN)
    {
        // SSPEN enabled;
        SSP1CON1bits.SSPEN = 1;
        return true;
    }

    return false;
}

static inline void I2C1_SlaveClose()
{
    SSP1CON1bits.SSPEN = 0;
}

static inline void I2C1_SlaveSetSlaveAddr(uint8_t slaveAddr)
{
    SSP1ADD = (uint8_t)(slaveAddr << 1);
}

static inline void I2C1_SlaveSetSlaveMask(uint8_t maskAddr)
{
    SSP1MSK = (uint8_t)(maskAddr << 1);
}

static inline void I2C1_SlaveEnableIrq()
{
    // [SSPxIF] Synchronous Serial Port ‘x’ Interrupt Flag bit
    PIE3bits.SSP1IE = 1;

    // [BCLxIF] MSSPx Bus Collision Interrupt Flag bit
    PIE3bits.BCL1IE = 1;
}

static inline bool I2C1_SlaveIsData()
{
    return (SSP1STATbits.D_nA);
}

static inline bool I2C1_SlaveBusIsRead()
{
    return (SSP1STATbits.R_nW);
}

static inline void I2C1_SlaveClearIrq()
{

    // [SSPxIF] Synchronous Serial Port ‘x’ Interrupt Flag bit
    PIR3bits.SSP1IF = 0;

    // [BCLxIF] MSSPx Bus Collision Interrupt Flag bit
    PIR3bits.BCL1IF = 0;
}

static inline void I2C1_SlaveReleaseClock()
{
    SSP1CON1bits.CKP = 1;
}

static inline bool I2C1_SlaveIsWriteCollision()
{
    return SSP1CON1bits.WCOL;
}

static inline void I2C1_SlaveIsClearCollision()
{
    SSP1CON1bits.WCOL = 0;
}

static inline bool I2C1_SlaveIsAddr()
{
    return !(SSP1STATbits.D_nA);
}

static inline void I2C1_SlaveRestart(void)
{
    SSP1CON2bits.RSEN = 1;
}

#ifdef USE_MPTT_WRAPPER
static inline bool I2C1_SlaveIsStart()
{
    return SSP1STATbits.S;
}

static inline bool I2C1_SlaveIsStop()
{
    return SSP1STATbits.P;
}
#endif

static inline bool I2C1_SlaveIsTxBufEmpty()
{
    return !SSP1STATbits.BF;
}

static inline bool I2C1_SlaveIsRxBufFull()
{
    return SSP1STATbits.BF;
}

static inline void I2C1_SlaveSendTxData(uint8_t data)
{
    SSP1BUF = data;
}

static inline uint8_t I2C1_SlaveGetRxData()
{
    return SSP1BUF;
}

static inline uint8_t I2C1_SlaveGetAddr()
{
    return SSP1ADD;
}

static inline uint8_t I2C1_SlaveGetMask()
{
    return SSP1MSK;
}

static inline void I2C1_SlaveSendAck()
{
    SSP1CON2bits.ACKDT = 0;
    SSP1CON2bits.ACKEN = 1;
}

static inline void I2C1_SlaveSendNack()
{
    SSP1CON2bits.ACKDT = 1;
    SSP1CON2bits.ACKEN = 1;
}

static inline bool I2C1_SlaveIsOverFlow()
{
    return SSP1CON1bits.SSPOV;
}