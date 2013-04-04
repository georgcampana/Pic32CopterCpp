/* 
 * File:   IEventObserver.h
 * Author: Georg Campana
 *
 * Created on 4 aprile 2013, 15.22
 */

#ifndef IEVENTOBSERVER_H
#define	IEVENTOBSERVER_H

class IEventObserver {

    virtual bool onEventFired(int eventid) = 0;
    

};


#endif	/* IEVENTOBSERVER_H */

