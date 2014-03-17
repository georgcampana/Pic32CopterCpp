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

#include "uartmanager.h"
#include "../kernel/kernel.h"

#include <p32xxxx.h>
#include <plib.h>

IInterruptHandlerPrt uart1_ref;
IInterruptHandlerPrt uart2_ref;



#ifdef	__cplusplus
extern "C" {
#endif

void
//__ISR(_UART_1_VECTOR, ipl5)
Uart1IntService(void){
    uart1_ref->handleInterrupt();
}

void
//__ISR(_UART_2_VECTOR, ipl5)
Uart2IntService(void){
    uart2_ref->handleInterrupt();
}

#ifdef	__cplusplus
}
#endif



UartManager::UartManager(SafeCircularBufferBase& txbuff, SafeCircularBufferBase& rxbuff,
                            UART_MODULE mod, UINT32 baud, Char const* seteol) :
                                localecho(false), module(mod), txbuffer(txbuff), rxbuffer(rxbuff),
                                rxmode(RX_M_NOWAIT), txmode(TX_M_NOWAIT), rxmissingchars(0),
                                endl(default_eol) {

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
    UARTSetFifoMode(module, (UART_FIFO_MODE)(UART_INTERRUPT_ON_RX_NOT_EMPTY | UART_INTERRUPT_ON_TX_DONE));

    UARTSetLineControl(module, (UART_LINE_CONTROL_MODE)(UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1));

    UARTSetDataRate(module, GetPeripheralClock(), baud) ;

    UARTEnable(module, (UART_ENABLE_MODE)UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));
    
    setupInterrupt();

    if(seteol != NULL) endl = seteol;
}

void UartManager::clearRxBuffer() {
    rxbuffer.reset();
}

void UartManager::clearTxBuffer() {
    txbuffer.reset();
}

UINT16 UartManager::write(const char* string2write) {
    
    if(string2write==NULL || *string2write == 0x00) return 0;

    UINT16 transferred = 0;

    if(txmode == TX_M_NOWAIT) {
        while(*string2write != NULL) {
            if(txbuffer.putChar(*string2write) == false) {
                break;
            }
            transferred++;
            string2write++;
        }

        if(transferred) {
            INTEnable((INT_SOURCE)INT_SOURCE_UART_TX(module), INT_ENABLED);
        }
    }
    else { // txmode == TX_M_WAIT_TRANSF
        bool allsent=false;
        while(allsent==false) {
            while(true) {
                if(*string2write == NULL) { allsent = true; break; }

                if(txbuffer.putChar(*string2write) == false) {
                    break; // no more space in the buffer
                }
                transferred++;
                string2write++;

            }
            if(transferred) {
                waitingtask = Kernel::GetRunningTask();
                INTEnable((INT_SOURCE)INT_SOURCE_UART_TX(module), INT_ENABLED);
                if(TaskDefaultWait(waitingtask, blockingtimeout) == false) { break; } // timeout
            }
        }
    }

    return transferred;
}

bool UartManager::write(char char2write) {
    bool success = false;

    if(txmode == TX_M_NOWAIT) {
        success =  txbuffer.putChar(char2write);
        if(success) {
            INTEnable((INT_SOURCE)INT_SOURCE_UART_TX(module), INT_ENABLED);
        }
    } else { // txmode == TX_M_WAIT_TRANSF
        waitingtask = Kernel::GetRunningTask();
        while((success = txbuffer.putChar(char2write)) == false) { // full buffer
            INTEnable((INT_SOURCE)INT_SOURCE_UART_TX(module), INT_ENABLED);
            if(TaskDefaultWait(waitingtask, blockingtimeout) == false) { return false; } // timeout
        }
        // wait for my transfer
        if(TaskDefaultWait(waitingtask, blockingtimeout) == false) { return false; } // timeout
        waitingtask == NULL;
    }

    return success;
}

UINT16 UartManager::countRxChars() const {
    return rxbuffer.getDataLen();
}

