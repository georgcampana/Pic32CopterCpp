/* 
 * File:   signal.h
 * Author: gcamp_000
 *
 * Created on 28 aprile 2013, 16.15
 */

#ifndef SIGNAL_H
#define	SIGNAL_H

class SignalPool {
 public: 
    typedef unsigned int SIGNAL;
    typedef unsigned int SIGNALMASK;
     
    SignalPool();
 
    void Set(int sigbit);
    SIGNAL Alloc();
    void Free(SIGNAL sig2free);
    SIGNALMASK CheckAndReset(SIGNALMASK signals2check);

 private:
    SIGNALMASK signals_alloc;
    SIGNALMASK signals_set;
};

inline SignalPool::SignalPool() : signals_alloc(0), signals_set(0) {}

inline void SignalPool::Set(int sigbit) { signals_set |= sigbit; }


inline void SignalPool::Free(SignalPool::SIGNAL sig2free) {
    signals_alloc &= ~sig2free;
    signals_set &= ~sig2free;
}

inline SignalPool::SIGNALMASK SignalPool::CheckAndReset(SignalPool::SIGNALMASK signals2check) {
    SIGNALMASK result = (signals_alloc & signals2check) & signals_set;
    signals_set &= ~(signals_alloc & signals2check);
    return result;
}

#endif	/* SIGNAL_H */

