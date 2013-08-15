/* 
 * File:   kernel.cpp
 * Author: Georg Campana
 * 
 * Created on 28 aprile 2013, 17.32
 */

#include "kernel.h"

// ******************* SysTimer *******************
SysTimer::QueueItem SysTimer::timeslice(0);
List SysTimer::queuedtasks;
SysTimer::Alarm SysTimer::alarmhandler;

SysTimer::SysTimer() {

}


void SysTimer::AddWaitingTask(QueueItem* item2queue, int ms, int us) {

    HAL::TICKS targetticks = HAL::GetCurrentTicks() +
                             HAL::ConvertTime2Ticks(ms, us);
    item2queue->SetTicks2Wait(targetticks);

    QueueItem* cursor = (QueueItem*)queuedtasks.GetFirst();
    while(queuedtasks.IsTail(cursor) == false) {
        if(cursor->GetTicks2Wait() > targetticks) { break; }

        cursor = (QueueItem*)cursor->GetNext();
    }
    cursor->AddInFront(item2queue);

    if(queuedtasks.GetFirst() == item2queue) { // the new item is the first one
        HAL::SetNextAlarm(targetticks);
    }
}

void SysTimer::Start() {
    HAL::SetAlarmHandler(&alarmhandler); // this is wher we will get the alarms

    AddWaitingTask(&timeslice,TIMESLICE_QUANTUM); // this appends the first alarm
}

bool SysTimer::Alarm::HandleAlarm() {
    while(!queuedtasks.IsEmpty()) {
        HAL::TICKS now = HAL::GetCurrentTicks();
        QueueItem* first = (QueueItem*)queuedtasks.GetFirst();
        HAL::TICKS waitingfor = first->GetTicks2Wait();

        if(now > waitingfor) { // time has passed we remove it and signal the task accordingly
            first->RemoveFromList();
            if(first->IsTimeSliceItem()) {
                // normal elapsed timeslice. we reschedule the task
                AddWaitingTask(&timeslice,TIMESLICE_QUANTUM); // this re-appends the timeslice alarm
                Kernel::QuantumElapsed();
            }
            else { // a task that is waiting for a timed signal
                first->SignalTask();
            }
        }
        else {
            HAL::SetNextAlarm(waitingfor);
            break;
        }
    }
    return true;
}

// *********************KERNEL **********************


TaskBase* Kernel::runningnow = 0;

List Kernel::readytasks;
List Kernel::waitingtasks;

bool Kernel::reschedulepending = false;
Kernel::Epilogue Kernel::reschedulehandler;

// no one calls this :( since it is just an abstract container for static stuff
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
        // The task does not run anymore: time to kill
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

    while(readytasks.IsEmpty()) {
        // put cpu on wait for the next interrupt
        HAL::NothingToDo();
    }

    // take the first ready and execute it
    TaskBase* newtask  = (TaskBase*)readytasks.GetFirst();
    TaskBase* currtask = runningnow;

    if(newtask != currtask) {
        runningnow = newtask;
        // defined in HAL
        if(InterruptRunning()) {
            // Last action of the interrupt (NOTE: the old task is already archived on its stack)
            // epilogue will restore the new "runningnow"
        }
        else {
            swapTaskContext(&currtask->savedstackpointer, newtask->savedstackpointer);
        }
    }
    // we exit being potentially another Task
}

// should be called by a service interrupt only (SysTimer)
void Kernel::QuantumElapsed() {
    runningnow->RemoveFromList();
    readytasks.Enqueue(runningnow);
    SetReschedulePending();
}

// called by the Hal Interrupt epilogue to see if a reschedule is needed
// returns the pointer of the saved sp for the task t activate/execute
char* Kernel::Epilogue::RescheduleIfNeeded(char* lastsp) {

    if(reschedulepending) {
        Reschedule();
        return getCurrentSavedSP();
    }
    return lastsp;
}

// this never returns and starts the main task
void Kernel::InitAndStartMainTask(TaskBase* firsttask) {

    HAL::SetRescheduleHandler(&reschedulehandler);

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

