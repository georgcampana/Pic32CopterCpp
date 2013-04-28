/* 
 * File:   list.cpp
 * Author: georg
 * 
 * Created on 27 aprile 2013, 0.02
 */

#include "include/list.h"


// put in front of the first node with a lower priority
// Example:  0000000111111122222233333344444444
// add a pri 2 would set it here: 00000001111111222222[2]33333344444444

void List::Enqueue(Node* newnode) {
    Node* cursor = GetFirst();
    while(IsTail(cursor) == false) {
        if(newnode->GetPriority() < cursor->GetPriority()) {
            cursor->AddInFront(newnode);
            return; // not nice (like a goto) but efficient
        }
        cursor = cursor->GetNext();
    }
    tail.AddInFront(newnode);
}