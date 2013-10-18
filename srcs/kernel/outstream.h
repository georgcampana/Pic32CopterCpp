/* 
 * File:   outstream.h
 * Author: gcamp_000
 *
 * Created on 1 settembre 2013, 22.18
 */

#ifndef OUTSTREAM_H
#define	OUTSTREAM_H

#include "device.h"

class OutStream {
    CharStreamDevice& outdevice;

public:
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
inline  OutStream& OutStream::operator << (UInt8 ucnumber) { return this->operator <<((UInt32)ucnumber);};

#endif	/* OUTSTREAM_H */

