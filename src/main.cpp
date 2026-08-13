#include "../h/TCB.hpp"
#include "../h/Riscv.hpp"
#include "../h/syscall_cpp.hpp"
#include "../lib/console.h"
#include "../test/printing.hpp"

extern void userMain();

void userMainWrapper(void* arg) {
    userMain();
    ((Semaphore*)arg)->signal();
}

int main()
{
    
Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    Semaphore userSem(0);
    TCB* threads[2];
    printString("Threads create\n");
    thread_create(&threads[0], nullptr, nullptr); //main je vec u scheduleru
    TCB::running = threads[0];

    thread_create(&threads[1], userMainWrapper, &userSem);

    printString("UserMain started\n");

    userSem.wait();

    while (true);
    return 0;
}
