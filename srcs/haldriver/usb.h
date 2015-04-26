/*
 * Pic32CopterCpp -- A C++ microOS for the PIC32
 *
 * Copyright (C) 2012 - 2015, Georg Campana
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
 * Created on 25 aprile 2015, 20.45
 */

#ifndef USB_H
#define	USB_H

#include "../kernel/basetypes.h"

enum DescriptorType {  //  DescriptorType : UInt8  (underlying data now working with MC compiler)
    USBDESCR_DEVICE     = 0x01,
    USBDESCR_CONFIG,
    USBDESCR_STRING,
    USBDESCR_INTERFACE,
    USBDESCR_ENDPOINT,
    USBDESCR_DEV_QUALIFIER,
    USBDESCR_OTHER_SPEED,
    USBDESCR_INTERF_PWR,
    USBDESCR_OTG
};


// DEVICE
struct
__attribute__ ((packed))
UsbDeviceDescriptor
{
    UInt8 lenght;               // lenght of the descriptor
    UInt8 type;                 // descriptor type
    UInt16 bcdusb;              // USB Spec Release Number (BCD).
    UInt8 devclass;             // Class code (assigned by the USB-IF). 0xFF-Vendor specific.
    UInt8 sublass;              // Subclass code (assigned by the USB-IF).
    UInt8 protocol;             // Protocol code (assigned by the USB-IF). 0xFF-Vendor specific.
    UInt8 ep0maxpktsize;        // endpoint 0 max packet size.
    UInt16 vendorid;            // Vendor ID (assigned by the USB-IF).
    UInt16 productid;           // Product ID (assigned by the manufacturer).
    UInt16 bcddevice;           // Device release number (BCD).
    UInt8 manufacturerindex;    // String index of the manufacturer
    UInt8 productindex;         // String index describing the product.
    UInt8 serialnumber;         // String index with the device's serial number.
    UInt8 numconfig;            // Number of aailable configurations.
};


// CONFIGURATION
enum UsbConfigAttribute {
    CfgAttrRequired     =   0x80,
    CfgAttrSelfPowered  =   0xC0,
    CfgAttrRemoteWakeup =   0xA0
};

struct
__attribute__ ((packed))
UsbConfigDescriptor
{
    UInt8 lenght;           // lenght of the descriptor
    UInt8 type;             // descriptor type
    UInt16 totalenght;      // tot lenght for all descr. in this config
    UInt8  numinterfaces;   // interfaces in this config
    UInt8 configvalue;      // value for this config
    UInt8 stringindex;      // index of the descr. string
    UInt8 attributes;       // config attributes
    UInt8 maxpower;         // power needed for this config
};

// ENDPOINT
enum UsbEndpointTransfer {
    EpTransferControl   =   0x00,
    EpTransferIsoCh     =   0x01,
    EpTransferBulk      =   0x02,
    EpTransferInterrupt =   0x03
};

enum UsbEndpointSyncType {
    EpIsoNosync         =   (0<<2),
    EpIsoAsync          =   (1<<2),
    EpIsoAdaptSync      =   (2<<2),
    EpIsoSync           =   (3<<2)
};

enum UsbEndpointIsoUsage {
    EpIsoData           =   (0<<4),
    EpIsoFeedback       =   (1<<4),
    EpIsoImplFeedback   =   (2<<4)
};

enum UsbEndpointSyncType {
    EpIsoNosync         =   (0<<2),
    EpIsoAsync          =   (1<<2),
    EpIsoAdaptSync      =   (2<<2) ,
    EpIsoSync           =   (3<<2)
};

struct
__attribute__ ((packed))
UsbEndpointDescriptor
{
    UInt8 lenght;           // lenght of the descriptor
    UInt8 type;             // descriptor type
    UInt8 address_dir;      // address and direction in  Bit 7 (0=OUT, 1=IN).
    UInt8 attributes;       // transfer type.
    UInt16 maxkptsize;      // max packet size.
    UInt8 pollinterval;     // polling interval expressed in frames.
};


#endif	/* USB_H */
