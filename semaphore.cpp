/* 
 * File:   semaphore.cpp
 * Author: gcamp_000
 * 
 * Created on 28 aprile 2013, 18.53
 */

#include "include/semaphore.h"
#include "include/kernel.h"

bool Semaphore::Obtain(int maxwaitms) {
    // stop ints
    
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

    // restore ints

    return (grantedtask == Kernel::GetRunningTask());
}

void Semaphore::Release() {

}