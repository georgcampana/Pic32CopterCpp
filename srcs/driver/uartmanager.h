/* 
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


#define UART_TX_BUFFER_LEN  256
#define UART_RX_BUFFER_LEN  64


class UartManager : public CharStreamDevice {

    SafeCircularBuffer<UART_TX_BUFFER_LEN> txbuffer;
    SafeCircularBuffer<UART_RX_BUFFER_LEN> rxbuffer;

    const UART_MODULE module;

    bool localecho;
public:
    UartManager(UART_MODULE mod, UInt32 baud = 115200);

    void handleInterrupt();
    void clearRxBuffer();
    void clearTxBuffer();

    UInt16 write(const Char* string2write);
    bool write(Char char2write);

    UInt16 countRxChars() const;
    Int16 getChar();

    UInt16 readLine(UInt8* dest, UInt16 maxlen);

    void setLocalEcho(bool newstate);
protected:
    void setupInterrupt();

};

#endif	/* UARTMANAGER_H */
