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
 * File:   uartmanagersync.h
 * Author: Georg Campana
 *
 * Created on 21 gennaio 2014, 12.23
 */

#ifndef UARTMANAGERSYNC_H
#define	UARTMANAGERSYNC_H


#include "../kernel/system.h"
#include "../kernel/device.h"
#include "uartinthandlers.h"

class UartManagerSync : public SingleAccessStreamDevice, public IInterruptHandler {

    const UART_MODULE module;
    Char* rxbuffer;
    Int32 rxbufferlen;

    Char* txcurrentptr;
    Char* rxcurrentptr;


    bool localecho;
public:
    UartManagerSync(UART_MODULE mod, UInt32 baud = 115200, Char* rx1buffer=NULL, Char* rx2buffer=NULL, Int32 rxbuflen=-1);

    void handleInterrupt(); // IInterruptHandler


    UInt16 write(const Char* string2write);
    bool write(Char char2write);

    UInt16 countRxChars() const;
    Int16 getChar();
    void clearRxBuffer();

    UInt16 readLine(UInt8* dest, UInt16 maxlen);

    void setLocalEcho(bool newstate);
    
    void setBlockingTimeout(Int32 mstimeout);
    void setRxBuffers(Char* rx1buffer,Char* rx2buffer, UInt32 len);

protected:
    void setupInterrupt();


};



#endif	/* UARTMANAGERSYNC_H */

