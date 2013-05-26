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

#ifdef	__cplusplus
}
#endif

class HAL {
public:
    typedef long long TICKS; // 64 bit counter for the ticks (should last for 7000 years)

    static unsigned int DisableInterrupts();
    static void RestoreInterrupts(unsigned int oldstatus);

    static unsigned int DisableScheduler();
    static void RestoreScheduler(unsigned int oldstatus);

// commented out because kernel MUST call the asm call directly to avoid another subroutine level
//    static void SwapTaskContext(char** from_context_sp, char* to_context_sp);
//    static bool ForkTask(void** ptaskpointer,void* taskpointer, char** stackpointer, int stacksize);
//    static void TransferMainStack(char** context_sp, int stacksize);

    static TICKS GetCurrentTicks();
    static void SetNextAlarm(TICKS almticks);
    static void ResetTickTimer();
    static TICKS ConvertTime2Ticks(int ms, int us=0);
    
private:

};


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
    // TODO: composite with higher long long
    return ReadCoreTimer();
}
inline void HAL::SetNextAlarm(HAL::TICKS almticks) {
   // UpdateCoreTimer
}
inline void HAL::ResetTickTimer() {

}
inline HAL::TICKS HAL::ConvertTime2Ticks(int ms, int us) {

}

#endif	/* HAL_H */

