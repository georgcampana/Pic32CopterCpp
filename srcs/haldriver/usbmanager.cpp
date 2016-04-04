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
 * File:   usbmanager.cpp
 * Author: Georg Campana
 *
 * Created on 25 aprile 2015, 20.30
 */


#include "../kernel/kernel.h"

#include <p32xxxx.h>
#include <plib.h>

#include "usbmanager.h"

UsbManager* usb_ref;


#ifdef	__cplusplus
extern "C" {
#endif

void
//__ISR(_USB_VECTOR, ipl5)
UsbIntService(void){
    usb_ref->handleInterrupt();
}

#ifdef	__cplusplus
}
#endif

UsbEpHandler::UsbEpHandler() :  rxisodd(false),txisodd(false){
  

    
};

void UsbEpHandler::init(UsbEndpointBD* descr, UInt8 epnr) {
    descriptor = descr;
    epnumber = epnr;
    
    switch(epnr) {
        case 0: ctrlreg = &U1EP0; break; 
        case 1: ctrlreg = &U1EP1; break; 
        case 2: ctrlreg = &U1EP2; break;
        case 3: ctrlreg = &U1EP3; break;
        case 4: ctrlreg = &U1EP4; break; 
        case 5: ctrlreg = &U1EP5; break; 
        case 6: ctrlreg = &U1EP6; break;
        case 7: ctrlreg = &U1EP7; break;
        case 8: ctrlreg = &U1EP8; break; 
        case 9: ctrlreg = &U1EP9; break; 
        case 10: ctrlreg = &U1EP10; break;
        case 11: ctrlreg = &U1EP11; break;
        case 12: ctrlreg = &U1EP12; break; 
        case 13: ctrlreg = &U1EP13; break; 
        case 14: ctrlreg = &U1EP14; break;
        case 15: ctrlreg = &U1EP15; break;
    }
}

void UsbEpHandler::transfer(bool tx, bool odd) {
    UsbBufferDescriptor* transdescriptor = &descrvec->vector[tx][odd];
    if(transdescriptor->uown == 0) { // we have access
        switch(transdescriptor->pid) {
            case PID_SETUP:
                break;
            case PID_IN:
                break;
            case PID_OUT:
                break;                
            default:
                // handleTransferPanic();
                break; 
        }
    }
    else { // hmmmm got the interrupt but no access to the descriptor
        // handleTransferPanic();
    }
}


UsbManager::UsbManager() {
    ep[0].init(&bdt.endpoint[0],0);
    usb_ref = this;
}

void UsbManager::handleInterrupt() {

    
    
    // Clear Interrupt
    //if(IFS1bits.USBIF) {
        IFS1CLR = IFS1_USB_BIT; // USBIF
    //}
        
        UInt32 clearflags = 0;
    // let's check where the Int arrives from
        
    // Packet Transfer complete ? (token))
    if(U1IRbits.TRNIF) {
        
        transferCompleted();
        
        clearflags |= _U1IR_TRNIF_MASK;
    }
    
    // Error occured ? 
    if(U1IRbits.UERRIF) {
        
        errorOccured();
        
        clearflags |= _U1IR_UERRIF_MASK;
    }
    
    // Bus Reset occured ? 
    if(U1IRbits.URSTIF) {

        resetOccured();
        
        clearflags |= _U1IR_URSTIF_MASK;        
    }
        
    // clear the int flag: makes pop the STATS register to the next entry
    U1IRCLR = clearflags; 
}



void UsbManager::initEndpoint0() {

}

void UsbManager::setupInterrupt() {

}

void UsbManager::initAsHost() {
}

void UsbManager::initAsDevice() {
    
    U1PWRCbits.USBPWR = 1; // power on usb periphal
    U1ADDR = 0; // device address is 0 by default //TODO: move to device class
    
    // interrupts we want to get
    U1IEbits.TRNIE = 1;  // transfer complete
    U1IEbits.URSTIE = 1; // reset

    IEC1bits.USBIE = 1; // Enable USB interrupts
    IPC11bits.USBIP = 6;  // Set priority level must reflect the level in config
    IPC11bits.USBIS = 0;  // Set sublevel    
}  

void UsbManager::initAsOtg() {
    
}


void UsbManager::transferCompleted() {

    UInt8 transfep = U1STATbits.ENDPT;
    bool tx = (U1STATbits.DIR == 1);
    bool oddbank = (U1STATbits.PPBI == 1);
    
    if(transfep >= USB_NR_EP) return; 
    ep[transfep].transfer(tx,oddbank);
    
    
    
}

void UsbManager::resetOccured() {
    
}

void UsbManager::errorOccured() {
    
}

// UsbDeviceManager
void UsbDeviceManager::dataReceived(UInt8* data, UInt16 len, bool iscontrol ) {
    
}

void UsbDeviceManager::sendCompleted(bool error) {
    
}

UsbDeviceManager::UsbDeviceManager()  {
    initAsDevice();
};