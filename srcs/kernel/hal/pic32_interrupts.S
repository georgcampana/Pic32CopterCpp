/*
 * Pic32CopterCpp -- A C++ microOS for the PIC32
 *
 * Copyright (C) 2012 - 2014, Georg Campana
 *
 * Georg Campana <g.campana(AT)stetel.com>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE.txt file
 * at the top of the source tree.
 *
 */

#include "pic32_intmacro.inc"
#include "config.h"

.text

/* PIC32 CoreTimer interrupt (vector0)
* This is the prologue + epilogue for the pre-empting interrupt routine which is
* present in the Hal module
* We have a pointer to the stack reserved for intrrupts
* (see "interruptstack" in the sbss section).
*/
#ifdef CORETIMER_INT_LEVEL
.section .vector_0,code
   j      handleCoreTimer
   nop

.section .text,code
.ent handleCoreTimer
handleCoreTimer:
    .set noreorder
    .set nomips16
    .set noat
    IntHandler intlevel=CORETIMER_INT_LEVEL intservice=handleSysTimerINT
.end handleCoreTimer
#endif

/* PIC32 Uart1 interrupt (vector24)
* This is the prologue + epilogue for the pre-empting interrupt routine which is
* present in the Hal module
* We have a pointer to the stack reserved for interrupts
* (see "interruptstack" in the sbss section).
*/
#ifdef UART1_INT_LEVEL
.section .vector_24,code
   j      uart1IntHandler
   nop

.section .text,code
.ent uart1IntHandler
uart1IntHandler:
    .set noreorder
    .set nomips16
    .set noat
    IntHandler intlevel=UART1_INT_LEVEL intservice=Uart1IntService
.end uart1IntHandler
#endif

/* PIC32 Uart2 interrupt (vector32)
* This is the prologue + epilogue for the pre-empting interrupt routine which is
* present in the Hal module
* We have a pointer to the stack reserved for interrupts
* (see "interruptstack" in the sbss section).
*/
#ifdef UART2_INT_LEVEL
.section .vector_32,code
   j      uart2IntHandler
   nop

.section .text,code
.ent uart2IntHandler
uart2IntHandler:
    .set noreorder
    .set nomips16
    .set noat
    IntHandler intlevel=UART2_INT_LEVEL intservice=Uart2IntService
.end uart2IntHandler
#endif

/* PIC32 Uart4 interrupt (vector49)
* This is the prologue + epilogue for the pre-empting interrupt routine which is
* present in the Hal module
* We have a pointer to the stack reserved for interrupts
* (see "interruptstack" in the sbss section).
*/
#ifdef UART4_INT_LEVEL
.section .vector_49,code
   j      uart4IntHandler
   nop

.section .text,code
.ent uart4IntHandler
uart4IntHandler:
    .set noreorder
    .set nomips16
    .set noat
    IntHandler intlevel=UART4_INT_LEVEL intservice=Uart4IntService
.end uart4IntHandler
#endif

/* PIC32 Uart5 interrupt (vector51)
* This is the prologue + epilogue for the pre-empting interrupt routine which is
* present in the Hal module
* We have a pointer to the stack reserved for interrupts
* (see "interruptstack" in the sbss section).
*/
#ifdef UART5_INT_LEVEL
.section .vector_51,code
   j      uart5IntHandler
   nop

.section .text,code
.ent uart5IntHandler
uart5IntHandler:
    .set noreorder
    .set nomips16
    .set noat
    IntHandler intlevel=UART5_INT_LEVEL intservice=Uart5IntService
.end uart5IntHandler
#endif

/* PIC32 SPI2 interrupt (vector31)
* This is the prologue + epilogue for the pre-empting interrupt routine which is
* present in the Hal module
* We have a pointer to the stack reserved for interrupts
* (see "interruptstack" in the sbss section).
*/
#ifdef SPI2_INT_LEVEL
.section .vector_31,code
   j      spi2IntHandler
   nop

.section .text,code
.ent spi2IntHandler
spi2IntHandler:
    .set noreorder
    .set nomips16
    .set noat
    IntHandler intlevel=SPI2_INT_LEVEL intservice=Spi2IntService
.end spi2IntHandler
#endif


/* PIC32 DMA2 interrupt (vector38)
* This is the prologue + epilogue for the pre-empting interrupt routine which is
* present in the Hal module
* We have a pointer to the stack reserved for interrupts
* (see "interruptstack" in the sbss section).
*/
#ifdef DMA2_INT_LEVEL
.section .vector_38,code
   j      dma2IntHandler
   nop

.section .text,code
.ent dma2IntHandler
dma2IntHandler:
    .set noreorder
    .set nomips16
    .set noat
    IntHandler intlevel=DMA2_INT_LEVEL intservice=Dma3IntService
