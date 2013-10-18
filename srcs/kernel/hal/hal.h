/* 
 * File:   hal.h
 * Author: georg
 *
 * Created on 29 aprile 2013, 17.43
 */

#ifndef HAL_H
#define	HAL_H

#include "./../system.h" // TODO: should be more generic

#ifdef	__cplusplus
extern "C" {
#endif

// assembly Task switch function
extern void swapTaskContext(char** from_context_sp, char* to_context_sp);
extern void* forkTask(void* newtaskpointer, char** newstackpointer, int stacksize, void (*exefunction)(void*));
extern void transferMainStack(char** context_sp, int stacksize);
// other asm functions
extern int getInterruptLevel();

extern int atomicInc(int* val2inc);
extern int atomicDec(int* val2dec);
extern int lockTestAndSet(int* val2set, int value);

#ifdef	__cplusplus
}
#endif

// TODO: should be moved in a common config file (system.h or a config.h)
#define INTERRUPTSTACKSIZE  1024

class HAL {
public:
    typedef long long TICKS; // 64 bit counter for the ticks (should last for 7000 years)
    
    class TimerAlarm {
     public:
        virtual bool HandleAlarm() { return true; };
    };

    class IntEpilogue {
      public:
        virtual char* RescheduleIfNeeded(char* lastsp) { return lastsp; };
    };

    static bool InterruptRunning();
    static UInt32 DisableInterrupts();
    static void RestoreInterrupts(UInt32 oldstatus);

    static UInt32 DisableScheduler();
    static void RestoreScheduler(UInt32 oldstatus);

    static TICKS GetCurrentTicks();
    static void SetNextAlarm(TICKS alarmticks);
    static void ResetTickTimer();
    static TICKS ConvertTime2Ticks(UInt32 ms, UInt32 us=0);

    static void Init();
    static void SetAlarmHandler(TimerAlarm* handler);
    static TimerAlarm* GetAlarmHandler();

    static void SetRescheduleHandler(IntEpilogue* handler);
    static IntEpilogue* GetRescheduleHandler();

    static Int32 AtomicInc(Int32* val2inc);
    static Int32 AtomicDec(Int32* val2dec);
    static Int32 LockTestAndSet(Int32* val2set, Int32 value);

    static void NothingToDo();
private:

    static char intstack[INTERRUPTSTACKSIZE];

    static const UInt32 MINDELTATICKS = 500; // must be set to real needed code overhead

    static UInt32 lastreadticks;
    static UInt32 highticks;
    static TimerAlarm* inthandler;
    static IntEpilogue* reschedhandler;
};

inline bool HAL::InterruptRunning() { return ::getInterruptLevel();}

inline UInt32 HAL::DisableInterrupts() { return ::INTDisableInterrupts();}
inline void HAL::RestoreInterrupts(UInt32 oldstatus) {::INTRestoreInterrupts(oldstatus);}

inline UInt32 HAL::DisableScheduler() { return DisableInterrupts();}
inline void HAL::RestoreScheduler(UInt32 oldstatus) {RestoreInterrupts(oldstatus);}

// commented out because kernel MUST call the asm call directly to avoid another subroutine level
//inline void HAL::SwapTaskContext(char** from_context_sp, char* to_context_sp) {
//    swapTaskContext(from_context_sp,to_context_sp);
//}
//inline bool HAL::ForkTask(void** ptaskpointer,void* taskpointer, char** stackpointer, int stacksize) {
//    return forkTask(ptaskpointer,taskpointer,stackpointer,stacksize);
//}
//inline void HAL::TransferMainStack(char** context_sp, int stacksize) {
//    transferMainStack(context_sp,stacksize);
//}

inline HAL::TICKS HAL::GetCurrentTicks() {
    UInt32 newreadticks = ReadCoreTimer();
    if(newreadticks < lastreadticks) {
        highticks++; // here we assume that we MUST have an alarm at least every 30 secs
                     // which should be maxticks in the core timer divided by 2
    }
    lastreadticks = newreadticks;
    return ((TICKS)highticks)<<32 | lastreadticks ;
}

inline void HAL::SetNextAlarm(HAL::TICKS alarmticks) {

    TICKS now = GetCurrentTicks();
    TICKS delta = alarmticks-now;

    if(delta < MINDELTATICKS) {
        alarmticks += (MINDELTATICKS-delta);
    }

    // NOTE: UpdateCoreTimer is not suitable because it sums the value to the old in compare
    // set up the period in the compare reg
    asm volatile("mtc0   %0,$11" : "+r"(alarmticks));
}

inline void HAL::ResetTickTimer() {
    OpenCoreTimer(0xffffffff); // this prevent us from getting
                               // an immediate spurious core timer interrupt
    lastreadticks = 0;
    highticks = 0;
}

#define TICKS_PER_MS    (SYS_CLOCK / 2 / 1000)
#define TICKS_PER_US    (SYS_CLOCK / 2 / 1000000)

inline HAL::TICKS HAL::ConvertTime2Ticks(UInt32 ms, UInt32 us) {
    return HAL::TICKS ((ms*TICKS_PER_MS) + (us*TICKS_PER_US));
}

inline void HAL::SetAlarmHandler(HAL::TimerAlarm* handler) {
    inthandler = handler;
}

inline HAL::TimerAlarm* HAL::GetAlarmHandler() {
    return inthandler;
}

inline void HAL::SetRescheduleHandler(HAL::IntEpilogue* handler) {
    reschedhandler = handler;
}

inline HAL::IntEpilogue* HAL::GetRescheduleHandler() {
    return reschedhandler;
}

inline Int32 HAL::AtomicInc(Int32* val2inc) {
    return::atomicInc((int*)val2inc);
}

inline Int32 HAL::AtomicDec(Int32* val2dec) {
    return ::atomicDec((int*)val2dec);
}

inline Int32 HAL::LockTestAndSet(Int32* var2set, Int32 value) {
    return ::lockTestAndSet((int*)var2set, (int)value);
}

inline void HAL::NothingToDo() {
    while(1){
        // TODO: seems that wait is blocking everything must be tuned better
        //_wait();
    }
}

#endif	/* HAL_H */

