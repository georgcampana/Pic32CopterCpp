/* 
 * File:   DigitalIOManager.h
 * Author: georg
 *
 * Created on 4 aprile 2013, 15.45
 */

#ifndef DIGITALIOMANAGER_H
#define	DIGITALIOMANAGER_H

#include "ieventobserver.h"
//#include <plib.h>

class InputPin {
    const IoPortId pinport;
    const int pinno;
    bool lastset;

public:
    InputPin(IoPortId port, int pin);
    bool get();
    InputPin& operator >> (bool& result);
};


typedef class PinChangeHandler* PinChangeHandlerPtr;

class PinChangeHandler {
    const InputPin& inpin;
    const int cnotifypin;
    const IEventObserver* pinobserver;
public:
    enum PullUpDown {
        PUD_NONE =0,
        PUD_UP,
        PUD_DOWN
    };

private:
    friend class DigitalIO;
    bool laststatus;
    bool currentstatus;

    PinChangeHandlerPtr nexthandler;
    IEventObserver* observer;
public:
    PinChangeHandler(   InputPin& pinport, int cnpin,
                        PullUpDown pullmode=PUD_NONE,
                        IEventObserver* observer=0  );

    bool Read();
    bool ReadOld();

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
    const int pinno;
    bool lastset;

public:
    OutputPin(IoPortId port, int pin, bool opendrain=false);
    void operator << (bool newstatus) ;
    bool set(bool newstatus);
    bool toggle();
};




#endif	/* DIGITALIOMANAGER_H */

