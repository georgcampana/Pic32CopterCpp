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
//__ISR(_SPI_2_VECTOR, ipl5)
Spi1IntService(void){
    spi1_ref->handleInterrupt();
}

void
Spi2IntService(void){
    spi2_ref->handleInterrupt();
}

void // DMA_CHANNEL2
Dma3IntService(void) {
    spi2_ref->dmaHandleInterruptTx();
}

void // DMA_CHANNEL3
Dma4IntService(void) {
    spi2_ref->dmaHandleInterruptRx();
}

#ifdef	__cplusplus
}
#endif



SpiManager::SpiManager(SpiChannel chan, UInt32 baud, OutputPin* cs) : channel(chan),
                         dmachanneltx(DMA_CHANNEL2), dmachannelrx(DMA_CHANNEL3), chipselect(cs) {

    if(chipselect)chipselect->set(true); // deactivate chip interface;

    ::SpiChnEnable(channel, 0);
    ::SpiChnOpen(channel, (SpiOpenFlags) (SPI_OPEN_MSTEN | SPI_OPEN_MODE8) /* | SPI_OPEN_TBE_HALF_EMPTY | SPI_OPEN_RBF_NOT_EMPTY */,4);
    ::SpiChnSetBitRate(channel, GetPeripheralClock(), baud);
    ::SpiChnEnable(channel, true);

#ifdef _SPI_1_VECTOR  
    spi1_ref = this;
#endif
#ifdef _SPI_2_VECTOR  
    spi2_ref = this;

    //DMA SPI TX (pri same as the RX to be sure that it will perform roundrobin)
    DmaChnOpen(dmachanneltx, DMA_CHN_PRI3, DMA_OPEN_DEFAULT);
    DmaChnSetEventControl(dmachanneltx, (DmaEvCtrlFlags)( DMA_EV_START_IRQ_EN | DMA_EV_START_IRQ(_SPI2_TX_IRQ)));

    //DmaChnSetEvEnableFlags(dmachanneltx, DMA_EV_BLOCK_DONE);

    INTSetVectorPriority((INT_VECTOR)INT_VECTOR_DMA(dmachanneltx), INT_PRIORITY_LEVEL_5);
    INTSetVectorSubPriority((INT_VECTOR)INT_VECTOR_DMA(dmachanneltx), INT_SUB_PRIORITY_LEVEL_3);

    //INTEnable((INT_SOURCE)INT_SOURCE_DMA(dmachanneltx), INT_ENABLED);

    //DMA SPI RX
    DmaChnOpen(dmachannelrx, DMA_CHN_PRI3,DMA_OPEN_DEFAULT);
    DmaChnSetEventControl(dmachannelrx, (DmaEvCtrlFlags)(DMA_EV_START_IRQ_EN| DMA_EV_START_IRQ(_SPI2_RX_IRQ)));

    DmaChnSetEvEnableFlags(dmachannelrx, DMA_EV_BLOCK_DONE);
    
    INTSetVectorPriority((INT_VECTOR)INT_VECTOR_DMA(dmachannelrx), INT_PRIORITY_LEVEL_5);
    INTSetVectorSubPriority((INT_VECTOR)INT_VECTOR_DMA(dmachannelrx), INT_SUB_PRIORITY_LEVEL_3);

    INTEnable((INT_SOURCE)INT_SOURCE_DMA(dmachannelrx), INT_ENABLED);

#endif


}


bool SpiManager::transaction(UInt8* txdata, UInt8* rxdata, UInt32 len) {

    UInt32 dummy = *((UInt32*)SpiChnBuffer(channel)); // this is to free the read buffer
    ::SpiChnGetRov(channel, true); // this is to reset any overflow condition

    waitingtask = Kernel::GetRunningTask();

    if(chipselect) chipselect->set(false) ;

    ::SpiChnGetRov(channel, true);

    ::DmaChnSetTxfer(dmachannelrx, (void*)SpiChnBuffer(channel), rxdata, 1, len, 1 );
    ::DmaChnEnable(dmachannelrx);

    ::DmaChnSetTxfer(dmachanneltx, txdata, (void*)SpiChnBuffer(channel), len, 1, 1 );
    ::DmaChnStartTxfer(dmachanneltx, DMA_WAIT_NOT, 0 );


    TaskDefaultWait(waitingtask, blockingtimeout);
    waitingtask = NULL; // let's avoid spurious signals

    if(chipselect) chipselect->set(true) ;
}

void SpiManager::handleInterrupt() {

}

void SpiManager::setChipSelectPin(OutputPin* cs) {
    chipselect = cs;
    if(chipselect)chipselect->set(true); // deactivate chip interface;
}

void SpiManager::dmaHandleInterruptRx() {

    //event flags causing the interruptt
    int eventflags;

    //get the event flags generating the interrupt
    eventflags=DmaChnGetEvFlags(dmachannelrx);
    //clear for further ints
    INTClearFlag((INT_SOURCE)INT_SOURCE_DMA(dmachannelrx));

    if(eventflags & DMA_EV_BLOCK_DONE)
    {
        DmaChnClrEvFlags(dmachannelrx, DMA_EV_BLOCK_DONE);
        SignalWaitingTask(waitingtask);
    }
    else {
        // should never happen because only block complete is enabled (unless an error happened)
        DmaChnClrEvFlags(dmachannelrx, (DmaEvFlags)eventflags);
    }
}

void SpiManager::dmaHandleInterruptTx() {
    //event flags causing the interruptt
    int eventflags;

    //get the event flags generating the interrupt
    eventflags=DmaChnGetEvFlags(dmachanneltx);
    //clear for further ints
    INTClearFlag((INT_SOURCE)INT_SOURCE_DMA(dmachanneltx));

    if(eventflags & DMA_EV_BLOCK_DONE)
    {
        DmaChnClrEvFlags(dmachanneltx, DMA_EV_BLOCK_DONE);
    }
    else {
        // should never happen because only block complete is enabled (unless an error happened)
        DmaChnClrEvFlags(dmachanneltx, (DmaEvFlags)eventflags);
    }
}