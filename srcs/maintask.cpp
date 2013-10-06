/*
 * File:   maintask.c
 * Author: Georg
 *
 * Created on 28 May 2013, 23.31
 */


#include "maintask.h"
#include "kernel/kernel.h"

#include "kernel/semaphore.h"
#include "kernel/message.h"
#include "kernel/outstream.h"

#include "driver/digitaliomanager.h"
#include "driver/uartmanager.h"

MainTask parenttask;

// led present on the Pinguino micro
OutputPin testled(IOPORT_D, BIT_1);
UartManager dbgserial(UART1,9600);

OutStream dbgout(dbgserial);

class ProtectedResource : public Semaphore {
    Int32 counter;
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

    Int32 fakecounter;

public:
    BlinkerTask() : fakecounter(0) {
        priority = TSPRI_NORMAL;
    }

    void OnRun()  {
        while(1) {
            testled << true;
            //dbgout << "Led on\r\n" ;
            Delay(1200);
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
            //dbgout << "Led off\r\n" ;
            Delay(1200);
        }
    }

} blinker3;


void MainTask::OnRun() {

    dbgout << "Helloworld\r\n" ;
    dbgout << "Pic32Copter board here\r\n" ;

    Kernel::AddTask(&outrunning);
    Kernel::AddTask(&blinker2);
    Kernel::AddTask(&blinker3);

    while(1) {
        Delay(90);
        System::dbgcounter++;
        dbgout << "01234567890123456789 maintask running cycle:" << System::dbgcounter << "\r\n";
    }

}