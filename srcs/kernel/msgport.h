/* 
 * File:   msgport.h
 * Author: gcamp_000
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
     Message* GetMsg(int waitms=-1);
     void Post(Message* newmsg);

 private:
     List messages;
     TaskBase* task2notify;
     SignalPool::SIGNAL tasksignal;
};

inline MsgPort::MsgPort() : Node(NT_MSGPORT), task2notify(0), tasksignal(0) {

}


#endif	/* MSGPORT_H */

