/*
 * File:   i2cmanager.c
 * Author: Georg Campana
 *
 * Created on 22 ottobre 2012, 0.29
 */

#include <p32xxxx.h>
#include <plib.h>

#include "include/digitaliomanager.h"


DigitalIO* digitinout_ref;

#ifdef	__cplusplus
extern "C" {
#endif

void __ISR(_CHANGE_NOTICE_VECTOR, ipl6) DigitalIO1InterruptServiceRoutine(void)
{
    digitinout_ref->handleInterrupt();
}


#ifdef	__cplusplus
}
#endif

void DigitalIO::handleInterrupt() {
   System::dbgcounter++;

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

InputPin::InputPin(IoPortId port, int pin) : pinport(port), pinno(pin) {
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
                        InputPin& pinport, int cnpin,
                        PullUpDown pullmode,
                        IEventObserver* observer) : inpin(pinport), cnotifypin(cnpin), pinobserver(observer) {

    int pullup = 0;
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
OutputPin::OutputPin(IoPortId port, int pin, bool opendrain) : pinno(pin), pinport(port) {
    PORTSetPinsDigitalOut(pinport,pinno);
    PORTClearBits(pinport, pinno);
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

