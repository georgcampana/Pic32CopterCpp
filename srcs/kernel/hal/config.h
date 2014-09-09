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
 * File:   config.h
 * Author: Georg Campana
 *
 * Created on 18 ottobre 2013, 1.00
 */

#ifndef CONFIG_H
#define	CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

    #define CORETIMER_INT_LEVEL 0x0400 /* 0x0400 = level 1*/
    #define UART1_INT_LEVEL 0x0800     /* 0x0800 = level 2 */
    #define UART2_INT_LEVEL 0x0800     /* 0x0800 = level 2 */
    #define UART4_INT_LEVEL 0x0800     /* 0x0800 = level 2 */
    #define UART5_INT_LEVEL 0x0800     /* 0x0800 = level 2 */
    //#define SPI2_INT_LEVEL  0x1000     /* 0x1000 = level 4 */
    //#define DMA2_INT_LEVEL  0x1400     /* 0x1400 = level 5 */
    //#define DMA3_INT_LEVEL  0x1400     /* 0x1400 = level 5 */
    //#define I2C1_INT_LEVEL  0x1400     /* 0x1400 = level 5 */
    //#define INT0_INT_LEVEL  0x1000     /* 0x1000 = level 4 */
    //#define INT1_INT_LEVEL  0x1000     /* 0x1000 = level 4 */
    //#define INT2_INT_LEVEL  0x1000     /* 0x1000 = level 4 */
    //#define INT3_INT_LEVEL  0x1000     /* 0x1000 = level 4 */
    //#define INT4_INT_LEVEL  0x1000     /* 0x1000 = level 4 */
#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */

