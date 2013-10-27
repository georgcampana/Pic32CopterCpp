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
#include "driver/i2cmanager.h"

MainTask parenttask;

// led present on the Pinguino micro
OutputPin testled(IOPORT_D, BIT_1);
UartManager dbgserial(UART1, 115200);

OutStream dbgout(dbgserial);

I2c chipbus(I2C1, 50000);

class ProtectedResource : public Semaphore {
    Int32 counter;
public:
    ProtectedResource() {};


} testsemaphore;


class OutRunningTask : public Task<1024> {

public:
    OutRunningTask() {
        priority = TSPRI_NORMAL;
    }
    void OnRun() {
        testsemaphore.Obtain();
        dbgout << "Outrun: Semaphore taken\r\n" ;
        Delay(2000);
        testsemaphore.Release();
        dbgout << "Outrun: Semaphore released\r\n" ;
        dbgout << "Outrunning task: bye bye\r\n" ;
    }

} outrunning;

class BlinkerTask : public Task<1024> {

    Int32 fakecounter;

public:
    BlinkerTask() : fakecounter(0) {
        priority = TSPRI_NORMAL;
    }

    void OnRun()  {
        UInt32 counter = 0;
        Delay(200);
        if(testsemaphore.TryObtain() == true ) {
            dbgout << "Blinky: Tried worked\r\n" ;
        }
        else {
             dbgout << "Blinky: Tried failed\r\n" ;
             testsemaphore.Obtain();
             dbgout << "Blinky: released\r\n";
        }

        dbgout << "Blinky: sem released\r\n" ;

        while(1) {
            testled.toggle();
            dbgout << counter++ << " Led toggled\r\n" ;
            Delay(1200);
        }
    }

} blinker2;

class BlinkerTask3 : public Task<1024> {
    
    Message testmsg;

public:

    BlinkerTask3() :  testmsg(0xee,this) {

    }

    void OnRun() {
        while(1) {
            //testled << false;
            //dbgout << "Led off\r\n" ;
            Delay(1500);
        }
    }

} blinker3;


void MainTask::OnRun() {

    dbgout << "Helloworld\r\n" ;
    dbgout << "Pic32Copter board here\r\n" ;
    Delay(500);

    dbgserial.setLocalEcho(true);

    Kernel::AddTask(&outrunning);
    Kernel::AddTask(&blinker2);
    Kernel::AddTask(&blinker3);

    UInt8 tmp = 0xee;
    if(chipbus.open()) {
        chipbus.ReadByteFromReg( 0x68, 0x75 , &tmp ); // MPU-6050 whoami
        dbgout << "Whoami of the MPU 6050 is: "  << tmp << "\r\n";
        chipbus.close();
    }

    
    while(1) {
        Delay(43);
        System::dbgcounter++;
        dbgout << "maintask running cycle:" << System::dbgcounter << "\r\n";
    }

}