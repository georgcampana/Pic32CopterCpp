/* 
 * File:   hal.h
 * Author: georg
 *
 * Created on 29 aprile 2013, 17.43
 */

#ifndef HAL_H
#define	HAL_H

#include <p32xxxx.h>
#include <plib.h>

#ifdef	__cplusplus
extern "C" {
#endif

// assembly Task switch function
extern void swapTaskContext(char** from_context_sp, char* to_context_sp);
extern bool forkTask(void** ptaskpointer,void* taskpointer, char** stackpointer, int stacksize);
extern void transferMainStack(char** context_sp, int stacksize);
extern bool InterruptRunning();


#ifdef	__cplusplus
}
#endif

class HAL {
public:
    typedef long long TICKS; // 64 bit counter for the ticks (should last for 7000 years)
    
    class TimerAlarm {
     public:
        virtual bool HandleAlarm() { return true; };
    };

    static bool InterruptRunning();
    static unsigned int DisableInterrupts();
    static void RestoreInterrupts(unsigned int oldstatus);

    static unsigned int DisableScheduler();
    static void RestoreScheduler(unsigned int oldstatus);

    static TICKS GetCurrentTicks();
    static void SetNextAlarm(TICKS alarmticks);
    static void ResetTickTimer();
    static TICKS ConvertTime2Ticks(int ms, int us=0);

    static void Init();
    static void SetAlarmHandler(TimerAlarm* handler);
    static TimerAlarm* GetAlarmHandler();

    static void NothingToDo();
private:
    static const int MINDELTATICKS = 0; // must be set to real neede code overhead

    static unsigned int lastreadticks;
    static unsigned int highticks;
    static TimerAlarm* inthandler;

};

inline bool HAL::InterruptRunning() { return ::InterruptRunning;}

inline unsigned int HAL::DisableInterrupts() { return 0;}
inline void HAL::RestoreInterrupts(unsigned int oldstatus) {}

inline unsigned int HAL::DisableScheduler() { return DisableInterrupts();}
inline void HAL::RestoreScheduler(unsigned int oldstatus) {RestoreInterrupts(oldstatus);}

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
    unsigned int newreadticks = ReadCoreTimer();
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
        delta = MINDELTATICKS;
        alarmticks = now + delta;
    }

    UpdateCoreTimer(alarmticks & 0xffffffff); // the lowerpart
}

inline void HAL::ResetTickTimer() {
    OpenCoreTimer(0xffffffff); // this prevent us from getting
                               // an immediate spurious core timer interrupt
    lastreadticks = 0;
    highticks = 0;
}

inline HAL::TICKS HAL::ConvertTime2Ticks(int ms, int us) {

}

inline void HAL::SetAlarmHandler(HAL::TimerAlarm* handler) {
    inthandler = handler;
}

inline HAL::TimerAlarm* HAL::GetAlarmHandler() {
    return inthandler;
}

inline void HAL::NothingToDo() {
    while(1);
    // must be replaced by something like
    //_wait(); // in the pic32.s

}

#endif	/* HAL_H */

