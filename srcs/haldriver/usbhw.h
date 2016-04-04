/*
 * Pic32CopterCpp -- A C++ microOS for the PIC32
 *
 * Copyright (C) 2012 - 2016, Georg Campana
 *
 * Georg Campana <g.campana(AT)stetel.com>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE.txt file
 * at the top of the source tree.
 *
 * File:   usbhw.h
 * Author: Georg Campana
 *
 * Created on 25 January 2016, 20.30
 */

#ifndef USBHW_H    /* Guard against multiple inclusion */
#define USBHW_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */
#include "../kernel/basetypes.h"

enum UsbPktId {
    PID_IN      = 0x9,
    PID_OUT     = 0x1,
    PID_SETUP   = 0xd
};

struct __attribute__((packed))  UsbBufferDescriptor {
    
    UInt16 transfcount; // only 10 bits are used. Upper 6 bits are unused
    
    unsigned :8;
    
    unsigned uown:1;
    unsigned data01:1;
    UsbPktId pid:4;
    unsigned :2;

    UInt8* buffer;
};


struct __attribute__((packed))  
UsbEndpointBD {
    UsbBufferDescriptor rxeven;
    UsbBufferDescriptor rxodd;
    UsbBufferDescriptor txeven;
    UsbBufferDescriptor txodd;
};

struct __attribute__((packed))  
UsbEndpointBDVector { 
    UsbBufferDescriptor vector[2][2]; // rx/tx and even/odd
};

template<Int32 B>
struct EndPointBuffer {
    UInt8 rxeven[B]; 
    UInt8 rxodd[B];    
//    UInt8 txeven[B];
//    UInt8 txodd[B];
};


template<Int32 E>
class __attribute__((packed))  UsbBDTable {

public:
    
    UsbEndpointBD endpoint[E];
    EndPointBuffer<64> ep0data; // endpoint 0 must have at least 64 bytes

    UsbBDTable();
    void initEndpoint(UInt8 epnr, UInt8* rxevenbuff=0, UInt8* rxoddbuff=0,
                                  UInt8* txevenbuff=0, UInt8* txoddbuff=0);
};

template<Int32 E>
inline  UsbBDTable<E>::UsbBDTable() {
    for(int c=0; c<sizeof(endpoint); c++) {((UInt8*)endpoint)[c];}
    initEndpoint(0, ep0data.rxeven, ep0data.rxodd);

    UInt32 physicaladdr =  KVA_TO_PA((unsigned int)endpoint);
    U1BDTP1 = (UInt8)((physicaladdr) >> 8);
    U1BDTP2 = (UInt8)((physicaladdr) >> 16);
    U1BDTP3 = (UInt8)((physicaladdr) >> 24);    
};

template<Int32 E>
inline void UsbBDTable<E>::initEndpoint(UInt8 epnr, UInt8* rxevenbuff, UInt8* rxoddbuff,
                                                 UInt8* txevenbuff, UInt8* txoddbuff){
    
    if(rxevenbuff)endpoint[epnr].rxeven.buffer  = (UInt8*)KVA_TO_PA((unsigned int)rxevenbuff);
    if(rxoddbuff) endpoint[epnr].rxodd.buffer   = (UInt8*)KVA_TO_PA((unsigned int)rxoddbuff);
    if(txevenbuff)endpoint[epnr].txeven.buffer  = (UInt8*)KVA_TO_PA((unsigned int)txevenbuff);
    if(txoddbuff) endpoint[epnr].txodd.buffer   = (UInt8*)KVA_TO_PA((unsigned int)txoddbuff);    

}



#endif /* USBHW_H */

/* *****************************************************************************
 End of File
 */
