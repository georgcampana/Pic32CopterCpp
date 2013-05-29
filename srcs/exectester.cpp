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





