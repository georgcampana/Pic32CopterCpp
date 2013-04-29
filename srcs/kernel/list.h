/* 
 * File:   list.h
 * Author: georg
 *
 * Created on 27 aprile 2013, 0.02
 */

#ifndef LIST_H
#define	LIST_H

#include "node.h"

class List {

public:
    List();
    
    Node* GetFirst() const;
    Node* GetLast() const;
    void Enqueue(Node* newnode);
    void Append(Node* newnode);
    void AddAsFirst(Node* newnode);
    
    bool IsTail(const Node* node2check) const;
    bool IsHead(const Node* node2check) const;
    //virtual ~List();
private:

    Node head;
    Node tail;
};

inline bool List::IsTail(const Node* node2check) const {
    return (node2check == &tail)? true : false;
}

inline bool List::IsHead(const Node* node2check) const {
    return (node2check == &head)? true : false;
}

inline  Node* List::GetFirst() const {
    return head.GetNext();
}

inline List::List() : head(Node::NT_LISTHEAD, 0, &tail, 0),
                      tail(Node::NT_LISTTAIL, 0, 0, &head) {}

inline void List::Append(Node* newnode) {
    tail.AddInFront(newnode);
}

inline void List::AddAsFirst(Node* newnode) {
    head.Append(newnode);
}



#endif	/* LIST_H */

