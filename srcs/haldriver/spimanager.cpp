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
 * File:   uartmanager.c
 * Author: Georg Campana
 *
 * Created on 01 January 2013
 *
 * When the class client is writing or reading data interrupts are disabled for a short moment
 * When the interrupt code is running there should be no problem in terms of concurrencies
 * unless an interrupt with a higher priority interrupts the uart int handler and writes to the serial
 *
 */

#include "spimanager.h"
#include "../kernel/kernel.h"

#include <p32xxxx.h>
#include <plib.h>

SpiManager* spi1_ref;
SpiManager* spi2_ref;



#ifdef	__cplusplus
extern "C" {
#endif

void
//__ISR(_UART_1_VECTOR, ipl5)
Spi1IntService(void){
    spi1_ref->handleInterrupt();
}

void
//__ISR(_UART_2_VECTOR, ipl5)
Spi2IntService(void){
    spi2_ref->handleInterrupt();
}

#ifdef	__cplusplus
}
#endif



SpiManager::SpiManager(SpiChannel chan, UInt32 baud = 115200) : channel(chan) {

    ::SpiChnOpen(channel, SPI_OPEN_MSTEN | SPI_OPEN_MODE8 | SPI_OPEN_TBE_HALF_EMPTY | SPI_OPEN_RBF_NOT_EMPTY);
    ::SpiChnEnable(channel, true);

}

bool SpiManager::transaction(UInt8* txdata, UInt8* rxdata, UInt32 datalen) {



}

void SpiManager::handleInterrupt() {

}