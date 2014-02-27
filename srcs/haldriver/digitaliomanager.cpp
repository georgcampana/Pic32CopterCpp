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
 * File:   digitaliomanager.cpp
 * Author: Georg Campana
 *
 * Created on 22 ottobre 2012, 0.29
 */

#include <p32xxxx.h>
#include <plib.h>

#include "digitaliomanager.h"


DigitalIO* digitinout_ref;

InterruptPin* int_ref[5];


#ifdef	__cplusplus
extern "C" {
#endif
// __ISR(_CHANGE_NOTICE_VECTOR, ipl6)
void  DigitalIO1InterruptServiceRoutine(void)
{
    digitinout_ref->handleInterrupt();
}

void  InterruptPin0Service(void)
{
    int_ref[0]->handleInterrupt();
}

void  InterruptPin1Service(void)
{
    int_ref[1]->handleInterrupt();
}

void  InterruptPin2Service(void)
{
    int_ref[2]->handleInterrupt();
}

void  InterruptPin3Service(void)
{
    int_ref[3]->handleInterrupt();
}

void  InterruptPin4Service(void)
{
    int_ref[4]->handleInterrupt();
}

#ifdef	__cplusplus
}
#endif

void DigitalIO::handleInterrupt() {

   PinChangeHandlerPtr cnhandler = firsthandler;

   while(cnhandler) {

       if(cnhandler->ReadOld() != cnhandler->Read()) {
           cnhandler->notifyObserver();
           cnhandler->SetOld();
       }

       cnhandler = cnhandler->nexthandler;
   }

   mCNClearIntFlag();
}

InputPin::InputPin(IoPortId port, Int32 pin) : pinport(port), pinno(pin) {
    PORTSetPinsDigitalIn(pinport, pinno);
}

bool InputPin::get() const {
    return (PORTReadBits(pinport, pinno))? true : false;
}

InputPin& InputPin::operator >> (bool& result) {
    result = (PORTReadBits(pinport, pinno))? true : false;
    return *this;
}


PinChangeHandler::PinChangeHandler(
                        InputPin& pinport, Int32 cnpin,
                        PullUpDown pullmode,
                        IEventObserver* observer) : inpin(pinport), cnotifypin(cnpin), pinobserver(observer) {

    Int32 pullup = 0;
    if(pullmode == PUD_UP) {
        pullup = cnpin; // the enable bit seems to be the same used for the pullup enabling
    }

    // Ex. mCNOpen(CN_ON | CN_IDLE_CON, CN0_ENABLE | CN1_ENABLE, CN0_PULLUP_ENABLE | CN1_PULLUP_ENABLE);
    mCNOpen(CN_ON | CN_IDLE_CON, cnpin, pullup);

    SetOld();
}

void PinChangeHandler::notifyObserver() {
    if(pinobserver) pinobserver->onEventFired(0);
}

bool PinChangeHandler::Read() const {
    return inpin.get();
}
bool PinChangeHandler::ReadOld() const {
    return laststatus;
}

void PinChangeHandler::SetOld() {
    laststatus = Read();
}

DigitalIO::DigitalIO() {
    firsthandler = NULL;

    digitinout_ref = this;
}

void DigitalIO::enableChangeNotification(bool newstatus) {

    //ConfigIntCN(CHANGE_INT_PRI_3 | CHANGE_INT_ON);
    if(newstatus) {
        ConfigIntCN(CHANGE_INT_PRI_6 | CHANGE_INT_ON );
    }
    else {
        mCNIntEnable(0);
    }
}


void DigitalIO::addPinChangeHandler(PinChangeHandlerPtr changehandler) {
    if(firsthandler==NULL) {
        firsthandler = changehandler;
    }
    else {
        PinChangeHandlerPtr lasthandler = firsthandler;

        while(lasthandler->nexthandler) {
            lasthandler = lasthandler->nexthandler;
        }
        lasthandler->nexthandler = changehandler;
    }
}

// generic pin interface
OutputPin::OutputPin(IoPortId port, Int32 pin, bool opendrain) : pinno(pin), pinport(port) {
    PORTSetPinsDigitalOut(pinport,pinno);
    PORTClearBits(pinport, pinno);
    if(opendrain) {
        switch(port) {
            case IOPORT_B : mPORTBOpenDrainOpen(pinno); break;
            case IOPORT_C : mPORTCOpenDrainOpen(pinno); break;
            case IOPORT_D : mPORTDOpenDrainOpen(pinno); break;
            case IOPORT_E : mPORTEOpenDrainOpen(pinno); break;
            case IOPORT_F : mPORTFOpenDrainOpen(pinno); break;
        }

    }
}
void OutputPin::operator << (bool newstatus) {
    newstatus? PORTSetBits(pinport, pinno) : PORTClearBits(pinport, pinno);
}
bool OutputPin::set(bool newstatus) {
    newstatus? PORTSetBits(pinport, pinno) : PORTClearBits(pinport, pinno);
}
bool OutputPin::toggle() {
    PORTToggleBits(pinport, pinno);
}

// InterruptPin

InterruptPin::InterruptPin(Int32 intpin, IEventObserverPrt observer, Int8 intpri,
                                bool enable, bool risingedge):
                                        extintpin(intpin), intobserver(observer){
    Int32 config = intpri;
    if(enable) config |= EXT_INT_ENABLE;
    if(risingedge) config |= RISING_EDGE_INT;

    int_ref[intpin] = this;

    switch(intpin) {
        case 0: ConfigINT0(config); break;
        case 1: ConfigINT1(config); break;
        case 2: ConfigINT2(config); break;
        case 3: ConfigINT3(config); break;
        case 4: ConfigINT4(config); break;
    }
};

void InterruptPin::clearPendingInt() {
    switch(extintpin) {
        case 0: mINT0ClearIntFlag(); break;
        case 1: mINT1ClearIntFlag(); break;
        case 2: mINT2ClearIntFlag(); break;
        case 3: mINT3ClearIntFlag(); break;
        case 4: mINT4ClearIntFlag(); break;
    }
}

void InterruptPin::handleInterrupt() {
    clearPendingInt();
    if(intobserver!=NULL) intobserver->onEventFired(extintpin);
}

void InterruptPin::enable() {
    switch(extintpin) {
        case 0: EnableINT0; break;
        case 1: EnableINT1; break;
        case 2: EnableINT2; break;
        case 3: EnableINT3; break;
        case 4: EnableINT4; break;
    }
}

void InterruptPin::disable() {
    switch(extintpin) {
        case 0: DisableINT0; break;
        case 1: DisableINT1; break;
        case 2: DisableINT2; break;
        case 3: DisableINT3; break;
        case 4: DisableINT4; break;
    }
}