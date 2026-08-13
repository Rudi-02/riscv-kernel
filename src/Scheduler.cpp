//
// Scheduler implementation
//

#include "../h/Scheduler.hpp"
#include "../lib/mem.h"

Scheduler::Elem* Scheduler::head= nullptr;

void Scheduler::put(TCB* thread)
{
    Elem* newThread = new Elem;
    newThread->thread = thread;
    newThread->next = nullptr;

    if (head == nullptr) { head = newThread; }
    else {
        Elem* previous = nullptr;
        for(Elem* iter = head; iter ; iter=iter->next) previous = iter;
            previous->next = newThread;
    }
}

TCB* Scheduler::get()
{
    Elem* elem = head;
    head = head->next;
    TCB* releasedThread = elem->thread;
    delete elem;
    return releasedThread;
}

