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
#include "driver/mpu-9150.h"

MainTask parenttask;

// led and button present on the Pinguino micro
OutputPin testled(IOPORT_D, BIT_1);
InputPin dmpbutton(IOPORT_D, BIT_0);


UartManager dbgserial(UART1, 115200);

OutStream dbgout(dbgserial);

I2c chipbus(I2C1, 150000);
MPU_9150 motionsensor(chipbus);

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
            //dbgout << counter++ << " Led toggled\r\n" ;
            //dbgout << "flag is: " << System::dbgflag;
            if(dmpbutton.get()==false) {
                dbgout << "MT:" << (UInt32) &parenttask;
                Kernel::dbg_DumpStatus(dbgout);
            }
            Delay(1000);
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


MPU_9150::MpuFifoPacket pkt;

void MainTask::OnRun() {

    dbgout << "Helloworld\r\n" ;
    dbgout << "Pic32Copter board here\r\n" ;
    Delay(500);

    dbgserial.setLocalEcho(true);

    Kernel::AddTask(&outrunning);
    Kernel::AddTask(&blinker2);
    Kernel::AddTask(&blinker3);

    motionsensor.Init();


    bool mpuerror=false;
    motionsensor.SetFifoDest(&pkt);
    if(!mpuerror) mpuerror = motionsensor.EnableFifo();

    while(1) {

        UInt16 len = 0xeeee;
        if(!mpuerror) {
            mpuerror = motionsensor.ReadFifoLength(&len);
            //dbgout << "len:" ;
            //dbgout << (Int32)len ;
        }

        if(!mpuerror && len >= MPU_9150::MpuFifoPacket::FifoPktLength)
        {
            mpuerror = motionsensor.GetNextPacket();

        
            if(mpuerror == false) {
                dbgout << SysTimer::GetNowMillisecs() << ": ";
                //dbgout << (Int32)(pkt.Temp/34 + 350) ; // Note Celsius = HwTemp/340+35
                dbgout << (Int32)pkt.GyroZ;
                dbgout << "\r";
            }
        }

        if(mpuerror) {
          dbgout << "mpuerror was true :(\r\n";
        }

        Delay(5);
        System::dbgcounter++;
        //dbgout << "maintask running cycle:" << System::dbgcounter << "\r\n";
    }

}
