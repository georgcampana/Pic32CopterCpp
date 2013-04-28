/* 
 * File:   semaphore.h
 * Author: gcamp_000
 *
 * Created on 28 aprile 2013, 18.53
 */

#ifndef SEMAPHORE_H
#define	SEMAPHORE_H

#include "task.h"

class Semaphore : Node {
public:

    bool Obtain(int maxwaitms = -1);
    void Release();
private:

    class WaitingTask : public Node {
     public:
        WaitingTask(TaskBase* task2queue, SignalPool::SIGNAL sig2use);
        TaskBase* taskinqueue;
        SignalPool::SIGNAL tasksignal;
    };

    TaskBase* grantedtask;
    List taskqueue;
};

inline Semaphore::WaitingTask::WaitingTask(TaskBase* task2queue, SignalPool::SIGNAL sig2use) : taskinqueue(task2queue), tasksignal(sig2use) {}

#endif	/* SEMAPHORE_H */

