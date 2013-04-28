/* 
 * File:   kernel.h
 * Author: gcamp_000
 *
 * Created on 28 aprile 2013, 17.32
 */

#ifndef KERNEL_H
#define	KERNEL_H

#include "task.h"

class SysTimer {
 public:
     static void AddWaitingTask(TaskBase* task2queue, int ms, int us=0);
     static int GetNow();
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
private:

     static TaskBase* runningnow;

     static List readytasks;
     static List waitingtasks;
};

inline TaskBase* Kernel::GetRunningTask() { return runningnow; }



#endif	/* KERNEL_H */

