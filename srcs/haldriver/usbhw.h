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


struct __attribute__((packed))  UsbBufferDescriptor {
    
    UInt16 transcount; // only 10 bits are used. Upper 6 bits are unused
    
    unsigned :8;
    
    unsigned uown:1;
    unsigned data01:1;
    unsigned pid:4;
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

template<Int32 I>
struct EndPointBuffer {
    UInt8 rxeven[I]; 
    UInt8 rxodd[I];    
    UInt8 txeven[I];
    UInt8 txodd[I];
};


template<Int32 I>
class __attribute__((packed))  UsbBDTable {

public:
    
    UsbEndpointBD endpoint[I];
    EndPointBuffer<64> ep0data; // endpoint 0 must have at least 64 bytes

    UsbBDTable();

    void initEndpoint(UInt8 epnr);
};


#endif /* USBHW_H */

/* *****************************************************************************
 End of File
 */
