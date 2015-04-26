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

#ifndef USBDEVMANAGER_H
#define	USBDEVMANAGER_H

#include "../kernel/system.h"
#include "../kernel/device.h"

class UsbDevice : public SingleAccessDevice {

public:
    UsbDevice();
};

#endif	/* USBDEVMANAGER_H */
