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

    // set up the core timer interrupt with a prioirty of 1 and zero sub-priority
    mConfigIntCoreTimer((CT_INT_ON | CT_INT_PRIOR_1 | CT_INT_SUB_PRIOR_0));
    mEnableIntCoreTimer();
    mCTClearIntFlag();

    // we set the stack pointer
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

    mCTClearIntFlag();
}

char* RescheduleIfNeeded(char* lastsp) {
    return HAL::GetRescheduleHandler()->RescheduleIfNeeded(lastsp);
}

//void __ISR(_CORE_TIMER_VECTOR, ipl1) myISR(void)
//{
//        int coretimer = ReadCoreTimer();
//        coretimer++;
//}
//__attribute__((vector(_CORE_TIMER_VECTOR), nomips16)) void myISR() {
//
//        int coretimer = ReadCoreTimer();
//        coretimer++;
//}


#ifdef	__cplusplus
}
#endif