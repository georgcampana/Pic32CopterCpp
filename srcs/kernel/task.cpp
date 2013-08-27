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
    Kernel::SchedulerCtrl ossafe;
    ossafe.EnterProtected(); // stop ints
    {   // we set the signal
        tasksignals.Set(sig2notify);
        // Note: if the task is already ready (got another signal) then CheckWaitings returns 0
        if( status != TS_DONE &&
            tasksignals.CheckWaiting()) {
            // one or more sigs do match and task is waiting for
            // let's put him to the ready tasks
            if(status == TS_WAITING) {
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

    }
    ossafe.Exit(); // restore int status
}

void TaskBase::OnRun() {

    // Empty OnRun

}

void TaskBase::Delay(int waitms) {
    Wait(0,waitms);
}

SignalPool::SIGNALMASK TaskBase::Wait(SignalPool::SIGNALMASK sigs2wait, int maxms) {
    // stop ints
    Kernel::SchedulerCtrl ossafe;
    ossafe.EnterProtected();

    // check if signals match already, otherise reschedule
    SignalPool::SIGNALMASK resultsigs = tasksignals.CheckAndReset(sigs2wait);

    if(resultsigs == 0) {
        // no matching signals let's add to system timer
        // we register what we are waiting for to get Signal()ed if needed
        tasksignals.SetWaitingSigs(sigs2wait);

        SysTimer::AlarmItem waititem(this); // this must stay in the Reschedule scope
        if(maxms > 0) {
            tasksignals.SetWaitingSigs(sigs2wait | SignalPool::SYSTIMER_SIG);
            SysTimer::AddAlarm(&waititem, maxms);
        }

        Kernel::PutOnWait(this);

        Kernel::Reschedule(); // here the task will be stopped -> context switch
        // no need to remove the timerqueueitem (get removed by the AlarmHandler)
        // unless it was another signal (see below)
        if(maxms > 0) {
            if(tasksignals.CheckAndReset(SignalPool::SYSTIMER_SIG) == 0) {
                // we run because of an arrived signal not because of the expired timeout
                // so we have to remove the pending alarm queueitem
                SysTimer::CancelAlarm(&waititem);
            }
        }
        
        resultsigs = tasksignals.CheckAndReset(sigs2wait);
        tasksignals.SetWaitingSigs(0); // wait is over

    }

    // restore ints
    ossafe.Exit();

    return resultsigs;
}


