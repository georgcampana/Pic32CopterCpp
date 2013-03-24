/* 
 * File:   uartmanager.h
 * Author: Georg Campana
 *
 * Created on 10 gennaio 2013, 19.11
 */

#ifndef UARTMANAGER_H
#define	UARTMANAGER_H

#include "system.h"


#endif	/* UARTMANAGER_H */


template<int I>
class CircularBuffer {
    UINT8 buffer[I];

    UINT8 *wrptr;
    UINT8 *rdptr;
    UINT16 datalen;
    
public:
    CircularBuffer();
    
    bool putChar(char chartoput);
    INT16 getChar();
    void reset();
    UINT16 getDataLen();
};

template<int I>
inline CircularBuffer<I>::CircularBuffer() {
    reset();
}

template<int I>
inline bool CircularBuffer<I>::putChar(char char2put) {
    if(datalen == I) { // buffer is full
        return false;
    }

    *wrptr++ == char2put;
    datalen++;
    if(wrptr == &buffer[I]) wrptr = buffer;
    return true;
}

template<int I>
inline INT16 CircularBuffer<I>::getChar() {

    if(datalen > 0) {
        UINT8 char2return = *rdptr++;
        if(rdptr == &buffer[I]) rdptr = buffer;
        return char2return;
    }

    return -1;
}

template<int I>
inline void CircularBuffer<I>::reset() {
    wrptr = rdptr = buffer;
    datalen = 0;
}

template<int I>
inline UINT16 CircularBuffer<I>::getDataLen() {
    return datalen;
}

#define UART_TX_BUFFER_LEN  256
#define UART_RX_BUFFER_LEN  64


class UartManager {

    CircularBuffer<UART_TX_BUFFER_LEN> txbuffer;
    CircularBuffer<UART_RX_BUFFER_LEN> rxbuffer;

    UART_MODULE module;

public:
    UartManager(UART_MODULE mod, UINT32 baud = 115000);

    void handleInterrupt();
    void clearRxBuffer();
    void clearTxBuffer();

    UINT16 write(const char* string2write);
    bool write(char chart2write);

    UINT16 countRxChars();
    INT16 getChar();

    UINT16 readLine(UINT8* dest, UINT16 maxlen);

protected:
    void setupInterrupt();

};
