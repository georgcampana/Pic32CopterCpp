/* 
 * File:   msgport.cpp
 * Author: gcamp_000
 * 
 * Created on 28 aprile 2013, 16.50
 */

#include "msgport.h"
#include "task.h"
#include "kernel.h"


Message* MsgPort::GetMsg(Int32 waitms) {

    Message* msg2return = 0;
    Kernel::SchedulerCtrl safecode;

    safecode.EnterProtected();
    {
        if(messages.IsEmpty()) {
            Kernel::GetRunningTask()->Wait(tasksignal,waitms);
        }
        // TODO: to understand if it would be better to check the signal instead of empty
        if(!messages.IsEmpty()) {
            msg2return = (Message*)messages.GetFirst();
            msg2return->RemoveFromList();
            msg2return->status =
                 (msg2return->status == Message::MS_POSTED)? Message::MS_RETRIEVED : Message::MS_REPLYRETRIEVED;
        }
    }
    safecode.Exit();

    return msg2return;
}

// Note this is normally called by other tasks.
// TODO: maybe the append could become an Enqueue
void MsgPort::Post(Message* newmsg) {

    Kernel::SchedulerCtrl safecode;

    safecode.EnterProtected();
    {
        messages.Append(newmsg);
        if(newmsg->status != Message::MS_REPLIED) {
            newmsg->status = Message::MS_POSTED;
        }
        task2notify->Signal(tasksignal);
    }
    safecode.Exit();
}

