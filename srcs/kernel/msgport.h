/* 
 * File:   msgport.h
 * Author: Georg Campana
 *
 * Created on 28 aprile 2013, 16.50
 */

#ifndef MSGPORT_H
#define	MSGPORT_H

#include "message.h"
#include "list.h"
#include "signal.h"

class TaskBase;

class MsgPort : public Node {
 public:

     MsgPort();
     MsgPort(TaskBase* receivertask, SignalPool::SIGNAL sig2use);

     Message* GetMsg(int waitms = -1);
     void Post(Message* newmsg);

     void SetTargetTask(TaskBase* receivertask, SignalPool::SIGNAL sig2use);
 private:
     List messages;
     TaskBase* task2notify;
     SignalPool::SIGNAL tasksignal;
};

inline MsgPort::MsgPort() : Node(NT_MSGPORT), task2notify(0), tasksignal(0) {

}

inline MsgPort::MsgPort(TaskBase* receivertask, SignalPool::SIGNAL sig2use) :
                            Node(NT_MSGPORT), task2notify(receivertask), tasksignal(sig2use) {

}

inline void MsgPort::SetTargetTask(TaskBase* receivertask, SignalPool::SIGNAL sig2use) {
    task2notify = receivertask;
    tasksignal = sig2use;
}


#endif	/* MSGPORT_H */

