/*-------------------------------------------------------------------------
   Register Declarations for the Byd BF7615BM44LJTX Processor Range

   Copyright (C) 2003 - Maarten Brock, sourceforge.brock@dse.nl

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
-------------------------------------------------------------------------*/

#ifndef BF7615BM44LJTX_H
#define BF7615BM44LJTX_H

#include <compiler.h>

#define _nop_() __asm \
                  nop \
               __endasm

#define _asm      __asm
#define _endasm   __endasm

// keywords
#define interrupt __interrupt
#define using __using
#define _at_ __at
#define _priority_
#define _task_
#define _naked

// storage type
#define reentrant __reentrant
#define compact
#define small __near
#define large __far
#define data __data
#define bdata
#define idata __idata
#define pdata __pdata
#define xdata __xdata
#define code __code

/*  ----------------BYTE Register------------------  */
SFR(DATAB, 0x80);
SFR(SP, 0x81);
SFR(DPL, 0x82);
SFR(DPH, 0x83);
SFR(TIMER3_CFG, 0x84);
SFR(TIMER3_SET_H, 0x85);
SFR(TIMER3_SET_L, 0x86);
SFR(PCON, 0x87);
SFR(TCON, 0x88);
SFR(TMOD, 0x89);
SFR(TL0, 0x8A);
SFR(TL1, 0x8B);
SFR(TH0, 0x8C);
SFR(TH1, 0x8D);
SFR(SOFT_RST, 0x8E);
SFR(DATAC, 0x90);
SFR(WDT_CTRL, 0x91);
SFR(WDT_EN, 0x92);
SFR(TIMER2_CFG, 0x93);
SFR(TIMER2_SET_H, 0x94);
SFR(TIMER2_SET_L, 0x95);
SFR(REG_ADDR, 0x96);
SFR(REG_DATA, 0x97);
SFR(SCI_S1, 0x98);
SFR(PWM0_L_L, 0x99);
SFR(PWM0_L_H, 0x9A);
SFR(PWM0_H_L, 0x9B);
SFR(PWM0_H_H, 0x9C);
SFR(PWM1_L_L, 0x9D);
SFR(PWM1_L_H, 0x9E);
SFR(PWM1_H_L, 0x9F);
SFR(P2_XH, 0xA0); // ????
SFR(PWM1_H_H, 0xA1);
SFR(PWM2_L_L, 0xA2);
SFR(PWM2_L_H, 0xA3);
SFR(PWM2_H_L, 0xA4);
SFR(PWM2_H_H, 0xA5);
SFR(PWM3_L_L, 0xA6);
SFR(PWM3_L_H, 0xA7);
SFR(IE, 0xA8);
SFR(PWM3_H_L, 0xA9);
SFR(PWM3_H_H, 0xAA);
SFR(CSD_RAWDATAL, 0xAB);
SFR(CSD_RAWDATAH, 0xAC);
SFR(SYS_CLK_CFG, 0xAD);
SFR(INT_PE_STAT, 0xAE);
SFR(SCAN_START, 0xAF);
SFR(DATAE, 0xB0);
SFR(DP_CON, 0xB1);
SFR(DP_MODE, 0xB2);
SFR(SCAN_WIDTH, 0xB3);
SFR(LED2_WIDTH, 0xB4);
SFR(SPI_CFG1, 0xB5);
SFR(SPI_CFG2, 0xB6);
SFR(IPL0, 0xB8);
SFR(DP_CON1, 0xB9);
SFR(SCI_C2, 0xBA);
SFR(SCI_C3, 0xBB);
SFR(SCI_S2, 0xBC);
SFR(SCI_D, 0xBD);
SFR(SPI_STATE, 0xBE);
SFR(SPI_SPID, 0xBF);
SFR(DATAF, 0xC0);
SFR(ADC_SPT, 0xC1);
SFR(SCI_INT_CLR, 0xC2);
SFR(ADC_SCAN_CFG, 0xC3);
SFR(ADCCKC, 0xC4);
SFR(ADC_RDATAH, 0xC5);
SFR(ADC_RDATAL, 0xC6);
SFR(EXINT_STAT, 0xC7);
SFR(DATAG, 0xC8);
SFR(CSD_START, 0xC9);
SFR(PULL_I_SEL, 0xCA);
SFR(SNS_SCAN_CFG1, 0xCB);
SFR(SNS_SCAN_CFG2, 0xCC);
SFR(SNS_SCAN_CFG3, 0xCD);
SFR(SPROG_ADDR_H, 0xCE);
SFR(SPROG_ADDR_L, 0xCF);
SFR(PSW, 0xD0);
SFR(SPROG_DATA, 0xD1);
SFR(SPROG_CMD, 0xD2);
SFR(SPROG_TIM, 0xD3);
SFR(SPROG_RDATA, 0xD4);
SFR(INT_POBO_STAT, 0xD5);
SFR(UART1_BDL, 0xD6);
SFR(UART1_CON1, 0xD7);
SFR(DATAH, 0xD8);
SFR(UART1_CON2, 0xD9);
SFR(UART1_STATE, 0xDA);
SFR(UART1_BUF, 0xDB);
SFR(UART0_BDL, 0xDC);
SFR(UART0_CON1, 0xDD);
SFR(UART0_CON2, 0xDE);
SFR(UART0_STATE, 0xDF);
SFR(ACC, 0xE0);
SFR(IRCON2, 0xE1);
SFR(UART0_BUF, 0xE2);
SFR(IICADD, 0xE3);
SFR(IICBUF, 0xE4);
SFR(IICCON, 0xE5);
SFR(IEN1, 0xE6);
SFR(IEN2, 0xE7);
SFR(IICSTAT, 0xE8);
SFR(IICBUFFER, 0xE9);
SFR(TRISA, 0xEA);
SFR(TRISB, 0xEB);
SFR(TRISC, 0xEC);
SFR(SCI_C1, 0xED);
SFR(TRISE, 0xEE);
SFR(TRISF, 0xEF);
SFR(B, 0xF0);
SFR(IRCON1, 0xF1);
SFR(TRISG, 0xF2);
SFR(IPL2, 0xF4);
SFR(IPL1, 0xF6);
SFR(TRISH, 0xF7);
SFR(DATAA, 0xF8);
SFR(PCD_BUF1, 0xF9);
SFR(PCD_BUF2, 0xFA);
SFR(PCD_CFG, 0xFB);
SFR(PCD_CMD, 0xFC);
SFR(PCD_CFG2, 0xFD);
SFR(PCD_BIT_ER, 0xFE);
SFR(PCD_CFG3, 0xFF);

