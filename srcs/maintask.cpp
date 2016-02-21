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
#include "haldriver/usbmanager.h"

MainTask parenttask;

// led and button present on the Pinguino micro
OutputPin testled(IOPORT_B, BIT_15);
InputPin dmpbutton(IOPORT_D, BIT_8);

UartDefault dbgserial(UART2, 115200, "\r");

OutStream dbgout(dbgserial);

UsbManager usb;
//UsbCdcSerialDevice usbserial(usb) ;

class BlinkerTask : public Task<1024> {

    Int32 fakecounter;

public:
    BlinkerTask() : fakecounter(0) {
        priority = TSPRI_NORMAL;
    }

    void OnRun()  {
        UInt32 counter = 0;


        dbgout << "Blinky started\r\n" ;

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

} blinker1;



void MainTask::OnRun() {

    dbgout << "Helloworld\r\n" ;
    dbgout << "Pic32Copter board here\r\n\r\n" ;
    Delay(500);

    dbgserial.setMode(UartManager::RX_M_WAIT_EOL, UartManager::TX_M_NOWAIT);

    dbgserial.setLocalEcho(true);
    dbgserial.setBlockingTimeout(10000);

    Kernel::AddTask(&blinker1);

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



    while(1) {

    }
}
