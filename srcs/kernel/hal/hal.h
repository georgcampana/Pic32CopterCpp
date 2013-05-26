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
    static unsigned int DisableInterrupts();
    static void RestoreInterrupts(unsigned int oldstatus);

    static unsigned int DisableScheduler();
    static void RestoreScheduler(unsigned int oldstatus);

    static void swapTaskContext(char** from_context_sp, char* to_context_sp);
    static bool forkTask(void** ptaskpointer,void* taskpointer, char** stackpointer, int stacksize);
    static void transferMainStack(char** context_sp, int stacksize);

private:

};


inline unsigned int HAL::DisableInterrupts() { return 0;}
inline void HAL::RestoreInterrupts(unsigned int oldstatus) {}

inline unsigned int HAL::DisableScheduler() { return DisableInterrupts();}
inline void HAL::RestoreScheduler(unsigned int oldstatus) {RestoreInterrupts(oldstatus);}

inline void HAL::swapTaskContext(char** from_context_sp, char* to_context_sp) {
    swapTaskContext(from_context_sp,to_context_sp);
}
inline bool HAL::forkTask(void** ptaskpointer,void* taskpointer, char** stackpointer, int stacksize) {
    return forkTask(ptaskpointer,taskpointer,stackpointer,stacksize);
}
inline void HAL::transferMainStack(char** context_sp, int stacksize) {
    transferMainStack(context_sp,stacksize);
}

#endif	/* HAL_H */

