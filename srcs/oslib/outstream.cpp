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
 * File:   outstream.cpp
 * Author: georg
 *
 * Created on 1 settembre 2013, 22.18
 */

#include "outstream.h"

// let me avoid to include heavy headers
template<typename T>
static void itoa(Char * buf, T val) {

    if(val==0) *buf++= '0';
    else {
        if(val<0) {
            val=-val;
            *buf++ = '-';
        }

        register Char* origptr = buf;
        T quot;
        while(val != 0) {
            quot = val / 10; // could be a variable
            *buf++ = '0' + (val - (quot*10));
            val = quot;
        }

        register Char* endptr = buf;
        endptr--;
        Char tmpchar;
        while(origptr < endptr) {
            tmpchar = *endptr;
            *endptr-- = *origptr;
            *origptr++ = tmpchar;
        }
    }

    *buf = '\0'; // final null
}


static void uitoa(Char * buf, UInt32 val);

//extern char* itoa(char * buf, int val, int base);
//extern char* uitoa(char * buf, unsigned int val, int base);

OutStream::OutStream(CharStreamDevice& serialif) : outdevice(serialif) {
    outdevice.open();
    endl = serialif.getEol();
}

OutStream::~OutStream(){
    outdevice.close();
}

OutStream& OutStream::operator << (Int32 lnumber) {
    Char asciibuffer[32]; // should be enough to accomodate long floats number too
    asciibuffer[0] = 0;
    itoa<Int32>(asciibuffer, lnumber);
    outdevice.write(asciibuffer);
    return *this;
}

OutStream& OutStream::operator << (UInt32 ulnumber) {
    Char asciibuffer[32]; // should be enough to accomodate long floats number too
    asciibuffer[0] = 0;
    itoa<UInt32>(asciibuffer, ulnumber);
    outdevice.write(asciibuffer);
    return *this;
}

OutStream& OutStream::operator << (UInt8 unumber) {
    outdevice.write(unumber);
    return *this;
}

OutStream& OutStream::operator << (const Char* string) {
    outdevice.write(string);
    return *this;
}

OutStream& OutStream::operator << (bool onezero) {
    if(onezero == true) outdevice.write("true");
    else  outdevice.write("false");
    return *this;
}

OutStream& OutStream::operator << (float fnumber) {
    return *this;
}
