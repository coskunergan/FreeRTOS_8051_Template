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
#include "BF7615BM44LJTX.h"

#include "FreeRTOS.h"
#include "task.h"
#include "partest.h"

#define partstPUSH_PULL			( ( unsigned char ) 0xff )
#define partstALL_OUTPUTS_OFF	( ( unsigned char ) 0xff )

/* LED to output is dependent on how the LED's are wired. */
#define partstOUTPUT_0			( ( unsigned char ) 0x02 )
#define partstOUTPUT_1			( ( unsigned char ) 0x08 )
#define partstOUTPUT_2			( ( unsigned char ) 0x20 )
#define partstOUTPUT_3			( ( unsigned char ) 0x01 )
#define partstOUTPUT_4			( ( unsigned char ) 0x04 )
#define partstOUTPUT_5			( ( unsigned char ) 0x10 )
#define partstOUTPUT_6			( ( unsigned char ) 0x40 )
#define partstOUTPUT_7			( ( unsigned char ) 0x80 )

/*-----------------------------------------------------------
 * Simple parallel port IO routines.
 *-----------------------------------------------------------*/

void vParTestInitialise( void )
{
    REG_ADDR = 0x29;
    REG_DATA &= ~0x01;
    TRISA &= ~partstALL_OUTPUTS_OFF;
    DATAA = 0;
}
/*-----------------------------------------------------------*/

void vParTestSetLED( unsigned portBASE_TYPE uxLED, portBASE_TYPE xValue )
{
portBASE_TYPE xError = pdFALSE;

	vTaskSuspendAll();
	{
		// if( xValue == pdFALSE )
		// {
		// 	switch( uxLED )
		// 	{
		// 		case 0	:	DATAA |= partstOUTPUT_0;
		// 					break;
		// 		case 1	:	DATAA |= partstOUTPUT_1;
		// 					break;
		// 		case 2	:	DATAA |= partstOUTPUT_2;
		// 					break;
		// 		case 3	:	DATAA |= partstOUTPUT_3;
		// 					break;
		// 		case 4	:	DATAA |= partstOUTPUT_4;
		// 					break;
		// 		case 5	:	DATAA |= partstOUTPUT_5;
		// 					break;
		// 		case 6	:	DATAA |= partstOUTPUT_6;
		// 					break;
		// 		case 7	:	DATAA |= partstOUTPUT_7;
		// 					break;
		// 		default	:	/* There are no other LED's wired in. */
		// 					xError = pdTRUE;
		// 					break;
		// 	}
		// }
		// else
		// {
		// 	switch( uxLED )
		// 	{
		// 		case 0	:	DATAA &= ~partstOUTPUT_0;
		// 					break;
		// 		case 1	:	DATAA &= ~partstOUTPUT_1;
		// 					break;
		// 		case 2	:	DATAA &= ~partstOUTPUT_2;
		// 					break;
		// 		case 3	:	DATAA &= ~partstOUTPUT_3;
		// 					break;
		// 		case 4	:	DATAA &= ~partstOUTPUT_4;
		// 					break;
		// 		case 5	:	DATAA &= ~partstOUTPUT_5;
		// 					break;
		// 		case 6	:	DATAA &= ~partstOUTPUT_6;
		// 					break;
		// 		case 7	:	DATAA &= ~partstOUTPUT_7;
		// 					break;
		// 		default	:	/* There are no other LED's wired in. */
		// 					break;
		// 	}
		//}
	}
	xTaskResumeAll();
}
/*-----------------------------------------------------------*/

void vParTestToggleLED( unsigned portBASE_TYPE uxLED )
{
	unsigned char ucBit = 0;
	portBASE_TYPE xError = pdFALSE;

	vTaskSuspendAll();
	{
		// switch( uxLED )
		// {
		// 	case 0	:	ucBit = partstOUTPUT_0;
		// 				break;
		// 	case 1	:	ucBit = partstOUTPUT_1;
		// 				break;
		// 	case 2	:	ucBit = partstOUTPUT_2;
		// 				break;
		// 	case 3	:	ucBit = partstOUTPUT_3;
		// 				break;
		// 	case 4	:	ucBit = partstOUTPUT_4;
		// 				break;
		// 	case 5	:	ucBit = partstOUTPUT_5;
		// 				break;
		// 	case 6	:	ucBit = partstOUTPUT_6;
		// 				break;
		// 	case 7	:	ucBit = partstOUTPUT_7;
		// 				break;
		// 	default	:	/* There are no other LED's wired in. */
		// 				xError = pdTRUE;
		// 				break;
		// }

		// if( xError != pdTRUE )
		// {
		// 	if( DATAA & ucBit )
		// 	{
		// 		DATAA &= ~ucBit;
		// 	}
		// 	else
		// 	{
		// 		DATAA |= ucBit;
		// 	}
		// }
	}
	xTaskResumeAll();
}


