//
// Scheduler header
//

#ifndef PROJECT_BASE_SCHEDULER_HPP
#define PROJECT_BASE_SCHEDULER_HPP

#include "../h/TCB.hpp"
#include "../lib/mem.h"


class TCB;

class Scheduler
{
public:
    static void put(TCB* thread);
    static TCB* get();

private:
    struct Elem
    {
        TCB* thread;
        Elem* next;
    };

    static Elem* head;
};


#endif //PROJECT_BASE_SCHEDULER_HPP
