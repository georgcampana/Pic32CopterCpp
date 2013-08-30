/*
 * File:   maintask.c
 * Author: Georg
 *
 * Created on 28 May 2013, 23.31
 */


#include "maintask.h"
#include "kernel/kernel.h"

#include "driver/digitaliomanager.h"
#include "kernel/semaphore.h"
#include "kernel/message.h"


MainTask parenttask;

// led present on the Pinguino micro
OutputPin testled(IOPORT_D, BIT_1);


class ProtectedResource : public Semaphore {
    int counter;
public:
    ProtectedResource() {};


} testsemaphore;


class OutRunningTask : public Task<2048> {

public:
    OutRunningTask() {
        priority = TSPRI_NORMAL;
    }
    void OnRun() {
        Delay(2000);

    }

} outrunning;

class BlinkerTask : public Task<2048> {

    int fakecounter;

public:
    BlinkerTask() : fakecounter(0) {
        priority = TSPRI_NORMAL;
    }

    void OnRun()  {
        while(1) {
            if(Message* received = GetMsg()) {
                int data = received->GetIntPayload();
                data++;
                if(received->IsReplyRequested()) {
                    received->Reply();
                }

            }
        }
    }

} blinker2;

class BlinkerTask3 : public Task<2048> {
    
    Message testmsg;

public:



    BlinkerTask3() :  testmsg(0xee,this) {

    }



    void OnRun() {
        while(1) {
            testled << false;
            Delay(1200);
            blinker2.Post(&testmsg);

            // wait for reply
            Message* reply = GetMsg();
            if(reply->GetReplyResult() != 0) {
                // it's not ok
                // let's panic
                Delay(120000);
            }
        }
    }

} blinker3;


void MainTask::OnRun() {

    Kernel::AddTask(&outrunning);
    Kernel::AddTask(&blinker2);
    Kernel::AddTask(&blinker3);

    while(1) {
        Delay(130);
        System::dbgcounter--;

    }

}