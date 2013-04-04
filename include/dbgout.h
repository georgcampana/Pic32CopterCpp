/* 
 * File:   dbgout.h
 * Author: gcamp_000
 *
 * Created on 4 aprile 2013, 16.18
 */

#ifndef DBGOUT_H
#define	DBGOUT_H

#include "uartmanager.h"


class DebugConsole {
    UartManager& outconsole;

    char asciibuffer[32]; // should be enogh to accomodate long floats number too

public:
    DebugConsole(UartManager& serialif);

    DebugConsole& operator << (int number) ;
    DebugConsole& operator << (unsigned int unumber) ;
    DebugConsole& operator << (char cnumber) ;
    DebugConsole& operator << (unsigned char ucnumber) ;
    DebugConsole& operator << (const char* string) ;
    DebugConsole& operator << (bool onezero) ;
    DebugConsole& operator << (float fnumber) ;
};

inline  DebugConsole& DebugConsole::operator << (char cnumber) { return this->operator <<((int)cnumber);};
inline  DebugConsole& DebugConsole::operator << (unsigned char ucnumber) { return this->operator <<((unsigned int)ucnumber);};

extern DebugConsole cout;


#endif	/* DBGOUT_H */

