/* 
 * File:   node.h
 * Author: georg
 *
 * Created on 24 aprile 2013, 19.15
 */

#ifndef NODE_H
#define	NODE_H

class Node {
 public:
    enum Nodetype {
         NT_UNKNOWN = 0,
         NT_LISTHEAD,
         NT_LISTTAIL,
         NT_MSG,
         NT_MSGPORT,
         NT_DRIVER,
         NT_INTHANDLER,
         NT_TASK,
         NT_SEMAPHORE,
    };


    Node(Nodetype typ = NT_UNKNOWN, int pri=0, Node* n=0, Node* p=0);

    Node* GetNext() const;
    Node* GetPrev() const;
    int GetPriority() const;
    void RemoveFromList();

    void Append(Node* newnode);
    void AddInFront(Node* newnode);

    //virtual ~Node();

 protected:
    Node* next;
    Node* prev;
    int priority;
    Nodetype ntype;
};

inline Node::Node(Nodetype typ, int pri, Node* n, Node* p) : priority(pri), ntype(typ), next(n), prev(p) {}
inline Node* Node::GetNext() const { return next;}
inline Node* Node::GetPrev() const { return prev;}
inline int Node::GetPriority() const { return priority;}

inline void Node::RemoveFromList() {
    if(next)next->prev = prev;
    if(prev)prev->next = next;
    //TODO: next lines for debug, could be removed
    next=prev=0;
}

inline void Node::Append(Node* newnode) {
    newnode->next = next;
    newnode->prev = this;
    next->prev = newnode;
    next = newnode;
}

inline void Node::AddInFront(Node* newnode) {
    newnode->next = this;
    newnode->prev = prev;
    prev->next = newnode;
    prev = newnode;
}


#endif	/* NODE_H */
