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
 * File:   signal.cpp
 * Author: gcamp_000
 * 
 * Created on 28 aprile 2013, 16.15
 */

#include "signal.h"


SignalPool::SIGNAL SignalPool::Alloc() {
    SIGNAL newalloc = FIRSTFREE_SIG;
    while(newalloc & signals_alloc) { newalloc <<= 1; }
    signals_alloc |= newalloc;
    return newalloc; // returns 0 in case of no more available signal bits
}