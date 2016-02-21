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
 * File:   usbdevmanager.h
 * Author: Georg Campana
 *
 * Created on 25 aprile 2015, 20.30
 */

#ifndef USBMANAGER_H
#define	USBMANAGER_H

#include "../kernel/system.h"
#include "../kernel/device.h"

#include "../driver/usb.h"
#include "usbhw.h"

#ifndef USB_NR_DEFAULT_EP
    // Max numbers of EP to allocate. By default 4 which should be enough for most of the use cases.
#define USB_NR_DEFAULT_EP 4 
#endif //USB_NR_DEFAULT_EP 

class UsbEpHandler {
    
    UsbEndpointBD& descriptor;

public:
    
    UsbEpHandler(UsbEndpointBD& descr);
};

// Note: on Pic32 only one instance is allowed (due to static BDT)
class UsbManager : public SingleAccessDevice {

    static __attribute__ ((aligned(512))) UsbBDTable<USB_NR_DEFAULT_EP> bdt;
  
    void initAsHost();
    void initAsDevice();   
    void initAsOtg();   
    
    void initEndpoint0();
    void setupInterrupt();
    
    void transferCompleted();
    void resetOccured();
    
    
    
public:
    UsbManager();
    
    void handleInterrupt();
};

#endif	/* USBMANAGER_H */
