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
 * File:   signal.h
 * Author: gcamp_000
 *
 * Created on 28 aprile 2013, 16.15
 */

#ifndef SIGNAL_H
#define	SIGNAL_H

#include "basetypes.h"

class SignalPool {
 public: 
    typedef UInt32 SIGNAL;
    typedef UInt32 SIGNALMASK;
     
    SignalPool();
 
    void Set(SIGNAL sigbit);
    SIGNAL Alloc();
    void Free(SIGNAL sig2free);
    SIGNALMASK CheckAndReset(SIGNALMASK signals2check);
    SIGNALMASK CheckWaiting() const;
    void SetWaitingSigs(SIGNALMASK signals2wait);

    static const SIGNAL SYSTIMER_SIG  = 0x00000001; // preallocated signal for the Systimer
    static const SIGNAL FIRSTFREE_SIG = 0x00000002; // first user available signal

 private:
    SIGNALMASK signals_alloc; // allocated signales
    SIGNALMASK signals_set; // arrived signales
    SIGNALMASK signals_waitingfor; // waitingfor signals

};

inline SignalPool::SignalPool() : signals_alloc(SYSTIMER_SIG), signals_set(0) {}

inline void SignalPool::Set(SIGNAL sigbit) { signals_set |= sigbit; }


inline void SignalPool::Free(SignalPool::SIGNAL sig2free) {
    signals_alloc &= ~sig2free;
    signals_set &= ~sig2free;
}

inline SignalPool::SIGNALMASK SignalPool::CheckAndReset(SignalPool::SIGNALMASK signals2check) {
    SIGNALMASK result = (signals_alloc & signals2check) & signals_set;
    signals_set &= ~(signals_alloc & signals2check);
    return result;
}

inline SignalPool::SIGNALMASK SignalPool::CheckWaiting() const {
    return (signals_alloc & signals_waitingfor) & signals_set;
}

inline void SignalPool::SetWaitingSigs(SIGNALMASK signals2wait) {
    signals_waitingfor = signals2wait;
}

#endif	/* SIGNAL_H */

