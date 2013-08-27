/* 
 * File:   semaphore.cpp
 * Author: gcamp_000
 * 
 * Created on 28 aprile 2013, 18.53
 */

#include "semaphore.h"
#include "kernel.h"

// could reschedule the Task
bool Semaphore::Obtain(int maxwaitms) {

    Kernel::SchedulerCtrl ossafe;
    ossafe.EnterProtected();

    
    TaskBase* myself = Kernel::GetRunningTask();
    // if sempahore is free grant immediately
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

// could reschedule the Task
void Semaphore::Release() {

}