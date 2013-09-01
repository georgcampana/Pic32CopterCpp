/*
 * File:   device.h
 * Author: gcamp_000
 *
 * Created on 1 settembre 2013, 0.44
 */

#ifndef DEVICE_H
#define	DEVICE_H

#include "basetypes.h"

class DeviceBase {
    Int32 opencnt;

public:

    DeviceBase() : opencnt(0)  {}

    virtual bool open() { opencnt++;}
    virtual void close() { if(opencnt>0) opencnt--;}

};

class CharStreamDevice : public DeviceBase {

public:

    virtual UInt16 write(const Char* string2write) { return 0;}
    virtual bool write(Char chart2write) { return false;}

    virtual UInt16 countRxChars() const {return 0;}
    virtual Int16 getChar() {return -1;}
    virtual UInt16 readLine(UInt8* dest, UInt16 maxlen) { return 0 ;}


};

#endif	/* DEVICE_H */