.end dma2IntHandler
#endif

/* PIC32 DMA3 interrupt (vector39)
* This is the prologue + epilogue for the pre-empting interrupt routine which is
* present in the Hal module
* We have a pointer to the stack reserved for interrupts
* (see "interruptstack" in the sbss section).
*/
#ifdef DMA3_INT_LEVEL
.section .vector_39,code
   j      dma3IntHandler
   nop

.section .text,code
.ent dma3IntHandler
dma3IntHandler:
    .set noreorder
    .set nomips16
    .set noat
    IntHandler intlevel=DMA3_INT_LEVEL intservice=Dma4IntService
.end dma3IntHandler
#endif

/* PIC32 I2C1 interrupt (vector25)
* This is the prologue + epilogue for the pre-empting interrupt routine which is
* present in the Hal module
* We have a pointer to the stack reserved for interrupts
* (see "interruptstack" in the sbss section).
*/
#ifdef I2C1_INT_LEVEL
.section .vector_25,code
   j      i2c1IntHandler
   nop

.section .text,code
.ent i2c1IntHandler
i2c1IntHandler:
    .set noreorder
    .set nomips16
    .set noat
    IntHandler intlevel=I2C1_INT_LEVEL intservice=I2c1IntService
.end i2c1IntHandler
#endif

///* PIC32 I2C2 interrupt (vector33)
//* This is the prologue + epilogue for the pre-empting interrupt routine which is
//* present in the Hal module
//* We have a pointer to the stack reserved for interrupts
//* (see "interruptstack" in the sbss section).
//*/
//#ifdef I2C2_INT_LEVEL
//.section .vector_33,code
//   j      i2c2IntHandler
//   nop
//
//.section .text,code
//.ent i2c2IntHandler
//i2c2IntHandler:
//    .set noreorder
//    .set nomips16
//    .set noat
//    IntHandler intlevel=I2C2_INT_LEVEL intservice=I2c2InterruptServiceRoutine
//.end i2c2IntHandler
//#endif

/* PIC32 INT1 interrupt (vector7)
* This is the prologue + epilogue for the pre-empting interrupt routine which is
* present in the Hal module
* We have a pointer to the stack reserved for interrupts
* (see "interruptstack" in the sbss section).
*/
#ifdef INT1_INT_LEVEL
.section .vector_7,code
   j      int1IntHandler
   nop

.section .text,code
.ent int1IntHandler
int1IntHandler:
    .set noreorder
    .set nomips16
    .set noat
    IntHandler intlevel=INT1_INT_LEVEL intservice=InterruptPin1Service
.end int1IntHandler
#endif

/* PIC32 INT4 interrupt (vector19)
* This is the prologue + epilogue for the pre-empting interrupt routine which is
* present in the Hal module
* We have a pointer to the stack reserved for interrupts
* (see "interruptstack" in the sbss section).
*/
#ifdef INT4_INT_LEVEL
.section .vector_19,code
   j      int4IntHandler
   nop

.section .text,code
.ent int4IntHandler
int4IntHandler:
    .set noreorder
    .set nomips16
    .set noat
    IntHandler intlevel=INT4_INT_LEVEL intservice=InterruptPin4Service
.end int4IntHandler
#endif

/* PIC32 Change Notification Pin interrupt (vector26)
* This is the prologue + epilogue for the pre-empting interrupt routine which is
* present in the Hal module
* We have a pointer to the stack reserved for interrupts
* (see "interruptstack" in the sbss section).
*/
#ifdef CN_INT_LEVEL
.section .vector_26,code
   j      cnIntHandler
   nop

.section .text,code
.ent cnIntHandler
cnIntHandler:
    .set noreorder
    .set nomips16
    .set noat
    IntHandler intlevel=CN_INT_LEVEL intservice=DigitalIO1InterruptServiceRoutine
.end cnIntHandler
#endif

/* PIC32 UsbOTG interrupt (vector45)
* This is the prologue + epilogue for the pre-empting interrupt routine which is
* present in the Hal module
* We have a pointer to the stack reserved for interrupts
* (see "interruptstack" in the sbss section).
*/
#ifdef USB_INT_LEVEL
.section .vector_45,code
   j      usbIntHandler
   nop

.section .text,code
.ent usbIntHandler
usbIntHandler:
    .set noreorder
    .set nomips16
    .set noat
    IntHandler intlevel=USB_INT_LEVEL intservice=UsbIntService
.end usbIntHandler
#endif
    

.section	.sbss,bss
.globl	interruptstack
	.align	2
	.type	interruptstack, @object
	.size	interruptstack, 4
interruptstack:
	.space	4
