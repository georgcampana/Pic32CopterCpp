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


UsbManager::UsbManager() {
    usb_ref = this;

}

void UsbManager::handleInterrupt() {

    // Clear Interrupt
    //if(IFS1bits.USBIF) {
        IFS1CLR = IFS1_USB_BIT; // USBIF
    //}
        
    // let's check where the Int arrives from
        
    // Packet Transfer complete ? (token))
    if(U1IRbits.TRNIF) {
        
        transferCompleted();
        
        // clear the int flag: makes pop the STATS register to the next entry
        U1IRCLR = _U1IR_TRNIF_MASK;
    }
    
    // Bus Reset occured ? 
    if(U1IRbits.URSTIF) {
        // clear the int flag: makes pop the STATS register to the next entry
        
        resetOccured();
        
        U1IRCLR = _U1IR_URSTIF_MASK;        
    }

    // Error occured ? 
    if(U1IRbits.UERRIF) {
        // clear the int flag: makes pop the STATS register to the next entry
        U1IRCLR = _U1IR_UERRIF_MASK;        
    }
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
    
    IEC1bits.USBIE = 1; // Enable USB interrupts
    IPC11bits.USBIP = 6;  // Set priority level must reflect the level in config
    IPC11bits.USBIS = 0;  // Set sublevel
        
}  

void UsbManager::initAsOtg() {
    
}


void UsbManager::transferCompleted() {
    
}

void UsbManager::resetOccured() {
    
}