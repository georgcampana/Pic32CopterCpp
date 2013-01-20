/* 
 * File:   uartmanager.h
 * Author: georg
 *
 * Created on 10 gennaio 2013, 19.11
 */

#ifndef UARTMANAGER_H
#define	UARTMANAGER_H

#include "system.h"


#endif	/* UARTMANAGER_H */

#define UART_TX_BUFFER_LEN  256
#define UART_RX_BUFFER_LEN  64


class UartManager {

    UINT8 txbuffer[UART_TX_BUFFER_LEN];
    UINT8 rxbuffer[UART_RX_BUFFER_LEN];

    UINT8* txptr;
    UINT8* rxptr;

    UINT16 txlen;
    UINT16 rxlen;

    UART_MODULE module;


public:
    UartManager(UART_MODULE mod, UINT16 baud = 115000);

    void clearRxBuffer();
    void clearTxBuffer();

    UINT16 write(char* string2write);
    bool write(char chart2write);

    UINT16 countRxChars();
    INT16 getChar();

    UINT16 readLine(UINT8* dest, UINT16 maxlen);

protected:
    void setupInterrupt();

};
