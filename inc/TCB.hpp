//
// Task Control Block header
//

#ifndef PROJECT_BASE_TCB_HPP
#define PROJECT_BASE_TCB_HPP

#include "../lib/hw.h"
#include "../lib/mem.h"
#include "../h/Sem.hpp"
#include "../h/Scheduler.hpp"
#include "../h/Riscv.hpp"


class Sem;

class TCB
{
public:
    using Body = void (*)(void*);

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    bool isBlocked() const { return blocked; }

    void setBlocked(bool value) { blocked = value; }

    static TCB* createThread(Body body, void* arguments, uint64* stack);

    static void dispatch();

    static void exit();

    void join();

    static TCB *running;

    ~TCB() { if (stack) delete[] stack; }

private:

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Context context;
    uint64* stack;
    Body body;
    bool finished;
    bool blocked;
    void* arguments;
    Sem* semaphore;

    static uint64 timeSliceCounter;
    static void contextSwitch(Context *oldContext, Context *newContext);
    static void threadWrapper();

    TCB(Body body, void* arguments, uint64* stack);


};

#endif //PROJECT_BASE_TCB_HPP
