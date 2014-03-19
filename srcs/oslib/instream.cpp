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
 * File:   instream.cpp
 * Author: Georg Campana
 *
 * Created on 5 marzo 2014, 11.47
 */

#include "instream.h"


const Char InStream::defaultseparators[] = ", \n\r";
const InStream::NilClazz Nil();


Int64 InStream::Ascii2Integer(UInt64 maxpositive, UInt64 minabsnegative) {
    bool isnegative = false;
    Int64 number = 0;

    Int16 currentchar = indevice.getChar(); // should wait (or timeout)
    if((Char)currentchar == '+') { currentchar = indevice.getChar(); }
    else if((Char)currentchar == '-') { currentchar = indevice.getChar(); isnegative == true;}

    while (currentchar != -1 && (Char)currentchar >= '0' && (Char)currentchar <='9') {
        number = (number<<3)+(number<<1)+(currentchar)-'0';
        currentchar = indevice.getChar();
    }

    // no more numbers.. we check if we have a separator now
    bool separatorfound = false;
    Char const* eol = indevice.getEol();
    if(eol[1] != NULL && (Char)currentchar == eol[0]) { currentchar = indevice.getChar(); }
    if(currentchar!=-1) {
        for(Char const* currseparator = fieldseparators; *currseparator != NULL; currseparator++) {
            if((Char)currentchar == *currseparator) {
                separatorfound=true;
                break;
            }
        }
        if(separatorfound==false) {
            lasterror = IS_NotANumber;
            // let's seek for the next separator
            while(separatorfound==false && currentchar!=-1) {
                currentchar = indevice.getChar();
                for(Char const* currseparator = fieldseparators; *currseparator != NULL; currseparator++) {
                    if((Char)currentchar == *currseparator) {
                        separatorfound=true;
                        break;
                    }
                }

            }
        }
    }
    
    if(currentchar==-1) {
        // timeout error
        lasterror = IS_Timeout;
    }



    if(isnegative) {
        if(number > minabsnegative) lasterror = IS_DecimatedToFit;
        number = -number;
    }
    else {
        if(number > maxpositive) lasterror = IS_DecimatedToFit;
    }

    return number;
}


void InStream::setSeparators(const Char* newseparators) {
    fieldseparators = (newseparators)?  newseparators : defaultseparators;
}

void InStream::setMaxStringLen(Int32 maxchars) {
    maxstrlen = maxchars;
}

InStream::InStream(CharStreamDevice& serialif, Int32 initialmaxlen) :
                          indevice(serialif), fieldseparators(defaultseparators),
                          maxstrlen(initialmaxlen), lasterror(IS_OK) {

}
//    InStream::~InStream();

InStream& InStream::operator >> (Int32& number) {

    Int64 readnumber = Ascii2Integer(0x7fffffff, 0x80000000);
    number = (Int32)readnumber; // possible loss of information (marked as error by Ascii2Integer)

    return *this;
}

InStream& InStream::operator >> (UInt32& unumber) {
    Int64 readnumber = Ascii2Integer(0x7fffffff, 0x7fffffff);
    if(readnumber < 0) {
        readnumber=-readnumber;
        if(lasterror == IS_OK) lasterror = IS_NegativeInUnsigned;
    }
    unumber = (UInt32)readnumber; // possible loss of information (marked as error by Ascii2Integer)

    return *this;
}

InStream& InStream::operator >> (Char* string) {

    bool separatorfound = false;
    Int32 maxchars = maxstrlen-1; // reserve one char for final string NULL

    while(separatorfound==false && maxchars > 0) {
        Int16 currentchar = indevice.getChar(); // should wait (or timeout)
        if(currentchar == -1) {
            lasterror = IS_Timeout;
            break;
        }
        for(Char const* currseparator = fieldseparators; *currseparator != NULL; currseparator++) {
            if((Char)currentchar == *currseparator) {
                separatorfound=true;
                break;
            }
        }
        if(separatorfound==false) {
            // ok it's a char that is not a separator or a timeout: We copy it
            *string++ = (Char)currentchar;
            maxchars--;
        }
    }

    *string = NULL; // space ensured above

    if(maxchars == 0 && separatorfound == false) {
        lasterror = IS_StringOverflow;
    }

    return *this;
}


InStream& InStream::operator >> (const NilClazz& skip) {

    bool separatorfound = false;

    while(separatorfound==false) {
        Int16 currentchar = indevice.getChar(); // should wait (or timeout)
        if(currentchar == -1) {
            lasterror = IS_Timeout;
            break;
        }
        for(Char const* currseparator = fieldseparators; *currseparator != NULL; currseparator++) {
            if((Char)currentchar == *currseparator) {
                separatorfound=true;
                break;
            }
        }
    }

    return *this;
}


InStream& InStream::operator >> (Char& singlechar) {
    Int16 currentchar = indevice.getChar(); // should wait (or timeout)
    if(currentchar != -1) {
        lasterror = IS_Timeout;
    }
    else {
        singlechar = (Char)currentchar;
    }

    return *this;
}

InStream& InStream::operator >> (bool& onezero) {

    return *this;
}

InStream& InStream::operator >> (float& fnumber) {

    return *this;
}
