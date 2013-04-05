/*
 * File:   i2cmanager.c
 * Author: Georg Campana
 *
 * Created on 22 ottobre 2012, 0.29
 */

#include "include/digitaliomanager.h"

#include <p32xxxx.h>
#include <plib.h>

// generic pin interface
OutputPin::OutputPin(int pin) : pinno(pin) {

}
void OutputPin::operator << (bool newstatus) {
    newstatus? Set() : Clear();
}
bool OutputPin::set(bool newstatus) {
    newstatus? Set() : Clear();
}

OutPinPortA::OutPinPortA(int pin) : OutputPin(pin) {}
void OutPinPortA::Toggle() {  mPORTDToggleBits(pinno); }
void OutPinPortA::Clear() {  mPORTDClearBits(pinno);  }
void OutPinPortA::Set() {   mPORTDSetBits(pinno);   }
void OutPinPortA::SetAsOut() { mPORTDSetPinsDigitalOut(pinno); }

OutPinPortB::OutPinPortB(int pin) : OutputPin(pin) {}
void OutPinPortB::Toggle() {  mPORTDToggleBits(pinno); }
void OutPinPortB::Clear() {  mPORTDClearBits(pinno);  }
void OutPinPortB::Set() {   mPORTDSetBits(pinno);   }
void OutPinPortB::SetAsOut() { mPORTDSetPinsDigitalOut(pinno); }

OutPinPortC::OutPinPortC(int pin) : OutputPin(pin) {}
void OutPinPortC::Toggle() {  mPORTDToggleBits(pinno); }
void OutPinPortC::Clear() {  mPORTDClearBits(pinno);  }
void OutPinPortC::Set() {   mPORTDSetBits(pinno);   }
void OutPinPortC::SetAsOut() { mPORTDSetPinsDigitalOut(pinno); }

OutPinPortD::OutPinPortD(int pin) : OutputPin(pin) {}
void OutPinPortD::Toggle() {  mPORTDToggleBits(pinno); }
void OutPinPortD::Clear() {  mPORTDClearBits(pinno);  }
void OutPinPortD::Set() {   mPORTDSetBits(pinno);   }
void OutPinPortD::SetAsOut() { mPORTDSetPinsDigitalOut(pinno); }