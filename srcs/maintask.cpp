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


MainTask parenttask;

// led present on the Pinguino micro
OutputPin testled(IOPORT_D, BIT_1);

SignalPool::SIGNAL wakeupsignal;


class ProtectedResource : public Semaphore {
    int counter;
public:
    ProtectedResource() {};


} testsemaphore;


class BlinkerTask2 : public Task<2048> {
public:
    BlinkerTask2() {
        priority = TSPRI_NORMAL;
        wakeupsignal = tasksignals.Alloc();
    }
    void OnRun() {
        SignalPool::SIGNALMASK receivedsigs;
        //while(1) {
            Delay(120);
            bool gotit = testsemaphore.Obtain(30);
            if(gotit) {
                Delay(120);
                testsemaphore.Release();
            }
            receivedsigs = Wait(wakeupsignal,120); // waits
            testled << true;
            Delay(80);
            System::dbgcounter++;
            Delay(80);
            receivedsigs = Wait(wakeupsignal); // continues
            System::dbgcounter++;
            Delay(80);
            System::dbgcounter++;

        //}
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
            bool gotit = testsemaphore.Obtain(2000);
            if(gotit) {
                Delay(1200);
                testsemaphore.Release();
            }
            testled << true;
            Delay(1000);
            outrunning.Signal(wakeupsignal);
            System::dbgcounter++;
            fakecounter+=100;
        }
    }

} blinker2;

class BlinkerTask3 : public Task<2048> {

    void OnRun() {
        while(1) {
            testled << false;
            Delay(1200);
            System::dbgcounter++;
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