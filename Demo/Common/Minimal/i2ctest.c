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


/*
 * This version of comtest. c is for use on systems that have limited stack
 * space and no display facilities.  The complete version can be found in
 * the Demo/Common/Full directory.
 *
 * Creates two tasks that operate on an interrupt driven serial port.  A
 * loopback connector should be used so that everything that is transmitted is
 * also received.  The serial port does not use any flow control.  On a
 * standard 9way 'D' connector pins two and three should be connected together.
 *
 * The first task posts a sequence of characters to the Tx queue, toggling an
 * LED on each successful post.  At the end of the sequence it sleeps for a
 * pseudo-random period before resending the same sequence.
 *
 * The UART Tx end interrupt is enabled whenever data is available in the Tx
 * queue.  The Tx end ISR removes a single character from the Tx queue and
 * passes it to the UART for transmission.
 *
 * The second task blocks on the Rx queue waiting for a character to become
 * available.  When the UART Rx end interrupt receives a character it places
 * it in the Rx queue, waking the second task.  The second task checks that the
 * characters removed from the Rx queue form the same sequence as those posted
 * to the Tx queue, and toggles an LED for each correct character.
 *
 * The receiving task is spawned with a higher priority than the transmitting
 * task.  The receiver will therefore wake every time a character is
 * transmitted so neither the Tx or Rx queue should ever hold more than a few
 * characters.
 *
 */

/* Scheduler include files. */
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Demo program include files. */
#include "i2c_slave.h"
#include "i2c_master.h"
#include "i2ctest.h"
#include "serial.h"
#include "partest.h"

#define i2cSTACK_SIZE                     configMINIMAL_STACK_SIZE
#define i2cDATA_LED_OFFSET               ( 0 )
#define i2cTOTAL_PERMISSIBLE_ERRORS      ( 2 )

#define i2cTX_MAX_BLOCK_TIME           ( ( TickType_t ) 0x96 / portTICK_PERIOD_MS)
#define i2cTX_MIN_BLOCK_TIME           ( ( TickType_t ) 0x32 / portTICK_PERIOD_MS)
#define i2cOFFSET_TIME                 ( ( TickType_t ) 3 )

/* We should find that each character can be queued for Tx immediately and we
 * don't have to block to send. */
#define i2cNO_BLOCK                    ( ( TickType_t ) 0 )

/* The Rx task will block on the Rx queue for a long period. */
#define i2cRX_BLOCK_TIME               ( ( TickType_t ) 0xffff )

#define i2cFIRST_BYTE                  ( 'A' )
#define i2cLAST_BYTE                   ( 'X' )

#define i2cBUFFER_LEN                  ( ( UBaseType_t ) ( i2cLAST_BYTE - i2cFIRST_BYTE ) + ( UBaseType_t ) 1 )
#define i2cINITIAL_RX_COUNT_VALUE      ( 0 )

SemaphoreHandle_t xSlaveReceivedSemaphore;
SemaphoreHandle_t xSlaveTransmidSemaphore;

uint8_t I2CTempBuffer[i2cBUFFER_LEN];

extern QueueHandle_t xSlaveReceivedQueue;
extern QueueHandle_t xSlaveTransmidQueue;

/* The transmit task as described at the top of the file. */
static portTASK_FUNCTION_PROTO(vSlaveReceived, pvParameters);
static portTASK_FUNCTION_PROTO(vSlaveTransmid, pvParameters);
static portTASK_FUNCTION_PROTO(vMasterProccess, pvParameters);

/* The LED that should be toggled by the Rx and Tx tasks.  The Rx task will
 * toggle LED ( uxBaseLED + comRX_LED_OFFSET).  The Tx task will toggle LED
 * ( uxBaseLED + comTX_LED_OFFSET ). */
static UBaseType_t uxBaseLED = 0;

static volatile UBaseType_t uxRxLoops = i2cINITIAL_RX_COUNT_VALUE;

/*-----------------------------------------------------------*/

void vStartI2CTestTasks(UBaseType_t uxPriority,
                        UBaseType_t uxLED)
{
    /* Initialise the com port then spawn the Rx and Tx tasks. */
    uxBaseLED = uxLED;

    xI2CSlaveInitMinimal(i2cBUFFER_LEN);

    xI2CMasterInitMinimal();

    vSemaphoreCreateBinary(xSlaveTransmidSemaphore);

    vSemaphoreCreateBinary(xSlaveReceivedSemaphore);

    /* The Tx task is spawned with a lower priority than the Rx task. */
    xTaskCreate(vSlaveReceived, "I2C_RCV", i2cSTACK_SIZE, NULL, uxPriority, (TaskHandle_t *) NULL);
    xTaskCreate(vSlaveTransmid, "I2C_TRD", i2cSTACK_SIZE, NULL, uxPriority, (TaskHandle_t *) NULL);
    xTaskCreate(vMasterProccess, "MasterTst", i2cSTACK_SIZE, NULL, uxPriority - 1, (TaskHandle_t *) NULL);
}

