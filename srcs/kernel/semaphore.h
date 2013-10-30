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
 * File:   semaphore.h
 * Author: Georg Campana
 *
 * Created on 28 aprile 2013, 18.53
 */

#ifndef SEMAPHORE_H
#define	SEMAPHORE_H

#include "task.h"

class Semaphore : Node {
public:

    bool Obtain(Int32 maxwaitms = -1);
    bool TryObtain();
    void Release();

    Semaphore();
protected:

    TaskBase* grantedtask;
    List taskqueue;
};

inline Semaphore::Semaphore() : Node(Node::NT_SEMAPHORE), grantedtask(NULL) {}

class NestableSemaphore : Semaphore {
   UInt32 nestingcounter;
public:
   NestableSemaphore();
   bool ObtainNested(Int32 maxwaitms = -1);
   bool TryObtainNested();
   void ReleaseNested();

};

inline NestableSemaphore::NestableSemaphore() : nestingcounter(0) {}



#endif	/* SEMAPHORE_H */

