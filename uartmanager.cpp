/*
 * File:   uartmanager.c
 * Author: Georg Campana
 *
 * Created on 01 January 2013
 */

#include "include/uartmanager.h"

#include <p32xxxx.h>
#include <plib.h>

UartManager* uart1_ref;
UartManager* uart2_ref;



#ifdef	__cplusplus
extern "C" {
#endif

void __ISR(_UART_1_VECTOR, ipl5) Uart1InterruptServiceRoutine(void){



}


#ifdef	__cplusplus
}
#endif

UartManager::UartManager(UART_MODULE mod, UINT16 baud) {
    module = mod;
        
    txptr = txbuffer;
    rxptr = rxbuffer;
    txlen = rxlen = 0;

    UARTConfigure(module, UART_ENABLE_PINS_TX_RX_ONLY);
    // have to force/cast to the enum because it generates an error otherwise
    // when compiled under c++ --> should be communicated to microchip
    // to ammend the xc32 compiler

    UARTSetFifoMode(module, (UART_FIFO_MODE)(UART_INTERRUPT_ON_RX_NOT_EMPTY | UART_INTERRUPT_ON_TX_BUFFER_EMPTY));
    UARTSetLineControl(module, (UART_LINE_CONTROL_MODE)(UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1));
    UARTSetDataRate(module, GetPeripheralClock(), baud) ;
   
    UARTEnable(module, (UART_ENABLE_MODE)UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));
    
    setupInterrupt();
}

void UartManager::clearRxBuffer() {
    unsigned int int_status = INTDisableInterrupts();
    rxlen = 0;
    INTRestoreInterrupts(int_status);
}

void UartManager::clearTxBuffer() {
    unsigned int int_status = INTDisableInterrupts();
    txlen = 0;
    INTRestoreInterrupts(int_status);
}

UINT16 UartManager::write(char* string2write) {
    
    if(string2write==NULL) return 0;
    
    UINT32 strlen = 0;
    for(char* cntptr = string2write; *cntptr==NULL; cntptr++) strlen++;
    
    if(strlen==0) return 0;
    
    if(strlen > UART_TX_BUFFER_LEN) {
        strlen = UART_TX_BUFFER_LEN;
    }
    UINT16 transferred = strlen;
    
    unsigned int int_status = INTDisableInterrupts();
    {
        UINT16 bytesbeforeend = &txbuffer[UART_TX_BUFFER_LEN] - txptr;
        // let's fill till we hit the end of the buffer and then rotate
        if(strlen > bytesbeforeend) {
            while(bytesbeforeend--) {
               *txptr++ = *string2write++;
            }
            strlen -= bytesbeforeend;

            if(txptr == &txbuffer[UART_TX_BUFFER_LEN]) {
                txptr = txbuffer ;
            }
        }
        
        while(strlen--) {
            *txptr++ = *string2write++;
        }
        
        txlen += transferred;
        if(txlen > UART_TX_BUFFER_LEN)txlen = UART_TX_BUFFER_LEN;
    }
    INTRestoreInterrupts(int_status);    
    
    return transferred;
}

bool UartManager::write(char chart2write) {

}

UINT16 UartManager::countRxChars() {

}

INT16 UartManager::getChar(){

}

UINT16 UartManager::readLine(UINT8* dest, UINT16 maxlen) {

}

void UartManager::setupInterrupt() {

    // Set Interrupt priorities
    INTSetVectorPriority((INT_VECTOR)INT_VECTOR_UART(module), INT_PRIORITY_LEVEL_5);
    INTSetVectorSubPriority((INT_VECTOR)INT_VECTOR_UART(module), INT_SUB_PRIORITY_LEVEL_3);

    INTEnable((INT_SOURCE)INT_SOURCE_UART_RX(module), INT_ENABLED);
    INTEnable((INT_SOURCE)INT_SOURCE_UART_TX(module), INT_ENABLED);

}