SFR(SFRPAGE, 0x96);

/*  ----------------BYTE Register------------------  */

/*  ----------------Bit Register------------------  */

/*  DATAA  */
SBIT(PA0, 0xF8, 0);
SBIT(PA1, 0xF8, 1);
SBIT(PA2, 0xF8, 2);
SBIT(PA3, 0xF8, 3);

/*  DATAB  */
SBIT(PB0, 0x80, 0);
SBIT(PB1, 0x80, 1);
SBIT(PB2, 0x80, 2);
SBIT(PB3, 0x80, 3);
SBIT(PB4, 0x80, 4);
SBIT(PB5, 0x80, 5);
SBIT(PB6, 0x80, 6);
SBIT(PB7, 0x80, 7);

/*  TCON  */
SBIT(IE0, 0x88, 0);
SBIT(IE1, 0x88, 1);
SBIT(TR0, 0x88, 2);
SBIT(TF0, 0x88, 3);
SBIT(TR1, 0x88, 4);
SBIT(TF1, 0x88, 5);

/*  DATAC  */
SBIT(PC0, 0x90, 0);
SBIT(PC1, 0x90, 1);
SBIT(PC2, 0x90, 2);
SBIT(PC3, 0x90, 3);
SBIT(PC4, 0x90, 4);
SBIT(PC5, 0x90, 5);
SBIT(PC6, 0x90, 6);
SBIT(PC7, 0x90, 7);

/*  IEN0  */
SBIT(EX0, 0xA8, 0);
SBIT(ET0, 0xA8, 1);
SBIT(EX1, 0xA8, 2);
SBIT(ET1, 0xA8, 3);
SBIT(EA, 0xA8, 7);

/*  DATAE  */
SBIT(PE0, 0xB0, 0);
SBIT(PE1, 0xB0, 1);
SBIT(PE2, 0xB0, 2);
SBIT(P3, 0xB0, 3);
SBIT(PE4, 0xB0, 4);
SBIT(PE5, 0xB0, 5);
SBIT(PE6, 0xB0, 6);
SBIT(PE7, 0xB0, 7);

/*  IPL0  */
SBIT(PX0, 0xB8, 0);
SBIT(PT0, 0xB8, 1);
SBIT(PX2, 0xB8, 2);
SBIT(PT1, 0xB8, 3);

