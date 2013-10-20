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
 * File:   list.cpp
 * Author: georg
 * 
 * Created on 27 aprile 2013, 0.02
 */

#include "list.h"


// put in front of the first node with a lower priority
// Example:  4444444333333322222211111110000000
// add a pri 2 would set it here: 44444443333333222222[2]11111110000000

void List::Enqueue(Node* newnode) {
    Node* cursor = GetFirst();
    while(IsTail(cursor) == false) {
        if(newnode->GetPriority() > cursor->GetPriority()) { break; }
        cursor = cursor->GetNext();
    }
    cursor->AddInFront(newnode);
}