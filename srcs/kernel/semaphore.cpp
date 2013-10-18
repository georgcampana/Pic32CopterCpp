/* 
 * File:   semaphore.cpp
 * Author: gcamp_000
 * 
 * Created on 28 aprile 2013, 18.53
 */

#include "semaphore.h"
#include "kernel.h"

// could reschedule the Task
bool Semaphore::Obtain(Int32 maxwaitms) {

    TaskBase* myself = Kernel::GetRunningTask();

    // Quick attempt: works in case we get the semaphore
    // otherwise we go for the heavy way (disable and reenable)

    if(HAL::LockTestAndSet((Int32*)grantedtask, (Int32) myself ) == 0) {
        return true;
    }

    // was tken already we attempt again in the sad way (by locking scheduling)
    Kernel::SchedulerCtrl ossafe;
    ossafe.EnterProtected();

    // if sempahore is now free grant immediately (could have been freed between lock an EnterProtected)
    if(grantedtask == 0) {
        grantedtask = myself;
    }
    else {
        SignalPool::SIGNAL semsig = myself->tasksignals.Alloc();
        WaitingTask thistask(myself, semsig);
        taskqueue.Append(&thistask);

        SignalPool::SIGNALMASK arrivedsig =
                myself->Wait(semsig, maxwaitms); // scheduler might reschedule

        myself->tasksignals.Free(semsig);
        thistask.RemoveFromList();

        if(arrivedsig != 0) { //no timeout
            grantedtask = myself;
        }
    }

    ossafe.Exit();

    return (grantedtask == Kernel::GetRunningTask());
}

bool Semaphore::TryObtain() {

    TaskBase* myself = Kernel::GetRunningTask();

    // Quick attempt: works in case we get the semaphore
    // otherwise we go for the heavy way (disable and reenable)

    if(HAL::LockTestAndSet((Int32*)grantedtask, (Int32) myself ) == 0) {
        return true;
    }

    Kernel::SchedulerCtrl ossafe;
    ossafe.EnterProtected();

    // if sempahore is free grant immediately
    if(grantedtask == 0) {
        grantedtask = myself;
    }

    ossafe.Exit();

    return (grantedtask == Kernel::GetRunningTask());
}

// could reschedule the Task
void Semaphore::Release() {
    Kernel::SchedulerCtrl ossafe;
    ossafe.EnterProtected();

    grantedtask = 0;
    if(!taskqueue.IsEmpty()) {
        WaitingTask* nextinqueue = (WaitingTask*)taskqueue.GetFirst();
        nextinqueue->SignalAvailability(); // can potentially reschedule
    }

    ossafe.Exit();
}