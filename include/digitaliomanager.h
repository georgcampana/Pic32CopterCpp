/* 
 * File:   DigitalIOManager.h
 * Author: georg
 *
 * Created on 4 aprile 2013, 15.45
 */

#ifndef DIGITALIOMANAGER_H
#define	DIGITALIOMANAGER_H

#include "ieventobserver.h"


class InputPin {
    int pinno;
    bool lastset;
public:
    InputPin(int pin, bool pullup = false);
    bool get();
};

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


// OutputPin
class OutputPin {
protected:
    const int pinno;
    bool lastset;

    virtual void Toggle()=0;
    virtual void Clear()=0;
    virtual void Set()=0;
    virtual void SetAsOut()=0;

public:
    OutputPin(int pin);
    void operator << (bool newstatus) ;
    bool set(bool newstatus);
};


class OutPinPortA : public OutputPin {
    virtual void Toggle();
    virtual void Clear();
    virtual void Set();
    virtual void SetAsOut();
public:
    OutPinPortA(int pin);
};

class OutPinPortB : public OutputPin {
    virtual void Toggle();
    virtual void Clear();
    virtual void Set();
    virtual void SetAsOut();
public:
    OutPinPortB(int pin);
};

class OutPinPortC : public OutputPin {
    virtual void Toggle();
    virtual void Clear();
    virtual void Set();
    virtual void SetAsOut();
public:
    OutPinPortC(int pin);    
};

class OutPinPortD : public OutputPin {
    virtual void Toggle();
    virtual void Clear();
    virtual void Set();
    virtual void SetAsOut();
public:
    OutPinPortD(int pin);    
};

#endif	/* DIGITALIOMANAGER_H */

