/* 
 * File:   hal.cpp
 * Author: georg
 * 
 * Created on 29 aprile 2013, 17.43
 */

#include "hal.h"

unsigned int HAL::lastreadticks = 0;
unsigned int HAL::highticks = 0;
HAL::TimerAlarm* HAL::inthandler = 0;
HAL::IntEpilogue* HAL::reschedhandler = 0;
char  HAL::intstack[INTERRUPTSTACKSIZE];

extern char* interruptstack; // edfined in the assembler file

void HAL::Init() {

    ResetTickTimer();

    // we set the stack pointer
    // TODO: the notion that it grows down is pic32 specific and should be moved into pic32.s
    interruptstack = intstack + INTERRUPTSTACKSIZE; // because it grows downwards

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
void handleSysTimerINT() {
    /*if()*/
    HAL::GetAlarmHandler()->HandleAlarm();
}

char* RescheduleIfNeeded(char* lastsp) {
    return HAL::GetRescheduleHandler()->RescheduleIfNeeded(lastsp);
}


//__attribute__((vector(_CORE_TIMER_VECTOR), nomips16)) void myISR() {
//
//
//}


#ifdef	__cplusplus
}
#endif