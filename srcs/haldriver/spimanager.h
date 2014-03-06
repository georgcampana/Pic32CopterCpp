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
 * File:   spimanager.h
 * Author: Georg Campana
 *
 * Created on 23 gennaio 2014, 12.29
 */

#ifndef SPIMANAGER_H
#define	SPIMANAGER_H

#include "../kernel/hal/hal.h"
#include "../kernel/device.h"
#include "digitaliomanager.h"


class SpiManager : public SingleAccessDevice {

    SpiChannel channel;
    OutputPin* chipselect;

    DmaChannel dmachanneltx; // DMA enabled; -1 = no DMA usage
    DmaChannel dmachannelrx; // DMA enabled; -1 = no DMA usage

public:
    SpiManager(SpiChannel chan, UInt32 baud = 1000000, OutputPin* cs=NULL);

    void setChipSelectPin(OutputPin* cs);

    bool transaction(UInt8* txdata, UInt8* rxdata, UInt32 len);

    void handleInterrupt();
    void dmaHandleInterruptRx();
    void dmaHandleInterruptTx();
};


#endif	/* SPIMANAGER_H */

