/*
 * Pic32CopterCpp -- A C++ microOS for the PIC32
 *
 * Copyright (C) 2012 - 2014, Georg Campana
 *
 * Georg Campana <g.campana(AT)stetel.com>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE.txt file
 * at the top of the source tree.
 *
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

    if(HAL::LockTestAndSet((Int32*)&grantedtask, (Int32) myself ) == 0) {
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
        TaskBase::WaitingTaskItem* waititem = myself->GetWaitItem();
        taskqueue.Append(waititem);

        bool arrivedsig = waititem->Wait(maxwaitms);
        waititem->RemoveFromList();

        if(arrivedsig) { //no timeout
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
        TaskBase::WaitingTaskItem* nextinqueue = (TaskBase::WaitingTaskItem*)taskqueue.GetFirst();
        nextinqueue->Signal(); // can potentially reschedule
    }

    ossafe.Exit();
}



bool NestableSemaphore::ObtainNested(Int32 maxwaitms) {
    TaskBase* myself = Kernel::GetRunningTask();

    if(myself == grantedtask) {
        nestingcounter++ ;
        return true;
    }

    if(Obtain(maxwaitms)) {
        nestingcounter++ ;
        return true;
    }
    return false;
}

bool NestableSemaphore::TryObtainNested() {
    TaskBase* myself = Kernel::GetRunningTask();

    if(myself == grantedtask) {
        nestingcounter++ ;
        return true;
    }

    if(TryObtain()) {
        nestingcounter++ ;
        return true;
    }
    return false;
}

void NestableSemaphore::ReleaseNested() {
    if(--nestingcounter == 0) {
        Release();
    }
}
