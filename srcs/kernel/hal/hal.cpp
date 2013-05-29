/* 
 * File:   hal.cpp
 * Author: georg
 * 
 * Created on 29 aprile 2013, 17.43
 */

#include "hal.h"

unsigned int HAL::lastreadticks = 0;
unsigned int HAL::highticks = 0;

void HAL::Init() {

    ResetTickTimer();


    // let's init the timer and interrupts
    // configure for multi-vectored mode
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    // enable interrupts
    INTEnableInterrupts();
}

#ifdef	__cplusplus
extern "C" {
#endif



// this runs on a separate stack (interrupts stack)->original Task stack already saved
char* handleSysTimerINT() {

}



__attribute__((vector(_CORE_TIMER_VECTOR), nomips16)) void myISR() {

    
}


#ifdef	__cplusplus
}
#endif