/* 
 * File:   task.cpp
 * Author: gcamp_000
 * 
 * Created on 28 aprile 2013, 17.36
 */

#include "task.h"
#include "kernel.h"

TaskBase::TaskBase(char* newstack): execstack(newstack), tasksignals() {


}


void TaskBase::OnRun() {

    // Empty OnRun

}

void TaskBase::Delay(int waitms) {
    SysTimer::AddWaitingTask(this, waitms);
    Kernel::PutOnWait(this);
    Kernel::Reschedule(); // here the task will be stopped -> context switch
}

SignalPool::SIGNALMASK TaskBase::Wait(SignalPool::SIGNALMASK sigs2wait, int maxms) {
    // stop ints
    // check if signals match already, otherise reschedule
    SignalPool::SIGNALMASK resultsigs = tasksignals.CheckAndReset(sigs2wait);

    if(resultsigs == 0) {
        // no matching signals let's add to system timer
        if(maxms > 0) { 
            Delay(maxms);
        }
        else {
            Kernel::PutOnWait(this);
            Kernel::Reschedule();
        }
        resultsigs = tasksignals.CheckAndReset(sigs2wait);
    }

    // restore ints

    return resultsigs;
}


