/* 
 * File:   kernel.h
 * Author: gcamp_000
 *
 * Created on 28 aprile 2013, 17.32
 */

#ifndef KERNEL_H
#define	KERNEL_H

#include "hal/hal.h"
#include "task.h"


class SysTimer {
 public:
     static void AddWaitingTask(TaskBase* task2queue, int ms, int us=0);
     static int GetNowTicks();
 private:
 
     class QueueItem {
      public:
          QueueItem(TaskBase* task2queue) : task2wake(task2queue) {}
          TaskBase* task2wake;
     };
     
     List queuedtasks;
   
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


     class InterruptCtrl {
         unsigned int int_status;
       public:
           //InterruptCtrl() : int_status(0) {}
           void Disable() { int_status = HAL::DisableInterrupts(); }
           void Restore() { HAL::RestoreInterrupts(int_status); }
     };
private:

     static TaskBase* runningnow;

     static List readytasks;
     static List waitingtasks;
};

inline TaskBase* Kernel::GetRunningTask() { return runningnow; }



#endif	/* KERNEL_H */

