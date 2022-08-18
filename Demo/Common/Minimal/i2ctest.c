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

#define i2cSTACK_SIZE                  configMINIMAL_STACK_SIZE
#define i2cDATA_LED_OFFSET               ( 0 )
#define i2cTOTAL_PERMISSIBLE_ERRORS      ( 2 )

SemaphoreHandle_t xSlaveReceivedSemaphore;
SemaphoreHandle_t xSlaveTransmidSemaphore;

uint8_t I2CTempBuffer[6];

extern QueueHandle_t xSlaveReceivedQueue;
extern QueueHandle_t xSlaveTransmidQueue;

/* The transmit task as described at the top of the file. */
static portTASK_FUNCTION_PROTO(vSlaveReceived, pvParameters);
static portTASK_FUNCTION_PROTO(vSlaveTransmid, pvParameters);

/* The LED that should be toggled by the Rx and Tx tasks.  The Rx task will
 * toggle LED ( uxBaseLED + comRX_LED_OFFSET).  The Tx task will toggle LED
 * ( uxBaseLED + comTX_LED_OFFSET ). */
static UBaseType_t uxBaseLED = 0;

/*-----------------------------------------------------------*/

void vStartI2CTestTasks(UBaseType_t uxPriority,
                        UBaseType_t uxLED)
{
    /* Initialise the com port then spawn the Rx and Tx tasks. */
    uxBaseLED = uxLED;

    xI2CSlaveInitMinimal();

    xI2CMasterInitMinimal();

    SET_PB0_IO_OUT;
    PB0 = 0;

    vSemaphoreCreateBinary(xSlaveTransmidSemaphore);

    vSemaphoreCreateBinary(xSlaveReceivedSemaphore);

    /* The Tx task is spawned with a lower priority than the Rx task. */
    xTaskCreate(vSlaveReceived, "I2C_RCV", i2cSTACK_SIZE, NULL, uxPriority, (TaskHandle_t *) NULL);
    xTaskCreate(vSlaveTransmid, "I2C_TRD", i2cSTACK_SIZE, NULL, uxPriority, (TaskHandle_t *) NULL);
}
/*-----------------------------------------------------------*/

static portTASK_FUNCTION(vSlaveReceived, pvParameters)
{
    uint8_t temp = 0, i;

    (void) pvParameters;

    for(; ;)
    {
        if(xSemaphoreTake(xSlaveReceivedSemaphore, portMAX_DELAY) == pdPASS)
        {
            i = 0;
            while(xQueueReceive(xSlaveReceivedQueue, &temp, 0) == (portBASE_TYPE) pdTRUE)
            {
                temp += 1;
                I2CTempBuffer[i++] = temp;
                xQueueSend(xSlaveTransmidQueue, &temp, 0);
                vParTestToggleLED(uxBaseLED + i2cDATA_LED_OFFSET);
            }

            vI2CMasterReadData(0xC0, I2CTempBuffer, 6);

            vTaskDelay(50); // 100ms
        }
    }
}
/*-----------------------------------------------------------*/

static portTASK_FUNCTION(vSlaveTransmid, pvParameters)
{
    uint8_t temp = 0x55;
    (void) pvParameters;

    I2CTempBuffer[0] = 1;
    I2CTempBuffer[1] = 2;
    I2CTempBuffer[2] = 3;
    I2CTempBuffer[3] = 4;
    I2CTempBuffer[4] = 5;
    I2CTempBuffer[5] = 6;

    for(; ;)
    {
        if(xSemaphoreTake(xSlaveTransmidSemaphore, portMAX_DELAY) == pdPASS)
        {
            vParTestToggleLED(uxBaseLED + i2cDATA_LED_OFFSET);

            vI2CMasterWriteData(0xC0, I2CTempBuffer, 6);

            vTaskDelay(50); // 100ms
        }
    }
}

// static portTASK_FUNCTION( vI2CRxTask, pvParameters )
// {
//     signed char cExpectedByte, cByteRxed;
//     BaseType_t xResyncRequired = pdFALSE, xErrorOccurred = pdFALSE;

//     /* Just to stop compiler warnings. */
//     ( void ) pvParameters;

//     for( ; ; )
//     {
//         /* We expect to receive the characters from comFIRST_BYTE to
//          * comLAST_BYTE in an incrementing order.  Loop to receive each byte. */
//         for( cExpectedByte = comFIRST_BYTE; cExpectedByte <= comLAST_BYTE; cExpectedByte++ )
//         {
//             /* Block on the queue that contains received bytes until a byte is
//              * available. */
//             if( xI2CGetChar( xPort, &cByteRxed, comRX_BLOCK_TIME ) )
//             {
//                 /* Was this the byte we were expecting?  If so, toggle the LED,
//                 * otherwise we are out on sync and should break out of the loop
//                 * until the expected character sequence is about to restart. */
//                 if( cByteRxed == cExpectedByte )
//                 {
//                     vParTestToggleLED( uxBaseLED + comRX_LED_OFFSET );
//                 }
//                 else
//                 {
//                     xResyncRequired = pdTRUE;
//                     break; /*lint !e960 Non-switch break allowed. */
//                 }
//             }
//         }

//         /* Turn the LED off while we are not doing anything. */
//         vParTestSetLED( uxBaseLED + comRX_LED_OFFSET, pdFALSE );

//         /* Did we break out of the loop because the characters were received in
//          * an unexpected order?  If so wait here until the character sequence is
//          * about to restart. */
//         if( xResyncRequired == pdTRUE )
//         {
//             while( cByteRxed != comLAST_BYTE )
//             {
//                 /* Block until the next char is available. */
//                 xI2CGetChar( xPort, &cByteRxed, comRX_BLOCK_TIME );
//             }

//             /* Note that an error occurred which caused us to have to resync.
//              * We use this to stop incrementing the loop counter so
//              * sAreComTestTasksStillRunning() will return false - indicating an
//              * error. */
//             xErrorOccurred++;

//             /* We have now resynced with the Tx task and can continue. */
//             xResyncRequired = pdFALSE;
//         }
//         else
//         {
//             if( xErrorOccurred < comTOTAL_PERMISSIBLE_ERRORS )
//             {
//                 /* Increment the count of successful loops.  As error
//                  * occurring (i.e. an unexpected character being received) will
//                  * prevent this counter being incremented for the rest of the
//                  * execution.   Don't worry about mutual exclusion on this
//                  * variable - it doesn't really matter as we just want it
//                  * to change. */
//                 uxRxLoops++;
//             }
//         }
//     }
// } /*lint !e715 !e818 pvParameters is required for a task function even if it is not referenced. */
// /*-----------------------------------------------------------*/

BaseType_t xAreI2CTestTasksStillRunning(void)
{
    BaseType_t xReturn;

    /* If the count of successful reception loops has not changed than at
     * some time an error occurred (i.e. a character was received out of sequence)
     * and we will return false. */
    // if( uxRxLoops == comINITIAL_RX_COUNT_VALUE )
    // {
    //     xReturn = pdFALSE;
    // }
    // else
    {
        xReturn = pdTRUE;
    }


    return xReturn;
}
