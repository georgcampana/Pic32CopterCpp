/* 
 * File:   task.cpp
 * Author: gcamp_000
 * 
 * Created on 28 aprile 2013, 17.36
 */

#include "task.h"
#include "kernel.h"

TaskBase::TaskBase(char* newstack, int stackdimension): execstack(newstack), tasksignals(), savedstackpointer(newstack), stacksize(stackdimension), status(TS_NEW) {


}

// this can be called from any entity = other tasks, interrupts, scheduler
// it might cause a reschedule
void TaskBase::Signal(SignalPool::SIGNAL sig2notify) {
    // stop ints if not already stopped
    Kernel::InterruptCtrl intsafe;
    intsafe.Disable(); // stop ints
    {   // we set the signal
        tasksignals.Set(sig2notify);
        // Note: if the task is already ready then CheckWaitings returns 0
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
    intsafe.Restore(); // restore int status
}

void TaskBase::OnRun() {

    // Empty OnRun

}

void TaskBase::Delay(int waitms) {
    Wait(0,waitms);
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

        SysTimer::QueueItem waititem(this); // this must stay in the Reschedule scope
        if(maxms > 0) {
            SysTimer::AddWaitingTask(&waititem, maxms);
        }

        Kernel::PutOnWait(this);
        Kernel::SetReschedulePending();

        Kernel::Reschedule(); // here the task will be stopped -> context switch
        // we need to remove the timerqueueitem from the SysTimer
        if(maxms > 0) {
            //SysTimer::RemWaitingTask(&waititem);
        }
        resultsigs = tasksignals.CheckAndReset(sigs2wait);
        tasksignals.SetWaitingSigs(0); // wait is over
    }

    // restore ints
    intsafe.Restore();

    return resultsigs;
}


