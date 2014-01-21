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
 * File:   device.h
 * Author: georg
 *
 * Created on 1 settembre 2013, 0.44
 */

#ifndef DEVICE_H
#define	DEVICE_H

#include "semaphore.h"

class DeviceBase : Node {
    Int32 opencnt;

public:

    DeviceBase() ;

    virtual bool open(Int32 maxms = -1);
    virtual void close();

protected:
    bool TaskDefaultWait(TaskBase* taskwillwait, Int32 maxmx = -1);
    void SignalWaitingTask(TaskBase* taskwillwait1);

};

class CharStreamDevice : public DeviceBase {

public:

//    CharStreamDevice() {}

    virtual UInt16 write(const Char* string2write) { return 0;}
    virtual UInt16 write(const Char* chars2write, UInt32 len) { return 0;}
    virtual bool write(Char chart2write) { return false;}

    virtual UInt16 countRxChars() const {return 0;}
    virtual Int16 getChar() {return -1;}
    virtual UInt16 readLine(UInt8* dest, UInt16 maxlen) { return 0 ;}


    virtual void setRxBuffer(Char* rxbuffer, UInt32 len) { return;  }
};

class SingleAccessDevice : public DeviceBase {

    NestableSemaphore access;

protected:
    TaskBase* waitingtask;
    Int32 blockingtimeout;

public:
    SingleAccessDevice() : waitingtask(NULL), blockingtimeout(-1) {}

    bool open(Int32 maxms = -1);
    void close();
    // for Sync/blocking implementations
    void setBlockingTimeout(Int32 mstimeout) {  blockingtimeout=mstimeout; }

protected:
    void WaitForTransfer() ;

};

class SingleAccessStreamDevice : public CharStreamDevice {

    NestableSemaphore access;

protected:
    TaskBase* waitingtask;
    Int32 blockingtimeout;

public:
    SingleAccessStreamDevice() : waitingtask(NULL), blockingtimeout(-1) {}

    bool open(Int32 maxms = -1);
    void close();
    // for Sync/blocking implementations
    void setBlockingTimeout(Int32 mstimeout) {  blockingtimeout=mstimeout; }

protected:
    void WaitForTransfer() ;
};

#endif	/* DEVICE_H */

