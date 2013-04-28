/* 
 * File:   task.cpp
 * Author: gcamp_000
 * 
 * Created on 28 aprile 2013, 17.36
 */

#include "include/task.h"
#include "include/kernel.h"

TaskBase::TaskBase(char* newstack): execstack(newstack), tasksignals() {


}


void TaskBase::OnRun() {

    // Empty OnRun

}


SignalPool::SIGNALMASK TaskBase::Wait(SignalPool::SIGNALMASK sigs2wait, int maxms) {
    // stop ints
    // check if signals match already, otherise reschedule
    TaskBase* myself = Kernel::GetRunningTask();

    // restore ints
}