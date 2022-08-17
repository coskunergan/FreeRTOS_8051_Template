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
#include <stdbool.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "i2c_master.h"

/*-----------------------------------------------------------*/

void xI2CMasterInitMinimal(void)
{
    uint8_t ucOriginalSFRPage;

    ucOriginalSFRPage = SFRPAGE;

    portENTER_CRITICAL();
    {
        SFRPAGE = 0;
        OUT_SDA();
        OUT_SCL();
        SET_SDA();
        SET_SCL();
    }
    portEXIT_CRITICAL();

    SFRPAGE = ucOriginalSFRPage;
}

/*-----------------------------------------------------------*/

static void Delay(uint8_t time)// unit: 10us
{
    uint8_t a, b;
    for(b = time; b > 0; b--)
    {
        for(a = 17; a > 0; a--);
    }
}

/*-----------------------------------------------------------*/

static void Start()
{
    OUT_SDA();
    SET_SDA();
    SET_SCL();
    Delay(1);
    CLR_SDA();
    Delay(1);
    CLR_SCL();
    Delay(1);
}

/*-----------------------------------------------------------*/

static void Stop()
{
    CLR_SCL();
    OUT_SDA();
    CLR_SDA();
    Delay(1);
    SET_SCL();
    Delay(1);
    SET_SDA();
}

/*-----------------------------------------------------------*/

static uint8_t ReceiveByte()
{
    uint8_t i;
    uint8_t buffer = 0;

    IN_SDA();

    for(i = 0; i < 8; i++)
    {
        CLR_SCL();
        Delay(2);
        SET_SCL();
        Delay(2);
        if(SDA())
        {
            buffer |= 0x01;
        }
        if(i < 7)
        {
            buffer <<= 1;
        }
    }

    CLR_SCL();
    OUT_SDA();
    CLR_SDA();

    return (buffer);
}

/*-----------------------------------------------------------*/

static bool SendByteAndGetNACK(uint8_t dataToSend)
{
    uint8_t i;
    bool ack;

    for(i = 0; i < 8; i++)
    {
        CLR_SCL();
        Delay(1);
        if(dataToSend & 0x80)
        {
            SET_SDA();
        }
        else
        {
            CLR_SDA();
        }
        Delay(1);
        SET_SCL();
        Delay(1);

        dataToSend <<= 1;
    }

    CLR_SCL();
    Delay(1);

    IN_SDA();
    Delay(1);
    SET_SCL();
    Delay(2);

    ack = SDA();

    CLR_SCL();
    Delay(1);
    OUT_SDA();
    CLR_SDA();
    Delay(1);

    return (ack);
}

/*-----------------------------------------------------------*/

static void Respond(uint8_t ACKSignal)
{
    OUT_SDA();
    CLR_SDA();
    CLR_SCL();
    if(ACKSignal)
    {
        SET_SDA();
    }
    else
    {
        CLR_SDA();
    }
    Delay(1);
    SET_SCL();
    Delay(1);
    CLR_SCL();
}

/*-----------------------------------------------------------*/

void vI2CMasterWriteData(uint8_t deviceAddr, uint8_t *dataSource, uint8_t lengthOfData)
{
    uint8_t i;

    Start();

    if(SendByteAndGetNACK(deviceAddr & ~0x01))
    {
        Stop();
        return;
    }

    for(i = 0; i < lengthOfData; i++)
    {
        if(SendByteAndGetNACK(dataSource[i]))
        {
            Stop();
            return;
        }
    }
    Stop();
}

/*-----------------------------------------------------------*/

void vI2CMasterReadData(uint8_t deviceAddr, uint8_t *target, uint8_t lengthOfData)
{
    uint8_t i;

    Start();
    if(SendByteAndGetNACK(deviceAddr | 0x01))
    {
        Stop();
        return;
    }

    for(i = 0; i < lengthOfData; i++)
    {
        target[i] = ReceiveByte();
        if(i == (lengthOfData - 1))
        {
            break;
        }
        Respond(ACK);
    }

    Respond(NACK);
    Stop();
}

/*-----------------------------------------------------------*/

void vI2CMasterClose(void)
{

}

/*-----------------------------------------------------------*/





