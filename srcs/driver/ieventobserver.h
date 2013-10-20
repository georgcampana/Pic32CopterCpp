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
 * File:   IEventObserver.h
 * Author: Georg Campana
 *
 * Created on 4 aprile 2013, 15.22
 */

#ifndef IEVENTOBSERVER_H
#define	IEVENTOBSERVER_H

class IEventObserver {

public:
    virtual bool onEventFired(Int32 eventid) = 0;
    

};

typedef IEventObserver* IEventObserverPrt;
#endif	/* IEVENTOBSERVER_H */

