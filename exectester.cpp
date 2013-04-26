#include "include/list.h"

class NamedNode : public Node {

    char* name;
public:
    NamedNode(const char* nodename) : Node() { }

};

class Message : public NamedNode {
    void* msgptr;
public:
    Message(const char* name) : NamedNode(name) {
        
    }
};

void testexec() {

    Message testnode("puppa");

    List msglist;

    msglist.AddAsFirst(&testnode);

    Message* cursor = (Message*)msglist.GetFirst();

    while(msglist.IsTail(cursor) == false) {

        // do things here
        cursor = (Message*)cursor->GetNext();
    }

}
