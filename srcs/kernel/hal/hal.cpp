/* 
 * File:   hal.cpp
 * Author: georg
 * 
 * Created on 29 aprile 2013, 17.43
 */

#include "hal.h"

UInt32 HAL::lastreadticks = 0;
UInt32 HAL::highticks = 0;
HAL::TimerAlarm* HAL::inthandler = 0;
HAL::IntEpilogue* HAL::reschedhandler = 0;
char  HAL::intstack[INTERRUPTSTACKSIZE];

extern char* interruptstack; // defined in the assembler file

void HAL::Init() {
    // let's init the timer and interrupts

    ResetTickTimer();

    // set up the core timer interrupt with a prioirty of 1 and zero sub-priority
    mConfigIntCoreTimer((CT_INT_ON | CT_INT_PRIOR_1 | CT_INT_SUB_PRIOR_0));
    // The Core timer should halt when we are halted at a debug breakpoint.
    _CP0_BIC_DEBUG(_CP0_DEBUG_COUNTDM_MASK);
    // we set the stack pointer
    interruptstack = intstack + INTERRUPTSTACKSIZE; // because it grows downwards

    // enable multi-vector interrupts
    INTEnableSystemMultiVectoredInt();

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