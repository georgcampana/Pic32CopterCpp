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
 * File:   uartmanager.h
 * Author: Georg Campana
 *
 * Created on 10 gennaio 2013, 19.11
 */

#ifndef UARTMANAGER_H
#define	UARTMANAGER_H

#include "../kernel/system.h"
#include "../kernel/device.h"
#include "../kernel/safecircularbuffer.h"
#include "../kernel/iinterrupthandler.h"

#define UART_TX_DEFAULT_BUFFER_LEN  256
#define UART_RX_DEFAULT_BUFFER_LEN  64


class UartManager : public SingleAccessStreamDevice, public IInterruptHandler {

    SafeCircularBufferBase& txbuffer;
    SafeCircularBufferBase& rxbuffer;

    const UART_MODULE module;

    bool localecho;

    // for blocking calls where a specific amount of data is needed
    Int32  rxmissingchars;

    Char const* endl;
public:

    enum RxMode {
        RX_M_NOWAIT = 0, // return data if available. Never wait
        RX_M_WAIT_EOL,   // wait until you get an eol or the provided buffer is full (or timeout)
        RX_M_NR_CHARS    // wait until a given number of chars is reached (or timeout)
    };

    enum TxMode {
        TX_M_NOWAIT = 0, // put in the tx buffer (if not ful) and return
        TX_M_WAIT_TRANSF // put in the tx buffer, if full wait and retry

    };

    UartManager(SafeCircularBufferBase& txbuff, SafeCircularBufferBase& rxbuff, UART_MODULE mod, UInt32 baud = 115200, Char const* seteol=NULL);

    void handleInterrupt();
    void clearRxBuffer();
    void clearTxBuffer();

    Char const* getEol();
    void   setEol(Char const* eol);
    UInt16 write(const Char* string2write);
    bool write(Char char2write);

    UInt16 countRxChars() const;
    Int16 getChar();

    UInt16 readLine(UInt8* dest, UInt16 maxlen);
    UInt16 read(UInt8*dest, UInt16 nobytes2read); // useful to read binary data

    void setLocalEcho(bool newstate) ;
    void setMode(RxMode rxm,TxMode txm);
    
protected:
    void setupInterrupt();

    RxMode rxmode;
    TxMode txmode;


};

inline void UartManager::setLocalEcho(bool newstate) { localecho = newstate;}
inline void UartManager::setMode(UartManager::RxMode rxm,UartManager::TxMode txm) {
    rxmode = rxm;
    txmode = txm;
}
inline Char const* UartManager::getEol() {return endl;}
inline void UartManager::setEol(Char const* eol) { endl = eol;}

class UartDefault : public UartManager {

    SafeCircularBuffer<UART_TX_DEFAULT_BUFFER_LEN> txdefaultbuff;
    SafeCircularBuffer<UART_RX_DEFAULT_BUFFER_LEN> rxdefaultbuff;

public:
    UartDefault(UART_MODULE mod, UInt32 baud = 115200, Char const* seteol=NULL) :
                            UartManager(txdefaultbuff, rxdefaultbuff, mod, baud, seteol ) {};

};

template<Int32 I, Int32 T>
class Uart : public UartManager {

    SafeCircularBuffer<I> txdefaultbuff;
    SafeCircularBuffer<T> rxdefaultbuff;

public:
    Uart(UART_MODULE mod, UInt32 baud = 115200, Char const* seteol=NULL) :
                            UartManager(txdefaultbuff, rxdefaultbuff, mod, baud, seteol ) {};

};

#endif	/* UARTMANAGER_H */
