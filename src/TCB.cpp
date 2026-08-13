//
// Task Control Block implementation
//

#include "../h/TCB.hpp"
#include "../h/Riscv.hpp"
#include "../h/Sem.hpp"
#include "../h/syscall_c.hpp"
#include "TCB.hpp"


TCB *TCB::running = nullptr;
uint64 TCB::timeSliceCounter = 0;
int TCB::nextId = 0;
int TCB::maxThreads = 5;
bool TCB::limitThreads = false;
Sem* TCB::threadsSem = nullptr;

TCB::TCB(Body body, void* arguments, uint64* stack)
{
    this->body = body;
    this->stack = (body != nullptr ? stack : nullptr);
    this->context = {body!= nullptr ? (uint64) &threadWrapper : 0,
                 stack != nullptr ? (((uint64) stack) + DEFAULT_STACK_SIZE) : 0
                };
    this->arguments = arguments;
    this->finished = false;
    this->blocked = false;
    this->semaphore = Sem::createSemaphore(0);
    this->id = nextId++;

    {
        if (body != nullptr) { Scheduler::put(this); }
    }
}


void TCB::dispatch()
{
    TCB *old = running;
    if (!old->isFinished() && !old->isBlocked()) { Scheduler::put(old); }
    else if (old->isFinished() && old->parent != nullptr) {
        old->parent->childrenCount--;
    if (old->parent->childrenCount == 0) {
        old->parent->childrenSem->signal();
    }
}
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);
}


void TCB::exit()
{
    TCB *old = running;
    old->setFinished(true);
    old->semaphore->close();
    if (limitThreads) { threadsSem->signal(); }
    TCB::dispatch();
}


TCB *TCB::createThread(Body body, void* arguments, uint64 *stack)
{
    return new TCB(body, arguments, stack);
}


void TCB::threadWrapper()
{
    if (limitThreads) { threadsSem->wait(); }
    Riscv::popSppSpie();
    running->body(running->arguments);
	thread_exit();
}

void TCB::join()
{
    this->semaphore->wait();
}

void TCB::addChild(TCB * child)
{
     running->childrenCount++; 
     child->parent = running; 
     if(!running->childrenSem) running->childrenSem = Sem::createSemaphore(0);
}

void TCB::joinAll()
{
    if(running->childrenCount > 0) running->childrenSem->wait();
}

void TCB::setMax(int max){
    maxThreads = max;
    limitThreads = true;
    threadsSem = Sem::createSemaphore(maxThreads);
}