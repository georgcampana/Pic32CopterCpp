/* 
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


// singleton class (everything is static)
class System {

public:
    static Int32 dbgcounter;



};



#endif	/* SYSTEM_H */

