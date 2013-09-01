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
    
    TaskBase(char* taskstack, Int32 stackdimension);

    void Signal(SignalPool::SIGNAL sig2notify);

    virtual void OnRun();
 protected:

    friend class Semaphore;
    friend class MsgPort;
    friend class Kernel;

    SignalPool::SIGNALMASK Wait(SignalPool::SIGNALMASK sigs2wait, Int32 maxms=-1 );
    void Delay(Int32 waitms);

    char* execstack;
    SignalPool tasksignals;
    char* savedstackpointer;
    Int32 stacksize;

    enum TaskStatus{
     TS_NONE = 0,
     TS_NEW, // must be started
     TS_READY,
     TS_WAITING,
     TS_DONE
    } status;
    
    enum TaskPri {
        TSPRI_XHIGH = 10,
        TSPRI_HIGH = 5,
        TSPRI_NORMAL = 0,
        TSPRI_LOW = -5,
        TSPRI_XLOW = -10,
        
        TSPRI_IDLE = -127, // pri is an int but i prefer to fit in a char
    };

};



template <Int32 S>
class Task : public TaskBase {
 public:
     Task();

 private:
     char stack[S];

};

template <Int32 S>
inline Task<S>::Task() : TaskBase(stack, S) {}

#endif	/* TASK_H */

