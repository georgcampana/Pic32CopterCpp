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

// USer can define following definition to set number of desired endpoints
//#define USB_NR_EP 

#ifndef USB_NR_EP
    // Max numbers of EP to allocate. By default 4 which should be enough for most of the use cases.
#define USB_NR_EP 4 
#endif //USB_NR_EP 


class IUsbEndpointClient {
    
public:     
    virtual void dataReceived(UInt8* data, UInt16 len ) {}; // not pure (=0) to prevent the need of a heap
    virtual void sendCompleted(bool error) {};
};

class UsbEpHandler {
    
 protected:
    UsbEndpointBD* descriptor;
    bool txisodd;
    bool rxisodd;
    UInt8 epnumber;
    volatile unsigned int *ctrlreg;
    IUsbEndpointClient* client;
 public:
    
    UsbEpHandler();
    
    // for the manager
    void init(UsbEndpointBD* descr, UInt8 epnr);
    void transfer(bool tx, bool odd);
    
    // for the client
    void sendData(UInt8* data, UInt16 len);
    void setEpClient(IUsbEndpointClient* epclient);
    
};
 


// Note: on Pic32 only one instance is allowed (due to static BDT)
class UsbManager : public SingleAccessDevice {

 protected:
    /*static*/ 
    __attribute__ ((aligned(512))) 
    UsbBDTable<USB_NR_EP> bdt;
    
    UsbEpHandler ep[USB_NR_EP];
  
    void initAsHost();
    void initAsDevice();   
    void initAsOtg();   
    
    void initEndpoint0();
    void setupInterrupt();
    
    void transferCompleted();
    void resetOccured();
    void errorOccured();
       
 public:
    UsbManager();
    
    void handleInterrupt();
};

class UsbDeviceManager : public UsbManager, IUsbEndpointClient {
    
 public:
    void dataReceived(UInt8* data, UInt16 len ); 
    void sendCompleted(bool error);
    
    UsbDeviceManager();
}; 




#endif	/* USBMANAGER_H */
