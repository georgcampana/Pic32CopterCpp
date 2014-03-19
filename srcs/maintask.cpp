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
#include "oslib/outstream.h"
#include "oslib/instream.h"

#include "haldriver/digitaliomanager.h"
#include "haldriver/uartmanager.h"
#include "haldriver/i2cmanager.h"
#include "haldriver/spimanager.h"
#include "driver/mpu-9150.h"
#include "driver/mrf24wlan.h"

#include "oslib/streamparser.h"

MainTask parenttask;

// led and button present on the Pinguino micro
OutputPin testled(IOPORT_D, BIT_1);
InputPin dmpbutton(IOPORT_D, BIT_0);

UartDefault dbgserial(UART1, 115200, "\r");

OutStream dbgout(dbgserial);

// MPU_9150 with interrupt pin
I2c chipbus(I2C1, 150000);
MPU_9150 motionsensor(chipbus);
InterruptPin mpuinterrupt(4,  &motionsensor); // INT4 external pin

// MRF24WLAN with interrupt pin - 1MHz bus speed
OutputPin wifiselect(IOPORT_F, BIT_0, true); // true = open-drain
SpiManager spibus(SPI_CHANNEL2, 1000000, &wifiselect );
OutputPin wifireset(IOPORT_D, BIT_5, true); // true = open-drain
Mrf24Wlan wifi(spibus, wifireset);
InterruptPin wifiinterrupt(1, &wifi);





class GsmCEng : public StreamParser {

    Int32Element mcc;
    StrElement<10> name;
    SkipElement datoskippato;

public:

    GsmCEng() : mcc(&name), name(&datoskippato,"Puppa"),datoskippato(), StreamParser(&mcc){} ;
};

GsmCEng linea1();

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
#ifdef DEBUG
                Kernel::dbg_DumpStatus(dbgout);
#endif
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
    dbgout << "Pic32Copter board here\r\n\r\n" ;
    Delay(500);

    dbgserial.setMode(UartManager::RX_M_WAIT_EOL, UartManager::TX_M_NOWAIT);

    dbgserial.setLocalEcho(true);
    dbgserial.setBlockingTimeout(10000);

    Kernel::AddTask(&outrunning);
    Kernel::AddTask(&blinker2);
    Kernel::AddTask(&blinker3);

    Char psw[16];
    Int32 insint;

    dbgout << "testinstream: ";
    InStream dbgin(dbgserial);

    dbgin.setMaxStringLen(16);
    dbgin >> insint;
    dbgin >> psw;
    dbgout << "data parsed:" << insint << " " << psw << "\r\n";
    dbgout << "lasterror:" << (Int32)dbgin.lasterror;
    
    //dbgout << "password ?: " << "\r\n";
    //dbgserial.readLine(psw, sizeof(psw) );
    //dbgout << "you said: " << (Char*) psw << "\r\n";
    Delay(4000);

    wifi.reset();
    Delay(100);

    bool mpuerror=false;
    mpuerror = motionsensor.Init();
    
    motionsensor.SetFifoDest(&pkt);

    if(!mpuerror) mpuerror = motionsensor.EnableFifo();
    motionsensor.EnableInterrupt(this);

    while(1) {

        UInt16 len = 0;

        if(len < MPU_9150::MpuFifoPacket::FifoPktLength) {
            if(motionsensor.WaitForNextPacket(2000) == false) {
                dbgout << "timeout\r\n";
            }

            if(!mpuerror) {
                mpuerror = motionsensor.ReadFifoLength(&len);
                //dbgout << "len:" ;
                //dbgout << (Int32)len ;
            }
        }
        else {
            dbgout << "one packet more :)\r\n";
        }

        if(!mpuerror) {
            if(len >= MPU_9150::MpuFifoPacket::FifoPktLength) {
                mpuerror = motionsensor.GetNextPacket();


                if(mpuerror == false) {
                    dbgout << SysTimer::GetNowMillisecs() << ": ";
                    //dbgout << (Int32)(pkt.Temp/34 + 350) ; // Note Celsius = HwTemp/340+35
                    dbgout << (Int32)pkt.GyroZ;
                    dbgout << "\r";
                    len -= MPU_9150::MpuFifoPacket::FifoPktLength;
                }
            }
            else {
              dbgout << "too short:" ;
              dbgout << (Int32)len ;
            }
        }

        if(mpuerror) {
          dbgout << "mpuerror was true :(\r\n";
        }
    }

}
