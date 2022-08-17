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

#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#include "FreeRTOS.h"

#define SET_SDA()   DATAF |= 0x20
#define CLR_SDA()   DATAF &= ~0x20
#define OUT_SDA()   /*SET_PE5_PU_ON;*/SET_PF5_IO_OUT
#define IN_SDA()    /*SET_PE5_PU_ON;*/SET_PF5_IO_IN
#define SDA()       DATAE & 0x20

#define SET_SCL()   DATAF |= 0x10
#define CLR_SCL()   DATAF &= ~0x10
#define OUT_SCL()   /*SET_PE4_PU_ON;*/SET_PF4_IO_OUT
#define IN_SCL()    /*SET_PE4_PU_ON;*/SET_PF4_IO_IN

#define NACK    1
#define ACK     0

void xI2CMasterInitMinimal( void );
void vI2CMasterWriteData(uint8_t deviceAddr, uint8_t *dataSource, uint8_t lengthOfData);
void vI2CMasterReadData(uint8_t deviceAddr, uint8_t *target, uint8_t lengthOfData);
void vI2CMasterClose( void );

#endif /* ifndef I2C_MASTER_H */
