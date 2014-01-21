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
 * File:   DigitalIOManager.h
 * Author: georg
 *
 * Created on 4 aprile 2013, 15.45
 */

#ifndef DIGITALIOMANAGER_H
#define	DIGITALIOMANAGER_H

#include "../kernel/system.h"
#include "../kernel/ieventobserver.h"
//#include <plib.h>

class InputPin {
    const IoPortId pinport;
    const Int32 pinno;
    bool lastset;

public:
    InputPin(IoPortId port, Int32 pin);
    bool get() const;
    InputPin& operator >> (bool& result);
};


typedef class PinChangeHandler* PinChangeHandlerPtr;

class PinChangeHandler {
    const InputPin& inpin;
    const Int32 cnotifypin;
    const IEventObserverPrt pinobserver;
    friend class DigitalIO;
    bool laststatus;
    //bool currentstatus;

    PinChangeHandlerPtr nexthandler;

    void notifyObserver();
public:
    enum PullUpDown {
        PUD_NONE =0,
        PUD_UP,
        PUD_DOWN
    };

    PinChangeHandler(   InputPin& pinport, Int32 cnpin,
                        PullUpDown pullmode=PUD_NONE,
                        IEventObserverPrt observer=0  );

    bool Read() const;
    bool ReadOld() const;
    void SetOld();
};



class DigitalIO {

    PinChangeHandlerPtr firsthandler;
public:
    void addPinChangeHandler(PinChangeHandlerPtr changehandler);

    void handleInterrupt();

    void enableChangeNotification(bool newstatus);

    DigitalIO();

};


// OutputPin
class OutputPin {
    const IoPortId pinport;
    const Int32 pinno;
    bool lastset;

public:
    OutputPin(IoPortId port, Int32 pin, bool opendrain=false);
    void operator << (bool newstatus) ;
    bool set(bool newstatus);
    bool toggle();
};




#endif	/* DIGITALIOMANAGER_H */

