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

int testMasmDue(int a, int b, int c, int d, int e) {
   return testAsm(a)+b+c+d+e;
}

int testCalAsm() {
    return testMasmDue(11,12,13,14,15);
}
// this runs on a separate stack (interrupts stack)->original Task stack already saved
char* handleSysTimerINT() {

}

void testTaskContext(char** from_context_sp, char* to_context_sp) {

    *from_context_sp =  to_context_sp + 8;

}


#ifdef	__cplusplus
}
#endif