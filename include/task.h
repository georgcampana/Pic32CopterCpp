/* 
 * File:   task.h
 * Author: gcamp_000
 *
 * Created on 28 aprile 2013, 17.36
 */

#ifndef TASK_H
#define	TASK_H

#include "msgport.h"


class TaskBase : public MsgPort {
 public:
    
    TaskBase(char* taskstack);

    virtual void OnRun();
 protected:

    friend class Semaphore;
    friend class Kernel;

    SignalPool::SIGNALMASK Wait(SignalPool::SIGNALMASK sigs2wait, int maxms=-1 );
    void Delay(int waitms);

    char* execstack;
    SignalPool tasksignals;

    enum TaskStatus{
     TS_NONE = 0,
     TS_NEW, // must be started
     TS_READY,
     TS_WAITING,
     TS_DONE
    } status;
};



template <int S>
class Task : public TaskBase {
 public:
     Task();

 private:
     char stack[S];

};

template <int S>
inline Task<S>::Task() : TaskBase(stack) {}

#endif	/* TASK_H */

