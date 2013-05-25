#include "kernel/list.h"
#include "kernel/task.h"
#include "kernel/kernel.h"
#include "kernel/system.h"

#include <p32xxxx.h>
#include <plib.h>

#include "driver/digitaliomanager.h"


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

OutputPin testled(IOPORT_D, BIT_1);

class BlinkerTask : public Task<2048> {
    // led present on the Pinguino micro

    void OnRun() {
        while(1) {
            testled << true;
            Kernel::QuantumElapsed();
            System::dbgcounter++;
        }
    }

};


class MainTask : public Task<2048> {
    BlinkerTask blinker;
 public:
    void OnRun() {

        Kernel::AddTask(&blinker);

        TaskBase* myself = Kernel::GetRunningTask();

        while(1) {
            testled << false;
            Kernel::QuantumElapsed();
            System::dbgcounter--;
        }

    }
};

MainTask parenttask;

void testexec() {

    Msg testnode("puppa");
    List msglist;
    msglist.AddAsFirst(&testnode);
    Msg* cursor = (Msg*)msglist.GetFirst();

    while(msglist.IsTail(cursor) == false) {
        // do things here
        cursor = (Msg*)cursor->GetNext();
    }

    // this will never come back
    Kernel::startMainTask(&parenttask);

}
