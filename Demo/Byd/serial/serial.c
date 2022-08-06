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


/* BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR DEMO PURPOSES */
#include <stdlib.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "serial.h"

static QueueHandle_t xRxedChars;
static QueueHandle_t xCharsForTx;

data static unsigned portBASE_TYPE uxTxEmpty;

/*-----------------------------------------------------------*/

xComPortHandle xSerialPortInitMinimal(unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength)
{
    unsigned char ucOriginalSFRPage;

    portENTER_CRITICAL();
    {
        ucOriginalSFRPage = SFRPAGE;

        SFRPAGE = 0;

        uxTxEmpty = pdTRUE;

        /* Create the queues used by the com test task. */
        xRxedChars = xQueueCreate(uxQueueLength, (unsigned portBASE_TYPE) sizeof(char));
        xCharsForTx = xQueueCreate(uxQueueLength, (unsigned portBASE_TYPE) sizeof(char));

        EA = 0;
        IPL2 |= 0x04;
        IRCON2 &= ~0x04;
        REG_ADDR = 0x34;
        REG_DATA &= ~(0x60);
        REG_DATA |= (0x60 & (1 << 5));

        REG_ADDR = 0x1B;
        REG_DATA |= 0x10;
        REG_ADDR = 0x1B;
        REG_DATA |= 0x20;
        REG_ADDR = 0x27;
        REG_DATA &= ~0x01;
        TRISE |= 0x10;
        REG_ADDR = 0x27;
        REG_DATA &= ~0x02;
        TRISE &= ~0x20;

        UART0_BDL = (24000000UL / (16UL * ulWantedBaud));
        UART0_CON2 = ((uint8_t)((uint16_t)(24000000UL / (16UL * ulWantedBaud)) >> 8) & 0x03);
        UART0_CON2 |= (0x08);
        UART0_CON2 |= (0x04);
        UART0_CON1 |= (0x40);
        UART0_CON1 &= (~0x10);
        UART0_CON1 |= (0x20);
        UART0_CON1 &= (~0x04);
        UART0_CON1 |= (0x08);
        UART0_CON1 &= (~0x02);
        UART0_CON1 |= (0x01);
        UART0_STATE &= ((~0x08) & (~0x10));
        IEN2 |= 0x04;

        SFRPAGE = ucOriginalSFRPage;
    }
    portEXIT_CRITICAL();

    /* Unlike some ports, this serial code does not allow for more than one
    com port.  We therefore don't return a pointer to a port structure and can
    instead just return NULL. */
    return NULL;
}
/*-----------------------------------------------------------*/

void vSerialISR(void) interrupt(17)
{
    char cChar;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    /* 8051 port interrupt routines MUST be placed within a critical section
    if taskYIELD() is used within the ISR! */

    portENTER_CRITICAL();
    {
        IRCON2 &= ~0x04;
        if(UART0_STATE & 0x08)
        {
            /* Get the character and post it on the queue of Rxed characters.
            If the post causes a task to wake force a context switch if the woken task
            has a higher priority than the task we have interrupted. */
            cChar = UART0_BUF;
            UART0_STATE = 0x17;
            xQueueSendFromISR(xRxedChars, &cChar, &xHigherPriorityTaskWoken);
        }
        if(UART0_STATE & 0x01)
        {
            UART0_STATE = 0x1E;
        }
        if(UART0_STATE & 0x02)
        {
            UART0_STATE = 0x1D;
        }
        if(UART0_STATE & 0x04)
        {
            UART0_STATE = 0x1B;
        }
        if(UART0_STATE & 0x10)
        {
            UART0_STATE = 0x0F;
            if(xQueueReceiveFromISR(xCharsForTx, &cChar, &xHigherPriorityTaskWoken) == (portBASE_TYPE) pdTRUE)
            {
                /* Send the next character queued for Tx. */
                UART0_BUF = cChar;
            }
            else
            {
                /* Queue empty, nothing to send. */
                uxTxEmpty = pdTRUE;
            }
        }

        if(xHigherPriorityTaskWoken)
        {
            portYIELD();
        }
    }
    portEXIT_CRITICAL();
}


/*-----------------------------------------------------------*/

portBASE_TYPE xSerialGetChar(xComPortHandle pxPort, signed char *pcRxedChar, TickType_t xBlockTime)
{
    /* There is only one port supported. */
    (void) pxPort;

    /* Get the next character from the buffer.  Return false if no characters
    are available, or arrive before xBlockTime expires. */
    if(xQueueReceive(xRxedChars, pcRxedChar, xBlockTime))
    {
        return (portBASE_TYPE) pdTRUE;
    }
    else
    {
        return (portBASE_TYPE) pdFALSE;
    }
}
/*-----------------------------------------------------------*/

portBASE_TYPE xSerialPutChar(xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime)
{
    portBASE_TYPE xReturn;

    /* There is only one port supported. */
    (void) pxPort;

    portENTER_CRITICAL();
    {
        if(uxTxEmpty == pdTRUE)
        {
            UART0_BUF = cOutChar;
            uxTxEmpty = pdFALSE;
            UART0_STATE = 0x0F;            
            xReturn = (portBASE_TYPE) pdTRUE;
        }
        else
        {
            xReturn = xQueueSend(xCharsForTx, &cOutChar, xBlockTime);

            if(xReturn == (portBASE_TYPE) pdFALSE)
            {
                xReturn = (portBASE_TYPE) pdTRUE;
            }
        }
    }
    portEXIT_CRITICAL();

    return xReturn;
}
/*-----------------------------------------------------------*/

void vSerialClose(xComPortHandle xPort)
{
    /* Not implemented in this port. */
    (void) xPort;
}
/*-----------------------------------------------------------*/