INT16 UartManager::getChar(){

    rxmissingchars=1;
    waitingtask = Kernel::GetRunningTask(); // TODO: can we optimize this ?
    INT16 readchar = rxbuffer.getChar();

    if(rxmode != RX_M_NOWAIT) { // eol and nrchars modes (actually there is no real eol mode for getChar())
        while(readchar == -1) {  // buffer was empty
            if(this->TaskDefaultWait(waitingtask, blockingtimeout) == false) {
                // timeout
                break;
            }
            readchar = rxbuffer.getChar();
        }

    }

    return readchar;
}

UInt16 UartManager::readLine(UINT8* dest, UINT16 maxlen) {
    if(maxlen==0)return 0;

    maxlen--; // reserved for the final null to close the string

    UInt16 nrreadchars = 0;

    switch(rxmode) {
        case RX_M_NOWAIT:
            while(maxlen--) {
                INT16 readchar = rxbuffer.getChar();
                if(readchar == -1) break;
                if(readchar == endl[0] && endl[1]== NULL) break; // eol found (case of \r or \n)
                if(readchar == endl[1] && endl[1]!= NULL) break; // eol found (case of \r\n)
                if(endl[1] != NULL && readchar == endl[0]) { maxlen++; continue; }
                *dest++ = (BYTE) readchar;
                nrreadchars++;
            }
            break;
        case RX_M_WAIT_EOL:
            {
                bool eolfound=false;
                waitingtask = Kernel::GetRunningTask();
                while(eolfound==false) {
                    while(maxlen) {
                        INT16 readchar = rxbuffer.getChar();                        
                        if(readchar == -1) break; // buffer is empty

                        if(readchar == endl[0] && endl[1]== NULL) {
                            eolfound=true;
                            break; // eol found (case of \r or \n)
                        }
                        if(readchar == endl[1] && endl[1]!= NULL) {
                            eolfound=true;
                            break; // eol found (case of \r\n)
                        }
                        if(endl[1] != NULL && readchar == endl[0]) {continue; } // we skip the first char in case of \r\n

                        maxlen--;
                        *dest++ = (BYTE) readchar;
                        nrreadchars++;
                    }
                    if(eolfound || maxlen==0) break;
                    if( this->TaskDefaultWait(waitingtask, blockingtimeout) == false){
                        // timeout
                        // let's exit whatever we have transferred til now.
                        break;
                    }
                }
                waitingtask = NULL;
            }
            break;
        case RX_M_NR_CHARS:
            // TODO: do we need an ssert here ?
            break;
    }



    *dest++ = NULL;

    return nrreadchars;
}

UInt16 UartManager::read(UInt8*dest, UInt16 nobytes2read) {
    if(nobytes2read==0)return 0;

    UInt16 nrreadchars = 0;

    switch(rxmode) {
        case RX_M_NOWAIT:
            while(nobytes2read--) {
                INT16 readchar = rxbuffer.getChar();
                if(readchar == -1) break;
                *dest++ = (BYTE) readchar;
                nrreadchars++;
            }
            break;
        case RX_M_WAIT_EOL:
            {
                bool eolfound=false;
                waitingtask = Kernel::GetRunningTask();
                while(eolfound==false) {
                    while(nobytes2read) {
                        INT16 readchar = rxbuffer.getChar();
                        if(readchar == -1) break; // buffer is empty
                        nobytes2read--;
                        *dest++ = (BYTE) readchar;
                        nrreadchars++;
                        if(readchar == endl[0] && endl[1]== NULL) { eolfound=true; break;} // eol found (case of \r or \n)
                        if(readchar == endl[1] && endl[1]!= NULL) { eolfound=true; break;} // eol found (case of \r\n)
                    }
                    if(eolfound || nobytes2read==0) break;
                    if( this->TaskDefaultWait(waitingtask, blockingtimeout) == false){
                        // timeout
                        // let's exit whatever we have transferred til now.
                        break;
                    }
                }
                waitingtask = NULL;
            }
            break;
        case RX_M_NR_CHARS:
            {
                waitingtask = Kernel::GetRunningTask();

                while(nobytes2read) {
                    INT16 readchar = rxbuffer.getChar();
                    if(readchar == -1) { // buffer is empty
                        rxmissingchars = nobytes2read;
                        if( this->TaskDefaultWait(waitingtask, blockingtimeout) == false){
                            // timeout
                            // let's exit whatever we have transferred til now.
                            break;
                        }
                        continue;
                    }
                    nobytes2read--;
                    *dest++ = (BYTE) readchar;
                    nrreadchars++;
                }

                waitingtask = NULL;
            }
            break;
    }

    *dest++ = NULL;

    return nrreadchars;
}



