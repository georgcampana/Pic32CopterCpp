/* 
 * File:   DigitalIOManager.h
 * Author: georg
 *
 * Created on 4 aprile 2013, 15.45
 */

#ifndef DIGITALIOMANAGER_H
#define	DIGITALIOMANAGER_H

#include "ieventobserver.h"

typedef class PinChangeHandler* PinChangeHandlerPtr;

class PinChangeHandler {

public:
    enum PullUpDown {
        PUD_NONE =0,
        PUD_UP,
        PUD_DOWN
    };

private:
    friend class DigitalIOManager;
    bool laststatus;
    bool currentstatus;

    PinChangeHandlerPtr nexthandler;
    IEventObserver* observer;
public:
    PinChangeHandler(int pin, PullUpDown pullmode, IEventObserver* observer=0  );

    bool Read();
    bool ReadOld();

};



class DigitalIOManager {

    PinChangeHandlerPtr firsthandler;
public:
    void AddPinChangeHandler(PinChangeHandlerPtr changehandler);

    void handleCnInterrupt();

};

class OutputPin {
    int pinno;
    bool lastset;



public:
    OutputPin(int pin);
    void operator << (bool newstatus) ;
    bool set(bool newstatus);
};

class InputPin {
    int pinno;
    bool lastset;
public:
    InputPin(int pin, bool pullup = false);
    bool get();
};

//class OutPinP

#endif	/* DIGITALIOMANAGER_H */

