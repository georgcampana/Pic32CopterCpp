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
 * File:   instream.h
 * Author: Georg Campana
 *
 * Created on 5 marzo 2014, 11.47
 */

#ifndef INSTREAM_H
#define	INSTREAM_H

#include "../kernel/device.h"




class InStream {
    CharStreamDevice& indevice;

    Char const* fieldseparators;
    static const Char defaultseparators[];
    Int32 maxstrlen;
    Int64 Ascii2Integer(UInt64 maxpositive, UInt64 minabsnegative);
public:
    const static class NilClazz{ public: NilClazz() {} } Nil ;

    enum IS_Error {
        IS_OK = 0,
        IS_NotANumber,
        IS_StringOverflow,
        IS_DecimatedToFit,
        IS_Timeout, // no more data received
        IS_NegativeInUnsigned
    } lasterror;

    void resetError();
    void setSeparators(const Char* newseparators);
    void setMaxStringLen(Int32 maxchars);

    InStream(CharStreamDevice& serialif, Int32 initialmaxlen=0);
//    ~InStream();

    InStream& operator >> (Int32& number) ;
    InStream& operator >> (UInt32& unumber) ;
    InStream& operator >> (Int8& cnumber) ;
    InStream& operator >> (UInt8& ucnumber) ;
    InStream& operator >> (Char* string) ;
    InStream& operator >> (Char& singlechar) ;
    InStream& operator >> (bool& onezero) ;
    InStream& operator >> (float& fnumber) ;
    InStream& operator >> (const NilClazz& skip) ;
};

inline  InStream& InStream::operator >> (Int8& cnumber) { return this->operator >>((Int32&)cnumber);};
inline  InStream& InStream::operator >> (UInt8& ucnumber) { return this->operator >>((UInt32&)ucnumber);};
inline  void InStream::resetError() { lasterror = IS_OK; }



#endif	/* INSTREAM_H */

