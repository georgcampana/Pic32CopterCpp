/* 
 * File:   message.cpp
 * Author: Georg Campana
 * 
 * Created on 28 aprile 2013, 16.55
 */

#include "message.h"
#include "msgport.h"


// if the message requires a reply then this replies it (otherwise it has no effect)
// result = 0 (default value) should mean OK
// TODO: evaluate if it would make sense to define an enum for results
void Message::Reply(int result)  {
    if(type==MT_REPLYREQUESTED) {
        payload.replyresult = result;
        status = MS_REPLIED;
        replyport->Post(this);
    }
}
