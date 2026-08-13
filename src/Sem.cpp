//
// Semaphore implementation
//

#include "../h/Sem.hpp"
#include "../lib/mem.h"

Sem::Sem(uint64 init)
{
    this->value = init;
    this->open = true;
}


uint32 Sem::signal()
{
    if (!this->isOpen()) return -1;

    if(++value <= 0){
        TCB* unblockedThread = getBlocked();
        unblockedThread->setBlocked(false);
        Scheduler::put(unblockedThread);
    }
    return 0;
}


uint32 Sem::wait()
{
    if (!this->isOpen()) return -1;

    if(--value < 0){
        TCB* blockedThread= TCB::running;
        putBlocked(blockedThread);
        blockedThread->setBlocked(true);
        TCB::dispatch();
    	if (!this->isOpen()) return -1;
    }
    return 0;
}


uint32 Sem::tryWait()
{
    if (!this->isOpen()) return -1;

    if(value > 0){
        value--;
        return 0;
    }
    else return 1;
}


uint32 Sem::close()
{
    if (!this->isOpen()) return -1;
    this->setOpen(false);

    for (Elem* iter = blocked; iter; iter = iter->next){
        iter->thread->setBlocked(false);
        Scheduler::put(iter->thread);
    }
    return 0;
}


TCB* Sem::getBlocked()
{
    Elem* firstBlocked = blocked;
    TCB* unblockedThread = firstBlocked->thread;
    blocked = blocked->next;
    delete firstBlocked;
    return unblockedThread;
}


void Sem::putBlocked(TCB* blockedThread)
{
    Elem* newBlocked = new Elem;
    newBlocked->thread = blockedThread;
    newBlocked->next = nullptr;

    if(blocked){
        Elem* previous = nullptr;
        for (Elem* iter = blocked; iter; iter = iter->next) previous = iter;
        previous->next = newBlocked;
    }
    else blocked = newBlocked;
}


Sem* Sem::createSemaphore(uint64 init)
{
    return new Sem(init);
}

