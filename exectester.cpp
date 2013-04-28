#include "include/list.h"
#include "include/task.h"
#include "include/kernel.h"


class NamedNode : public Node {

    char* name;
public:
    NamedNode(const char* nodename) : Node() { }

};

class Msg : public NamedNode {
    void* msgptr;
public:
    Msg(const char* name) : NamedNode(name) {
        
    }
};

class TestTask : public Task<256> {
 public:
    void OnRun() {

        TaskBase* myself = Kernel::GetRunningTask();

    }
};

TestTask newtask;

void testexec() {

    Msg testnode("puppa");
    List msglist;
    msglist.AddAsFirst(&testnode);
    Msg* cursor = (Msg*)msglist.GetFirst();

    while(msglist.IsTail(cursor) == false) {
        // do things here
        cursor = (Msg*)cursor->GetNext();
    }

    Kernel::AddTask(&newtask);

}
