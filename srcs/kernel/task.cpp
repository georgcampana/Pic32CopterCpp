/* 
 * File:   task.cpp
 * Author: gcamp_000
 * 
 * Created on 28 aprile 2013, 17.36
 */

#include "task.h"
#include "kernel.h"

TaskBase::TaskBase(char* newstack): execstack(newstack), tasksignals(), savedstackpointer(0), status(TS_NEW) {


}

// this can be called from any entity = other tasks, interrupts, scheduler
// it might cause a reschedule
void TaskBase::Signal(SignalPool::SIGNAL sig2notify) {
    // stop ints if not already stopped
    Kernel::InterruptCtrl intsafe;
    intsafe.Disable(); // stop ints
    {   // Note: if the task is already ready then CheckWaitings returns 0
        if(tasksignals.CheckWaiting()) {
            // one or more sigs do match and task is waiting for
            // let's put him to the ready tasks
            Kernel::PutOnReady(this);
            // if we have a new "highest priority" Task then we have to reschedule
            if(Kernel::RunningIsFirstTask() == false) {
                // If i'm an interrupt then we defer to the end a reschedule
                if(Kernel::InterruptRunning()) {
                    Kernel::SetReschedulePending();
                }
                else { // i'm a running Task
                    Kernel::Reschedule();
                }
            }


        }

    }
    intsafe.Restore(); // restore ints
}

void TaskBase::OnRun() {

    // Empty OnRun

}

void TaskBase::Delay(int waitms) {
    Kernel::InterruptCtrl intsafe;
    intsafe.Disable(); // stop ints
    {
        SysTimer::AddWaitingTask(this, waitms);
        Kernel::PutOnWait(this);
        Kernel::Reschedule(); // here the task will be stopped -> context switch
    }
    intsafe.Restore(); // restore ints
}

SignalPool::SIGNALMASK TaskBase::Wait(SignalPool::SIGNALMASK sigs2wait, int maxms) {
    // stop ints
    Kernel::InterruptCtrl intsafe;
    intsafe.Disable();

    // check if signals match already, otherise reschedule
    SignalPool::SIGNALMASK resultsigs = tasksignals.CheckAndReset(sigs2wait);

    if(resultsigs == 0) {
        // no matching signals let's add to system timer
        // we register what we are waiting for to get Signal()ed if needed
        tasksignals.SetWaitingSigs(sigs2wait);
        if(maxms > 0) { SysTimer::AddWaitingTask(this, maxms); }

        Kernel::PutOnWait(this);
        Kernel::Reschedule(); // here the task will be stopped -> context switch
        resultsigs = tasksignals.CheckAndReset(sigs2wait);
        tasksignals.SetWaitingSigs(0); // wait is over
    }

    // restore ints
    intsafe.Restore();

    return resultsigs;
}


