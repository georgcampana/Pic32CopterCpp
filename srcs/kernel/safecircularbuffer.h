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
 * File:   safecircularbuffer.h
 * Author: gcamp_000
 *
 * Created on 1 settembre 2013, 19.00
 */

#ifndef SAFECIRCULARBUFFER_H
#define	SAFECIRCULARBUFFER_H

#include "hal/hal.h"


class SafeCircularBufferBase {

    UInt8* const buffer;
    const UInt32 bufflen;
    UInt8* const bufferend; //  buffer+bufferend

    UInt8 *wrptr;  // read/written only by writer thread/interrupt
    UInt8 *rdptr;  // read/written only by reader thread/interrupt
    UINT32 datalen; // managed using atomic operations

public:
    SafeCircularBufferBase(UInt8* buff, UInt32 len) ;

    bool putChar(char chartoput);
    INT16 getChar();
    void reset();
    UINT16 getDataLen() const;
};


inline SafeCircularBufferBase::SafeCircularBufferBase(UInt8* buff, UInt32 len) : buffer(buff), bufflen(len), bufferend(buff+len)  {
    reset();
}

inline bool SafeCircularBufferBase::putChar(char char2put) {
    if(datalen == bufflen) { // buffer is full
        return false;
    }
    *wrptr++ = char2put;
    HAL::AtomicInc((Int32*)&datalen);
    if(wrptr == bufferend) wrptr = buffer;

    return true;
}

inline INT16 SafeCircularBufferBase::getChar() {

    if(datalen > 0) {
        UINT8 char2return = *rdptr++;
        HAL::AtomicDec((Int32*)&datalen);
        if(rdptr == bufferend) rdptr = buffer;
        return char2return;
    }

    return -1;
}

inline void SafeCircularBufferBase::reset() {
    datalen = 0;
    wrptr = rdptr = buffer;
}

inline UINT16 SafeCircularBufferBase::getDataLen() const {
    return datalen;
}

template<Int32 I>
class SafeCircularBuffer : public SafeCircularBufferBase {

    UINT8 actualbuffer[I];

public:
    SafeCircularBuffer() : SafeCircularBufferBase(actualbuffer,I) {};

};

#endif	/* SAFECIRCULARBUFFER_H */