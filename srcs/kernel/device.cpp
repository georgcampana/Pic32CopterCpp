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
 * File:   device.cpp
 * Author: georg
 *
 * Created on 20 october 2013, 0.44
 */

#include "device.h"
#include "../kernel/kernel.h"

DeviceBase::DeviceBase() : Node(NT_DRIVER), opencnt(0)  {}

bool DeviceBase::open(Int32 maxms) { opencnt++;}

void DeviceBase::close() { if(opencnt>0) opencnt--;}

// returns false in case of timeout
bool DeviceBase::TaskDefaultWait(TaskBase* taskwillwait, Int32 maxmx) {
    return (taskwillwait->GetWaitItem()->Wait(maxmx) != 0);
}

void DeviceBase::SignalWaitingTask(TaskBase* waitingtask) {
    waitingtask->GetWaitItem()->Signal();
}

bool SingleAccessDevice::open(Int32 maxms) {
    return access.ObtainNested(maxms);
}

void SingleAccessDevice::close() {
    access.ReleaseNested();
}


bool SingleAccessStreamDevice::open(Int32 maxms) {
    return access.ObtainNested(maxms);
}

void SingleAccessStreamDevice::close() {
    access.ReleaseNested();
}

