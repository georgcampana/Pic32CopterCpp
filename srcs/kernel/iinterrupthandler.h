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
 * File:   iinterrupthandler.h
 * Author: Georg Campana
 *
 * Created on 21 gennaio 2014, 14.56
 */

#ifndef IINTERRUPTHANDLER_H
#define	IINTERRUPTHANDLER_H

class IInterruptHandler {

public:

    virtual void handleInterrupt() {} ;
    
};

typedef IInterruptHandler* IInterruptHandlerPrt;

#endif	/* IINTERRUPTHANDLER_H */

