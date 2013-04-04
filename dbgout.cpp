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
static void ltoa(char * buf, long val, int base) {
    if(val==0) *buf++= '0';
    else
    if(val<0) val=-val;

    while
}
static void ultoa(char * buf, unsigned long val, int base);


DebugConsole::DebugConsole(UartManager& serialif) : outconsole(serialif) {
    asciibuffer[0] = NULL;
}

DebugConsole& DebugConsole::operator << (long lnumber) {
    ltoa(asciibuffer, lnumber, 10);
    outconsole.write(asciibuffer);
    return *this;
}

DebugConsole& DebugConsole::operator << (unsigned long ulnumber) {
    ultoa(asciibuffer, ulnumber, 10);
    outconsole.write(asciibuffer);
    return *this;
}

DebugConsole& DebugConsole::operator << (const char* string) {
    outconsole.write(string);
    return *this;
}

DebugConsole& DebugConsole::operator << (bool onezero) {
    if(onezero == true) outconsole.write("true");
    else  outconsole.write("true"); 
    return *this;
}

DebugConsole& DebugConsole::operator << (float fnumber) {
    return *this;
}


#else

DebugConsole::DebugConsole(UartManager& serialif): outconsole(serialif)  {}

DebugConsole& DebugConsole::operator << (long ) {}

DebugConsole& DebugConsole::operator << (unsigned long ) {}

DebugConsole& DebugConsole::operator << (const char* ) {}

DebugConsole& DebugConsole::operator << (bool ) {}

DebugConsole& DebugConsole::operator << (float ) {}
#endif