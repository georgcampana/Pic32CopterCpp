/* 
 * File:   kernel.cpp
 * Author: Georg Campana
 * 
 * Created on 28 aprile 2013, 17.32
 */

#include "kernel.h"

// ******************* SysTimer *******************
SysTimer::AlarmItem SysTimer::timeslice(0);
List SysTimer::queuedalarms;
SysTimer::Alarm SysTimer::alarmhandler;

SysTimer::SysTimer() {

}

// NOTE: there is no arbitration on the list
void SysTimer::AddAlarm(AlarmItem* item2queue, int ms, int us) {

    HAL::TICKS targetticks = HAL::GetCurrentTicks() +
                             HAL::ConvertTime2Ticks(ms, us);
    item2queue->SetTicks2Wait(targetticks);

    AlarmItem* cursor = (AlarmItem*)queuedalarms.GetFirst();
    while(queuedalarms.IsTail(cursor) == false) {
        if(cursor->GetTicks2Wait() > targetticks) { break; }

        cursor = (AlarmItem*)cursor->GetNext();
    }
    cursor->AddInFront(item2queue);

    if(queuedalarms.GetFirst() == item2queue) { // the new item is the first one
        HAL::SetNextAlarm(targetticks);
    }
}

// NOTE: there is no arbitration on the list
void SysTimer::CancelAlarm(AlarmItem* item2cancel) {
    if(queuedalarms.GetFirst() == item2cancel) { // the item is the first one
        AlarmItem* nextalarm = (AlarmItem*)item2cancel->GetNext();
        if(queuedalarms.IsTail(nextalarm)) {
            HAL::SetNextAlarm(0); // no alarm item to set: strange
        }
        else {
            HAL::SetNextAlarm(nextalarm->GetTicks2Wait()); //
        }
    }
    
    item2cancel->RemoveFromList();
}

void SysTimer::Start() {
    HAL::SetAlarmHandler(&alarmhandler); // this is where we will get the alarms

    AddAlarm(&timeslice,TIMESLICE_QUANTUM); // this appends the first alarm
}

bool SysTimer::Alarm::HandleAlarm() {
    while(!queuedalarms.IsEmpty()) {
        HAL::TICKS now = HAL::GetCurrentTicks();
        AlarmItem* first = (AlarmItem*)queuedalarms.GetFirst();
        HAL::TICKS waitingfor = first->GetTicks2Wait();

        if(now > waitingfor) { // time has passed we remove it and signal the task accordingly
            first->RemoveFromList();
            if(first->IsTimeSliceAlarm()) {
                // normal elapsed timeslice. we reschedule the queueitem
                AddAlarm(&timeslice,TIMESLICE_QUANTUM); // this re-appends the timeslice alarm
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

#ifdef	__cplusplus
extern "C" {
#endif
void exe_forkedtask(void* newtask) {
    Kernel::ExecuteNewTask((TaskBase*)newtask);
};
#ifdef	__cplusplus
}
#endif


// no one calls this :( since it is just an abstract container for static stuff
Kernel::Kernel() {

}

void Kernel::AddTask(TaskBase* newtask) {

    newtask->status = TaskBase::TS_READY; // was NEW
    readytasks.Enqueue(newtask);
    

    // let's fork here we get the new forkedtask returned
    forkTask((void*)newtask, &newtask->savedstackpointer, newtask->stacksize, &exe_forkedtask);

}

void Kernel::ExecuteNewTask(TaskBase* forkedtask) {

    forkedtask->OnRun();

    // The task does not run anymore: time to kill
    Kernel::InterruptCtrl intsafe;
    intsafe.Disable(); // stop ints
    {
        forkedtask->RemoveFromList();
        forkedtask->status = TaskBase::TS_DONE;
        Kernel::Reschedule();
    }
    intsafe.Restore(); // will never be executed--> reschedule jumps to another task
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
        reschedulepending = false;
        setCurrentSavedSP(lastsp);
        Reschedule();
        return getCurrentSavedSP();
    }
    return lastsp;
}

//TODO: check how much stack we really need for the idle task
class SysIdleTask : public Task<256> {

public:
    SysIdleTask() { priority = Task::TSPRI_IDLE;}
    void OnRun() {
        HAL::NothingToDo();
    }

} idletask;


// this never returns and starts the main task
void Kernel::InitAndStartMainTask(TaskBase* firsttask) {

    HAL::SetRescheduleHandler(&reschedulehandler);

    firsttask->status = TaskBase::TS_READY;
    readytasks.AddAsFirst(firsttask);
    runningnow = firsttask;
    transferMainStack(&firsttask->savedstackpointer, firsttask->stacksize);

    AddTask(&idletask);
    firsttask->OnRun();

    // the parent should never die: never exit from OnRun
    // in case it should happen this is a fair fallback
    firsttask->RemoveFromList();

    Reschedule(); // we stay with the idle task
}

