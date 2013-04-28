/* 
 * File:   list.cpp
 * Author: georg
 * 
 * Created on 27 aprile 2013, 0.02
 */

#include "include/list.h"



void List::Enqueue(Node* newnode) {
    Node* cursor = GetFirst();
    while(IsTail(cursor) == false) {
        if(newnode->GetPriority() <= cursor->GetPriority()) {
            cursor->AddInFront(newnode);
            return; // not nice (like a goto) but efficient
        }
        cursor = cursor->GetNext();
    }
    tail.AddInFront(newnode);
}