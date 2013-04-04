/* 
 * File:   DigitalIO.h
 * Author: georg
 *
 * Created on 4 aprile 2013, 15.45
 */

#ifndef DIGITALIO_H
#define	DIGITALIO_H

class CNPin {
    IEventObserver* observer;
};


class DigitalIO {

    
    void RegPinChangeObserver(IEventObserver* pinobserver);

    void handleCnInterrupt();



};


#endif	/* DIGITALIO_H */

