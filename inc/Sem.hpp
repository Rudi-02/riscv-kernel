//
// Semaphore header
//

#ifndef PROJECT_BASE_SEM_HPP
#define PROJECT_BASE_SEM_HPP

#include "../lib/hw.h"
#include "../h/TCB.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/Scheduler.hpp"

class TCB;

class Sem
{
public:
    uint32 signal();
    uint32 wait();
    uint32 tryWait();
    uint32 close();
    static Sem* createSemaphore(uint64 init);

    bool isOpen() const { return open; }
    void setOpen(bool value) { open = value; }

private:
    Sem(uint64 init);
    int value;
    bool open = false;

    struct Elem {
        TCB* thread;
        struct Elem* next;
    };

    Elem* blocked = nullptr;
    void putBlocked(TCB* blockedThread);
    TCB* getBlocked();
};


#endif //PROJECT_BASE_SEM_HPP