/*  DATAF  */
SBIT(PF0, 0xC0, 0);
SBIT(PF1, 0xC0, 1);
SBIT(PF2, 0xC0, 2);
SBIT(PF3, 0xC0, 3);
SBIT(PF4, 0xC0, 4);
SBIT(PF5, 0xC0, 5);
SBIT(PF6, 0xC0, 6);
SBIT(PF7, 0xC0, 7);

/*  DATAG  */
SBIT(PG0, 0xC8, 0) ;
SBIT(PG1, 0xC8, 1) ;
SBIT(PG2, 0xC8, 2) ;
SBIT(PG3, 0xC8, 3) ;

/*  PSW  */
SBIT(P, 0xD0, 0);
SBIT(F1, 0xD0, 1);
SBIT(OV, 0xD0, 2);
SBIT(RS0, 0xD0, 3);
SBIT(RS1, 0xD0, 4);
SBIT(F0, 0xD0, 5);
SBIT(AC, 0xD0, 6);
SBIT(CY, 0xD0, 7);

/*  DATAH  */
SBIT(PH0, 0xD8, 0);
SBIT(PH1, 0xD8, 1);
SBIT(PH2, 0xD8, 2);
SBIT(PH3, 0xD8, 3);
SBIT(PH4, 0xD8, 4);
SBIT(PH5, 0xD8, 5);
SBIT(PH6, 0xD8, 6);
SBIT(PH7, 0xD8, 7);

/*  IICSTAT  */
SBIT(IIC_RECOV, 0xE8, 0);
SBIT(IIC_WCOL, 0xE8, 1);
SBIT(IIC_ACK, 0xE8, 2);
SBIT(IIC_BF, 0xE8, 3);
SBIT(IIC_AD, 0xE8, 4);
SBIT(IIC_RW, 0xE8, 5);
SBIT(IIC_STOP, 0xE8, 6);
SBIT(IIC_START, 0xE8, 7);

/*  SCI_S1  */
SBIT(SCI_P, 0x98, 0);
SBIT(SCI_F, 0x98, 1);
SBIT(SCI_N, 0x98, 2);
SBIT(SCI_RO, 0x98, 3);
SBIT(SCI_I, 0x98, 4);
SBIT(SCI_RI, 0x98, 5);
SBIT(SCI_TF, 0x98, 6);
SBIT(SCI_TE, 0x98, 7);

/* Predefined SFR Bit Masks */

#define IDLE              0x01    /* PCON                                */
#define STOP              0x02    /* PCON                                */
#define ECCF              0x01    /* PCA0CPMn                            */
#define PWM               0x02    /* PCA0CPMn                            */
#define TOG               0x04    /* PCA0CPMn                            */
#define MAT               0x08    /* PCA0CPMn                            */
#define CAPN              0x10    /* PCA0CPMn                            */
#define CAPP              0x20    /* PCA0CPMn                            */
#define ECOM              0x40    /* PCA0CPMn                            */
#define PWM16             0x80    /* PCA0CPMn                            */
#define PORSF             0x02    /* RSTSRC                              */
#define SWRSF             0x10    /* RSTSRC                              */


/* SFR PAGE DEFINITIONS */

#define CONFIG_PAGE       0x0F     /* SYSTEM AND PORT CONFIGURATION PAGE */
#define LEGACY_PAGE       0x00     /* LEGACY SFR PAGE                    */
#define TIMER01_PAGE      0x00     /* TIMER 0 AND TIMER 1                */
#define CPT0_PAGE         0x01     /* COMPARATOR 0                       */
#define CPT1_PAGE         0x02     /* COMPARATOR 1                       */
#define UART0_PAGE        0x00     /* UART 0                             */
#define UART1_PAGE        0x01     /* UART 1                             */
#define SPI0_PAGE         0x00     /* SPI 0                              */
#define EMI0_PAGE         0x00     /* EXTERNAL MEMORY INTERFACE          */
#define ADC0_PAGE         0x00     /* ADC 0                              */
#define ADC2_PAGE         0x02     /* ADC 2                              */
#define SMB0_PAGE         0x00     /* SMBUS 0                            */
#define TMR2_PAGE         0x00     /* TIMER 2                            */
#define TMR3_PAGE         0x01     /* TIMER 3                            */
#define TMR4_PAGE         0x02     /* TIMER 4                            */
#define DAC0_PAGE         0x00     /* DAC 0                              */
#define DAC1_PAGE         0x01     /* DAC 1                              */
#define PCA0_PAGE         0x00     /* PCA 0                              */
#define PLL0_PAGE         0x0F     /* PLL 0                              */

#endif
