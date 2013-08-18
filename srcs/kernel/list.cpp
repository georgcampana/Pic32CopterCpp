/* 
 * File:   list.cpp
 * Author: georg
 * 
 * Created on 27 aprile 2013, 0.02
 */

#include "list.h"
#include "system.h"


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