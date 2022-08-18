/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://aws.amazon.com/freertos
 *
 */


/* BASIC INTERRUPT DRIVEN I2C PORT DRIVER FOR DEMO PURPOSES */
#include <stdlib.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "i2c_slave.h"

#define I2C_SLAVE_TRANSMID_BUFFER_SIZE  6
#define I2C_SLAVE_RECEIVED_BUFFER_SIZE  6

//#define I2C_PORT 1 // PE4/5
#define I2C_PORT 0 // PC4/5
#define I2C_ADDRESS 0xC0

QueueHandle_t xSlaveReceivedQueue;
QueueHandle_t xSlaveTransmidQueue;

static uint8_t I2CSlaveTransmidBufferIndex;
static uint8_t I2CSlaveReceivedBufferIndex;

extern SemaphoreHandle_t xSlaveReceivedSemaphore;
extern SemaphoreHandle_t xSlaveTransmidSemaphore;

/*-----------------------------------------------------------*/

void xI2CSlaveInitMinimal(void)
{
    uint8_t ucOriginalSFRPage;

    ucOriginalSFRPage = SFRPAGE;

    portENTER_CRITICAL();
    {
        SFRPAGE = 0;
        I2CSlaveReceivedBufferIndex = 0;
        I2CSlaveTransmidBufferIndex = 0;

        xSlaveReceivedQueue = xQueueCreate(I2C_SLAVE_TRANSMID_BUFFER_SIZE, (unsigned portBASE_TYPE) sizeof(uint8_t));
        xSlaveTransmidQueue = xQueueCreate(I2C_SLAVE_RECEIVED_BUFFER_SIZE, (unsigned portBASE_TYPE) sizeof(uint8_t));

        EA = 0;
        IPL1 |= 0x08;
        IRCON1 &= ~0x08;
        REG_ADDR = 0x50;
        REG_DATA = 0x02;

#if I2C_PORT  == 1
        //PE4
        //PE5
        REG_ADDR = 0x1B;
        REG_DATA |= 0x10;
        REG_ADDR = 0x1B;
        REG_DATA |= 0x20;
        REG_ADDR = 0x27;
        REG_DATA &= ~0x01;
        TRISE |= 0x10;
        REG_ADDR = 0x27;
        REG_DATA &= ~0x02;
        TRISE |= 0x20;
        REG_ADDR = 0x34;
        REG_DATA &= ~(0x10);
        REG_DATA |= (0x10 & (1 << 4));
#else
        //PC4
        //PC5
        REG_ADDR = 0x19;
        REG_DATA |= 0x10;
        REG_ADDR = 0x19;
        REG_DATA |= 0x20;
        REG_ADDR = 0x26;
        REG_DATA &= ~0x40;
        TRISC |= 0x10;
        REG_ADDR = 0x26;
        REG_DATA &= ~0x80;
        TRISC |= 0x20;
        REG_ADDR = 0x34;
        REG_DATA &= ~(0x10);
#endif
        IICADD = I2C_ADDRESS;
        IICSTAT = 0x00;
        IICCON |= 0x01;
        IICCON |= 0x02;
        IICCON |= 0x04;
        IICCON &= (~0x08);
        IICCON |= (0x10);
        IEN1 |= 0x08;
        EA = 1;
    }
    portEXIT_CRITICAL();

    SFRPAGE = ucOriginalSFRPage;
}
/*-----------------------------------------------------------*/

void vI2CISR(void) interrupt(10)
{
    uint8_t temp;
    uint8_t ucOriginalSFRPage;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    portENTER_CRITICAL();
    {
        PB0 = 1;
        ucOriginalSFRPage = SFRPAGE;
        PB0 = 0;
        IRCON1 &= ~0x08;
        if(IICSTAT & 0x02)
        {
            IICSTAT &= ~0x02;
        }
        if(IICSTAT & 0x01)
        {
            IICSTAT &= ~0x01;
            temp = IICBUF;
        }
        if((IICSTAT & 0x10) == 0)
        {
            I2CSlaveReceivedBufferIndex = 0;
            I2CSlaveTransmidBufferIndex = 0;

            if(IICSTAT & 0x20)
            {
                if(xQueueReceiveFromISR(xSlaveTransmidQueue, &temp, &xHigherPriorityTaskWoken) == (portBASE_TYPE) pdTRUE)
                {
                    IICBUF = temp;
                }
                IICCON |= 0x04;
            }
            else
            {
                temp = IICBUF;
            }
        }
        else
        {
            if(IICSTAT & 0x20)// RW
            {
                if(xQueueReceiveFromISR(xSlaveTransmidQueue, &temp, &xHigherPriorityTaskWoken) == (portBASE_TYPE) pdTRUE)
                {
                    IICBUF = temp;
                }
                if(++I2CSlaveTransmidBufferIndex >= I2C_SLAVE_TRANSMID_BUFFER_SIZE)
                {
                    I2CSlaveTransmidBufferIndex = 0;
                }
                IICCON |= 0x04;
            }
            else
            {
                if(IICSTAT & 0x08) // BF
                {
                    temp = IICBUF;
                    xQueueSendFromISR(xSlaveReceivedQueue, &temp, &xHigherPriorityTaskWoken);
                    if(++I2CSlaveReceivedBufferIndex >= I2C_SLAVE_RECEIVED_BUFFER_SIZE)
                    {
                        I2CSlaveReceivedBufferIndex = 0;
                        PB0 = 1;
                        xSemaphoreGiveFromISR(xSlaveReceivedSemaphore, &xHigherPriorityTaskWoken);
                        PB0 = 0;
                    }
                }
                else
                {              
                    PB0 = 1;
                    xSemaphoreGiveFromISR(xSlaveTransmidSemaphore, &xHigherPriorityTaskWoken);
                    PB0 = 0;
                }
            }
        }
        IICCON |= 0x04;
        if(xHigherPriorityTaskWoken)
        {
            portYIELD();
        }
        SFRPAGE = ucOriginalSFRPage;
    }
    portEXIT_CRITICAL();
}

/*-----------------------------------------------------------*/

void vI2CSlaveClose(void)
{

}

/*-----------------------------------------------------------*/


