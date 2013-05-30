/* 
 * File:   kernel.h
 * Author: Georg Campana
 *
 * Created on 28 aprile 2013, 17.32
 */

#ifndef KERNEL_H
#define	KERNEL_H

#include "hal/hal.h"
#include "task.h"


class SysTimer {

    static int const TIMESLICE_QUANTUM = 5; // 5ms

 public:

    SysTimer();
     
    class QueueItem : public Node {
        TaskBase* task2wake;
        SignalPool::SIGNAL sig2send;
        HAL::TICKS ticks2wait;
      public:
        QueueItem(TaskBase* task2queue) : task2wake(task2queue), sig2send(0), ticks2wait(0) {}
        QueueItem(TaskBase* task2queue, SignalPool::SIGNAL wakeupsig) :
                                        task2wake(task2queue), sig2send(wakeupsig), ticks2wait(0) {}

        void SetTicks2Wait(HAL::TICKS targetticks) {ticks2wait = targetticks;}
        HAL::TICKS GetTicks2Wait() const { return ticks2wait; }

        void SignalTask() { if(sig2send) task2wake->Signal(sig2send);};

        bool IsDelayItem() { return(sig2send==0);}
        bool IsTimeSliceItem() { return(task2wake==0); }
    };
     
    static void AddWaitingTask(QueueItem* item2queue, int ms, int us=0);
    static HAL::TICKS GetNowTicks();
    static void Start();
     
private:
    static class Alarm : public HAL::TimerAlarm {
        bool HandleAlarm();
    } alarmhandler;
    
    static QueueItem timeslice;
    static void SetTime2Elapse();

    static List queuedtasks;
};


class Kernel {
 public:

     Kernel();

     static TaskBase* GetRunningTask();
     static void AddTask(TaskBase* newtask);
     static void Reschedule();
     static void QuantumElapsed();

     static void PutOnWait(TaskBase* task2change);
     static void PutOnReady(TaskBase* task2change);

     static bool RunningIsFirstTask();
     static bool InterruptRunning();
     static void SetReschedulePending();
     static void InterruptEpilogue();

     static void StartMainTask(TaskBase* firsttask);

     class InterruptCtrl {
         unsigned int int_status;
       public:
           //InterruptCtrl() : int_status(0) {}
           void Disable() { int_status = HAL::DisableInterrupts(); }
           void Restore() { HAL::RestoreInterrupts(int_status); }
     };

     class SchedulerCtrl {
         unsigned int sched_status;
       public:
           //InterruptCtrl() : int_status(0) {}
           void Disable() { sched_status = HAL::DisableScheduler(); }
           void Restore() { HAL::RestoreScheduler(sched_status); }
     };

private:

     static TaskBase* runningnow;

     static List readytasks;
     static List waitingtasks;

     static bool reschedulepending;
};

inline TaskBase* Kernel::GetRunningTask() { return runningnow; }

inline bool Kernel::RunningIsFirstTask() { return (runningnow == readytasks.GetFirst()); }
inline bool Kernel::InterruptRunning() { return false ;}
inline void Kernel::SetReschedulePending() { reschedulepending = true; }
inline void Kernel::InterruptEpilogue() {if(reschedulepending) Reschedule(); }

#endif	/* KERNEL_H */

