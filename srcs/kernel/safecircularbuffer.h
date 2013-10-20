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

template<Int32 I>
class SafeCircularBuffer {

    UINT8 buffer[I];

    UINT8 *wrptr;  // read/written only by writer thread/interrupt
    UINT8 *rdptr;  // read/written only by reader thread/interrupt
    UINT32 datalen; // managed using atomic operations

public:
    SafeCircularBuffer();

    bool putChar(char chartoput);
    INT16 getChar();
    void reset();
    UINT16 getDataLen() const;
};

template<Int32 I>
inline SafeCircularBuffer<I>::SafeCircularBuffer() {
    reset();
}

template<Int32 I>
inline bool SafeCircularBuffer<I>::putChar(char char2put) {
    if(datalen == I) { // buffer is full
        return false;
    }
    *wrptr++ = char2put;
    HAL::AtomicInc((Int32*)&datalen);
    if(wrptr == &buffer[I]) wrptr = buffer;

    return true;
}

template<Int32 I>
inline INT16 SafeCircularBuffer<I>::getChar() {

    if(datalen > 0) {
        UINT8 char2return = *rdptr++;
        HAL::AtomicDec((Int32*)&datalen);
        if(rdptr == &buffer[I]) rdptr = buffer;
        return char2return;
    }

    return -1;
}

template<Int32 I>
inline void SafeCircularBuffer<I>::reset() {
    datalen = 0;
    wrptr = rdptr = buffer;
}

template<Int32 I>
inline UINT16 SafeCircularBuffer<I>::getDataLen() const {
    return datalen;
}

#endif	/* SAFECIRCULARBUFFER_H */

