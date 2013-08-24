/*
 * File:   maintask.c
 * Author: Georg
 *
 * Created on 28 May 2013, 23.31
 */


#include "maintask.h"
#include "kernel/kernel.h"

#include "driver/digitaliomanager.h"


MainTask parenttask;

// led present on the Pinguino micro
OutputPin testled(IOPORT_D, BIT_1);

SignalPool::SIGNAL wakeupsignal;




class BlinkerTask2 : public Task<2048> {
public:
    BlinkerTask2() {
        priority = TSPRI_NORMAL;
        wakeupsignal = tasksignals.Alloc();
    }
    void OnRun() {
        SignalPool::SIGNALMASK receivedsigs;
        //while(1) {
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

} blinker1;

class BlinkerTask : public Task<2048> {

    int fakecounter;

public:
    BlinkerTask() : fakecounter(0) {
        priority = TSPRI_NORMAL;
    }

    void OnRun() {
        while(1) {
            testled << true;
            Delay(100);
            blinker1.Signal(wakeupsignal);
            System::dbgcounter++;
            fakecounter+=100;
        }
    }

} blinker2;

class BlinkerTask3 : public Task<2048> {

    void OnRun() {
        while(1) {
            testled << false;
            Delay(120);
            System::dbgcounter++;
        }
    }

} blinker3;





void MainTask::OnRun() {

    Kernel::AddTask(&blinker1);
    Kernel::AddTask(&blinker2);
    Kernel::AddTask(&blinker3);

    while(1) {
        testled << false;

        Delay(130);
        System::dbgcounter--;

    }

}