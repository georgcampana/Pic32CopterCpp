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


class BlinkerTask : public Task<2048> {

    void OnRun() {
        while(1) {
            testled << true;
            //Delay(3000);
            Kernel::QuantumElapsed();
            System::dbgcounter++;
        }
    }

} blinker;






void MainTask::OnRun() {


    Kernel::AddTask(&blinker);

    while(1) {
        testled << false;
        //Delay(3000);
        Kernel::QuantumElapsed();
        System::dbgcounter--;
    }

}