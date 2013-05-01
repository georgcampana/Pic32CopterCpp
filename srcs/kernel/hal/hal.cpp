/* 
 * File:   hal.cpp
 * Author: georg
 * 
 * Created on 29 aprile 2013, 17.43
 */

#include "hal.h"


#ifdef	__cplusplus
extern "C" {
#endif

// defined externally in assembly

//void __ISR(_CORE_TIMER_VECTOR, ipl3) swappContext(void) {
//
//}

int testAsm(int a) {
   return (a*3);
}

int testMasm(int a, int b) {
   return testAsm(a)+b;
}



#ifdef	__cplusplus
}
#endif