/*-----------------------------------------------------------*/

static portTASK_FUNCTION(vSlaveTransmid, pvParameters)
{
    uint8_t TempByte;

    (void) pvParameters;

    for(; ;)
    {
        if(xSemaphoreTake(xSlaveTransmidSemaphore, portMAX_DELAY) == pdPASS)
        {
            for(TempByte = i2cFIRST_BYTE; TempByte <= i2cLAST_BYTE; TempByte++)
            {
                xQueueSend(xSlaveTransmidQueue, &TempByte, 0);
                vParTestToggleLED(uxBaseLED + i2cDATA_LED_OFFSET);
            }
        }
    }
}

/*-----------------------------------------------------------*/

static portTASK_FUNCTION(vSlaveReceived, pvParameters)
{
    uint8_t TempByte;
    uint8_t ExpectedByte;
    BaseType_t xErrorOccurred = pdFALSE;

    (void) pvParameters;

    for(; ;)
    {
        if(xSemaphoreTake(xSlaveReceivedSemaphore, portMAX_DELAY) == pdPASS)
        {
            for(ExpectedByte = i2cFIRST_BYTE; ExpectedByte <= i2cLAST_BYTE; ExpectedByte++)
            {
                if(xQueueReceive(xSlaveReceivedQueue, &TempByte, 0) == (portBASE_TYPE) pdTRUE)
                {
                    if(TempByte != ExpectedByte)
                    {
                        xErrorOccurred++;
                    }
                    vParTestToggleLED(uxBaseLED + i2cDATA_LED_OFFSET);
                }
            }
        }
        if(xErrorOccurred < i2cTOTAL_PERMISSIBLE_ERRORS)
        {
            /* Increment the count of successful loops.  As error
             * occurring (i.e. an unexpected character being received) will
             * prevent this counter being incremented for the rest of the
             * execution.   Don't worry about mutual exclusion on this
             * variable - it doesn't really matter as we just want it
             * to change. */
            uxRxLoops++;
        }
    }
}

/*-----------------------------------------------------------*/

static portTASK_FUNCTION(vMasterProccess, pvParameters)
{
    TickType_t xTimeToWait;

    (void) pvParameters;

    for(; ;)
    {
        vI2CMasterReadData(0xC0, I2CTempBuffer, i2cBUFFER_LEN);
        /* We have posted all the characters in the string - wait before
         * re-sending.  Wait a pseudo-random time as this will provide a better
         * test. */
        xTimeToWait = xTaskGetTickCount() + i2cOFFSET_TIME;

        /* Make sure we don't wait too long... */
        xTimeToWait %= i2cTX_MAX_BLOCK_TIME;

        /* ...but we do want to wait. */
        if(xTimeToWait < i2cTX_MIN_BLOCK_TIME)
        {
            xTimeToWait = i2cTX_MIN_BLOCK_TIME;
        }

        vTaskDelay(xTimeToWait);

        vI2CMasterWriteData(0xC0, I2CTempBuffer, i2cBUFFER_LEN);

        /* We have posted all the characters in the string - wait before
         * re-sending.  Wait a pseudo-random time as this will provide a better
         * test. */
        xTimeToWait = xTaskGetTickCount() + i2cOFFSET_TIME;

        /* Make sure we don't wait too long... */
        xTimeToWait %= i2cTX_MAX_BLOCK_TIME;

        /* ...but we do want to wait. */
        if(xTimeToWait < i2cTX_MIN_BLOCK_TIME)
        {
            xTimeToWait = i2cTX_MIN_BLOCK_TIME;
        }

        vTaskDelay(xTimeToWait);
    }
}

/*-----------------------------------------------------------*/

BaseType_t xAreI2CTestTasksStillRunning(void)
{
    BaseType_t xReturn;

    /* If the count of successful reception loops has not changed than at
     * some time an error occurred (i.e. a character was received out of sequence)
     * and we will return false. */
    if(uxRxLoops == i2cINITIAL_RX_COUNT_VALUE)
    {
        xReturn = pdFALSE;
    }
    else
    {
        xReturn = pdTRUE;
    }
    /* Reset the count of successful Rx loops.  When this function is called
     * again we expect this to have been incremented. */
    uxRxLoops = i2cINITIAL_RX_COUNT_VALUE;

    return xReturn;
}
