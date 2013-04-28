/* 
 * File:   kernel.h
 * Author: gcamp_000
 *
 * Created on 28 aprile 2013, 17.32
 */

#ifndef KERNEL_H
#define	KERNEL_H

#include "task.h"

class Kernel {
 public:

     Kernel();

     static TaskBase* GetRunningTask();
     static void AddTask(TaskBase* newtask);
 private:
     static TaskBase* runningnow;

     static List readytasks;
     static List waitingtasks;
};


inline TaskBase* Kernel::GetRunningTask() { return runningnow; }

inline void Kernel::AddTask(TaskBase* newtask) {
    readytasks.Enqueue(newtask);
}

#endif	/* KERNEL_H */

