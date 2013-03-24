/*
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

#include "include/uartmanager.h"

#include <p32xxxx.h>
#include <plib.h>

UartManager* uart1_ref;
UartManager* uart2_ref;



#ifdef	__cplusplus
extern "C" {
#endif

void __ISR(_UART_1_VECTOR, ipl5) Uart1InterruptServiceRoutine(void){
    uart1_ref->handleInterrupt();
}

void __ISR(_UART_2_VECTOR, ipl5) Uart2InterruptServiceRoutine(void){
    uart2_ref->handleInterrupt();
}

#ifdef	__cplusplus
}
#endif

UartManager::UartManager(UART_MODULE mod, UINT32 baud) {
    module = mod;
        

    if(mod == UART1 ) {
        uart1_ref = this;
    }
    else { // UART2
        uart2_ref = this;
    }

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
    rxbuffer.reset();
    INTRestoreInterrupts(int_status);
}

void UartManager::clearTxBuffer() {
    unsigned int int_status = INTDisableInterrupts();
    txbuffer.reset();
    INTRestoreInterrupts(int_status);
}

UINT16 UartManager::write(const char* string2write) {
    
    if(string2write==NULL) return 0;

    UINT16 transferred = 0;
    
    unsigned int int_status = INTDisableInterrupts();
    {
        for(const char* cntptr = string2write; *cntptr!=NULL; cntptr++) {
            if(txbuffer.putChar(*cntptr) == false) {
                break;
            }
            transferred++;
        }
    }
    INTRestoreInterrupts(int_status);    
    
    return transferred;
}

bool UartManager::write(char chart2write) {
    return txbuffer.putChar(chart2write);
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

void UartManager::handleInterrupt() {
    // we should get an interrupt when there is something in RX
    // and when the tx buffer is empty instead

    //RX interrupt?
    if( INTGetFlag((INT_SOURCE)INT_SOURCE_UART_RX(module)) )
    {
        // Clear the RX interrupt Flag
        INTClearFlag((INT_SOURCE)INT_SOURCE_UART_RX(module));
        
        while(UARTReceivedDataIsAvailable(module))
        {
            rxbuffer.putChar( UARTGetDataByte(module) );
        }
    }

    //TX interrupt
    if( INTGetFlag((INT_SOURCE)INT_SOURCE_UART_TX(module)) )
    {
       INTClearFlag((INT_SOURCE)INT_SOURCE_UART_TX(module));
       // Anything to send ?
       if(txbuffer.getDataLen() >0) {
           
          // I should have some free bytes in  the TX FIFO buffer
           while(UARTTransmitterIsReady(module)) { // buffer is not full
               mPORTDSetBits(BIT_1);
               INT16 nextchar = txbuffer.getChar();
               if(nextchar == -1) break;
               UARTSendDataByte(module,(BYTE)nextchar);
           }
       }
    }
}

