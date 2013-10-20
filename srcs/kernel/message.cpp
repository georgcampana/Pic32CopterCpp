/*
 * Pic32CopterCpp -- A C++ microOS for the PIC32
 *
 * Copyright (C) 2012 - 2014, Georg Campana
 *
 * Georg Campana <g.campana(AT)stetel.com>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE.txt file
 * at the top of the source tree.
 *
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
void Message::Reply(Int32 result)  {
    if(type==MT_REPLYREQUESTED) {
        payload.replyresult = result;
        status = MS_REPLIED;
        replyport->Post(this);
    }
}