void UartManager::setupInterrupt() {

    // Set Interrupt priorities
    INTSetVectorPriority((INT_VECTOR)INT_VECTOR_UART(module), INT_PRIORITY_LEVEL_2);
    INTSetVectorSubPriority((INT_VECTOR)INT_VECTOR_UART(module), INT_SUB_PRIORITY_LEVEL_3);

    INTEnable((INT_SOURCE)INT_SOURCE_UART_RX(module), INT_ENABLED);
}

void UartManager::handleInterrupt() {
    // we should get an interrupt when there is something in RX
    // and when the tx buffer is empty instead

    //RX interrupt?
    if( INTGetFlag((INT_SOURCE)INT_SOURCE_UART_RX(module)) )
    {
        // Clear the RX interrupt Flag
        INTClearFlag((INT_SOURCE)INT_SOURCE_UART_RX(module));
        if(rxmode == RX_M_NOWAIT) {
            while(UARTReceivedDataIsAvailable(module))
            {
                BYTE rxchar = UARTGetDataByte(module);
                rxbuffer.putChar(rxchar);
                if(localecho) {
                    write(rxchar);
                }
            }
        }
        else if(rxmode == RX_M_WAIT_EOL) {
            while(UARTReceivedDataIsAvailable(module))
            {
                BYTE rxchar = UARTGetDataByte(module);
                if(rxbuffer.putChar(rxchar) == false) {
                    // buffer is full
                    if(waitingtask) {
                        SignalWaitingTask(waitingtask);
                    }
                    break; // we must exit from here
                }

                if(localecho) {
                    write(rxchar);
                }

                if(rxchar == endl[0] && endl[1] == NULL) { // case of \r  or \n
                    if(waitingtask) { SignalWaitingTask(waitingtask); }
                }
                else if(endl[1] != NULL && rxchar == endl[1]) { // case of \r\n
                    if(waitingtask) { SignalWaitingTask(waitingtask); }
                }
            }
        }
        else { // RX_M_NR_CHARS
            while(UARTReceivedDataIsAvailable(module))
            {
                BYTE rxchar = UARTGetDataByte(module);
                if(rxbuffer.putChar(rxchar) == false) {
                    // buffer is full
                    if(waitingtask) {
                        SignalWaitingTask(waitingtask);
                    }
                    break; // we must exit from here
                }
                else {
                    rxmissingchars--;
                    if(rxmissingchars <=0 ) {
                        if(waitingtask) {
                            SignalWaitingTask(waitingtask); // we signal that enough chars are available
                        }
                    }
                    if(localecho) {
                        write(rxchar);
                    }
                }

            }
        }
    }
    //TX interrupt
    if( INTGetFlag((INT_SOURCE)INT_SOURCE_UART_TX(module)) )
    {
       Int32 sent = 0;
       // Anything to send ?
       if(txbuffer.getDataLen() >0) { 
          // I should have some free bytes in  the TX FIFO buffer
           while(UARTTransmitterIsReady(module)) { // buffer is not full
               INT16 nextchar = txbuffer.getChar();
               if(nextchar == -1) {
                   // empty circular buffer; we switch interrupts off, until we get more data
                   INTEnable((INT_SOURCE)INT_SOURCE_UART_TX(module), INT_DISABLED);
                   break;
               }
               UARTSendDataByte(module,(BYTE)nextchar);
               sent++;
           }
       }
       else {
           // nothing to send switch off INTs (should never happen)
           INTEnable((INT_SOURCE)INT_SOURCE_UART_TX(module), INT_DISABLED);
       }

       if(sent >0 ) {
           // if we didn't send anything then we do not clear the int
           // but simply disable it (see above)
           INTClearFlag((INT_SOURCE)INT_SOURCE_UART_TX(module));
       }
    }
}

