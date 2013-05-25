/* 
 * File:   kernel.cpp
 * Author: Georg Campana
 * 
 * Created on 28 aprile 2013, 17.32
 */

#include "kernel.h"

void SysTimer::AddWaitingTask(TaskBase* task2queue, int ms, int us) {

}

TaskBase* Kernel::runningnow = 0;

List Kernel::readytasks;
List Kernel::waitingtasks;

bool Kernel::reschedulepending = false;

Kernel::Kernel() {

}

void Kernel::AddTask(TaskBase* newtask) {

    newtask->status = TaskBase::TS_NEW;
    readytasks.Enqueue(newtask);
    
    TaskBase* forkedtask = newtask;
    // let's fork here
    if(forkTask((void**)&forkedtask, (void*)forkedtask, &forkedtask->savedstackpointer, forkedtask->stacksize ) == true) {
        // this is the new added task running
        // Note: the original "forkedtask" has been changed to the new one by forkTask
        forkedtask->OnRun();
        // The task does nor run anymore: time to kill
        forkedtask->RemoveFromList();
        Reschedule();
    }

    return; // creator exits
}

void Kernel::PutOnWait(TaskBase* task2change) {
    // remove from current list (ready)
    task2change->RemoveFromList();
    // add to waiting list
    task2change->status = TaskBase::TS_WAITING;
    waitingtasks.Enqueue(task2change);
}

void Kernel::PutOnReady(TaskBase* task2change) {
    // remove from current list (waiting)
    task2change->RemoveFromList();
    // add to ready list
    task2change->status = TaskBase::TS_READY;
    readytasks.Enqueue(task2change);
}

void Kernel::Reschedule() {
    // take the first ready and execute it
    TaskBase* newtask  = (TaskBase*)readytasks.GetFirst();
    TaskBase* currtask = runningnow;

    if(newtask != currtask) {
        runningnow = newtask;
        // defined in HAL
        if(InterruptRunning()) {
            // Last action of the interrupt
            // restoreTaskContext()
        }
        else {
            swapTaskContext(&currtask->savedstackpointer, newtask->savedstackpointer);
        }
    }
    // we exit being another Task
}

// should be called by a service interrupt only (SysTimer)
void Kernel::QuantumElapsed() {
    runningnow->RemoveFromList();
    readytasks.Enqueue(runningnow);
    Reschedule();
}


// this never returns and starts the main task
void Kernel::startMainTask(TaskBase* firsttask) {
    firsttask->status = TaskBase::TS_NEW;
    readytasks.AddAsFirst(firsttask);
    runningnow = firsttask;
    transferMainStack(&firsttask->savedstackpointer, firsttask->stacksize);

    firsttask->OnRun();

    // the parent should never die: never exit from OnRun
    // in case it should happen this is a fair fallback
    firsttask->RemoveFromList();
    if(readytasks.IsEmpty() && waitingtasks.IsEmpty()) {
        while(1);
    }
    Reschedule();
}

