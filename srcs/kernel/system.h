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
 * File:   system.h
 * Author: Georg Campana
 *
 * Created on 24 dicembre 2012, 13.36
 */

#ifndef SYSTEM_H
#define	SYSTEM_H

#include "basetypes.h"

#include <p32xxxx.h>
#include <plib.h>


#ifdef	__cplusplus
extern "C" {
#endif
//any C code goes here

#ifdef	__cplusplus
}
#endif

// we are running at 80MHz
#define SYS_CLOCK (80000000L)
#define GetSystemClock()            (SYS_CLOCK)
#define	GetPeripheralClock()        (GetSystemClock()/(1 << OSCCONbits.PBDIV))
#define GetInstructionClock()       (SYS_CLOCK)

#ifdef NDEBUG
# ifndef DEBUG
#  define DEBUG
# endif
#endif


// singleton class (everything is static)
class System {

public:
#ifdef DEBUG
    static Int32 dbgcounter;
    static bool dbgflag;
#endif


};



#endif	/* SYSTEM_H */

