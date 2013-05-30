/* 
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