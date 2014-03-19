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
 * File:   kernel.h
 * Author: Georg Campana
 *
 * Created on 28 aprile 2013, 17.32
 */

#ifndef KERNEL_H
#define	KERNEL_H

#include "hal/hal.h"
#include "task.h"

#ifdef DEBUG
# include"../oslib/outstream.h"
#endif

class SysTimer {

    static Int32 const TIMESLICE_QUANTUM = 5; // ms

 public:

    SysTimer();
     
    class AlarmItem : public Node {
        TaskBase* task2wake;
        SignalPool::SIGNAL sig2send;
        HAL::TICKS ticks2wait;
      public:
        AlarmItem(TaskBase* task2queue) : task2wake(task2queue), sig2send(SignalPool::SYSTIMER_SIG), ticks2wait(0) {}
        AlarmItem(TaskBase* task2queue, SignalPool::SIGNAL wakeupsig) :
                                        task2wake(task2queue), sig2send(wakeupsig), ticks2wait(0) {}

        void SetTicks2Wait(HAL::TICKS targetticks) {ticks2wait = targetticks;}
        HAL::TICKS GetTicks2Wait() const { return ticks2wait; }

        void SignalTask() { if(sig2send) task2wake->Signal(sig2send);};

        bool IsTimeSliceAlarm() const { return(task2wake==0); }
    };
     
    static void AddAlarm(AlarmItem* item2queue, Int32 ms, Int32 us=0);
    static void CancelAlarm(AlarmItem* item2cancel);
    static UInt32 GetNowMillisecs(UInt32* now_us=NULL);
    static void Start();

#ifdef DEBUG
    static void dbg_Dump();
#endif
     
private:
    static class Alarm : public HAL::TimerAlarm {
      public:
        bool HandleAlarm();
    } alarmhandler;
    
    static AlarmItem timeslice;
    static void SetTime2Elapse();

    static List queuedalarms;
};


class Kernel {
 public:

     Kernel();

     static TaskBase* GetRunningTask();
     static void AddTask(TaskBase* newtask);
     static void ExecuteNewTask(TaskBase* newtask);

     static void Reschedule();
     static void QuantumElapsed();

     static void PutOnWait(TaskBase* task2change);
     static void PutOnReady(TaskBase* task2change);

     static bool RunningIsFirstTask();
     static bool InterruptRunning();
     static void SetReschedulePending();
     static void InterruptEpilogue();
     static void RecheduleIfPending();

     static void DelayCurrentTask(Int32 waitms);

     static void InitAndStartMainTask(TaskBase* firsttask);
     
     static class Epilogue : public HAL::IntEpilogue {
      public:
        char* RescheduleIfNeeded(Char* lastsp);
     } reschedulehandler;

     class InterruptCtrl {
         UInt32 int_status;
       public:
           //InterruptCtrl() : int_status(0) {}
           void Disable() { int_status = HAL::DisableInterrupts(); }
           void Restore() { HAL::RestoreInterrupts(int_status); }
     };

     class SchedulerCtrl {
         UInt32 sched_status;
       public:
           //SchedulerCtrl() : sched_status(0) {}
           void EnterProtected() { sched_status = HAL::DisableScheduler(); }
           void Exit() { HAL::RestoreScheduler(sched_status); }
     };

#ifdef DEBUG
    static void dbg_DumpStatus(OutStream& dbo);
    static void dbg_DumpTaskList(OutStream& dbo, List& list2dump);
#endif

private:
     static Char* getCurrentSavedSP();
     static void setCurrentSavedSP(Char* stackpointer);

     static TaskBase* runningnow;

     static List readytasks;
     static List waitingtasks;

     static bool reschedulepending;

};

inline TaskBase* Kernel::GetRunningTask() { return runningnow; }

inline bool Kernel::RunningIsFirstTask() { return (runningnow == readytasks.GetFirst()); }
inline bool Kernel::InterruptRunning() { return HAL::InterruptRunning() ;}
inline void Kernel::SetReschedulePending() { reschedulepending = true; }
inline void Kernel::InterruptEpilogue() {if(reschedulepending) Reschedule(); }
inline void Kernel::RecheduleIfPending() {if(reschedulepending) Reschedule(); }

inline void Kernel::DelayCurrentTask(Int32 waitms) {runningnow->Delay(waitms) ; }


inline Char* Kernel::getCurrentSavedSP() { return runningnow->savedstackpointer; }
inline void Kernel::setCurrentSavedSP(Char* stackpointer) {  runningnow->savedstackpointer = stackpointer; }

#endif	/* KERNEL_H */

