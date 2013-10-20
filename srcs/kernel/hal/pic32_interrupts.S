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



.section	.sbss,bss
.globl	interruptstack
	.align	2
	.type	interruptstack, @object
	.size	interruptstack, 4
interruptstack:
	.space	4
