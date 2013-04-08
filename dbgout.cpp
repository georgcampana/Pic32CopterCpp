/*
 * File:   dbgout.c
 * Author: Georg Campana
 *
 * Created on 22 ottobre 2012, 0.29
 */

#include "include/dbgout.h"

#include <p32xxxx.h>
#include <plib.h>


#ifndef DEBUG

// let me avoid to include heavy headers
template<typename T>
static void itoa(char * buf, T val) {

    if(val==0) *buf++= '0';
    else {
        if(val<0) {
            val=-val;
            *buf++ = '-';
        }

        register char* origptr = buf;
        T quot;
        while(val != 0) {
            quot = val / 10; // could be a variable
            *buf++ = '0' + (val - (quot*10));
            val = quot;
        }

        register char* endptr = buf;
        endptr--;
        char tmpchar;
        while(origptr < endptr) {
            tmpchar = *endptr;
            *endptr-- = *origptr;
            *origptr++ = tmpchar;
        }
    }

    *buf = '\0'; // final null
}


static void uitoa(char * buf, unsigned int val);

//extern char* itoa(char * buf, int val, int base);
//extern char* uitoa(char * buf, unsigned int val, int base);

DebugConsole::DebugConsole(UartManager& serialif) : outconsole(serialif) {
    asciibuffer[0] = NULL;
}

DebugConsole& DebugConsole::operator << (int lnumber) {
    itoa<int>(asciibuffer, lnumber);
    outconsole.write(asciibuffer);
    return *this;
}

DebugConsole& DebugConsole::operator << (unsigned int ulnumber) {
    itoa<unsigned int>(asciibuffer, ulnumber);
    outconsole.write(asciibuffer);
    return *this;
}

DebugConsole& DebugConsole::operator << (const char* string) {
    outconsole.write(string);
    return *this;
}

DebugConsole& DebugConsole::operator << (bool onezero) {
    if(onezero == true) outconsole.write("true");
    else  outconsole.write("false");
    return *this;
}

DebugConsole& DebugConsole::operator << (float fnumber) {
    return *this;
}


#else

DebugConsole::DebugConsole(UartManager& serialif): outconsole(serialif)  {}

DebugConsole& DebugConsole::operator << (int ) {}

DebugConsole& DebugConsole::operator << (unsigned int ) {}

DebugConsole& DebugConsole::operator << (const char* ) {}

DebugConsole& DebugConsole::operator << (bool ) {}

DebugConsole& DebugConsole::operator << (float ) {}
#endif