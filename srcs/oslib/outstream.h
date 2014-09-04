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
 * File:   outstream.h
 * Author: Georg Campana
 *
 * Created on 1 settembre 2013, 22.18
 */

#ifndef OUTSTREAM_H
#define	OUTSTREAM_H

#include "../kernel/device.h"

class OutStream {
    CharStreamDevice& outdevice;

public:
    Char const* endl;

    OutStream(CharStreamDevice& serialif);
    ~OutStream();

    OutStream& operator << (Int32 number) ;
    OutStream& operator << (UInt32 unumber) ;
    OutStream& operator << (Int8 cnumber) ;
    OutStream& operator << (UInt8 ucnumber) ;
    OutStream& operator << (const Char* string) ;
    OutStream& operator << (bool onezero) ;
    OutStream& operator << (float fnumber) ;
};

inline  OutStream& OutStream::operator << (Int8 cnumber) { return this->operator <<((Int32)cnumber);};
//inline  OutStream& OutStream::operator << (UInt8 ucnumber) { return this->operator <<((UInt32)ucnumber);};

#endif	/* OUTSTREAM_H */

