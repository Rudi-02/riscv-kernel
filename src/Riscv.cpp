//
// RISC-V specific operations implementation
//

#include "../h/Riscv.hpp"
#include "../lib/console.h"
#include "../h/TCB.hpp"
#include "../h/Sem.hpp"


void Riscv::popSppSpie() {
    mc_sstatus(SSTATUS_SPP);
    __asm volatile("csrw sepc, ra");
    __asm volatile("sret");
}

void Riscv::handleSupervisorTrap()
{
    uint64 scause = r_scause();
    if (scause == 0x8000000000000001UL)
    {
        mc_sip(SIP_SSIP);
    }
    else if (scause == 0x8000000000000009UL)
    {
        console_handler();
    }
    else if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL)
    {
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        uint64 codeOp;
        __asm__ volatile("ld %0, 80(x8)" : "=r" (codeOp));

        if(codeOp == 0x01)
        {
            uint64 arg1;
        __asm__ volatile("ld %0, 88(x8)" : "=r" (arg1));

            void* ret = MemoryAllocator::mem_alloc((uint64)arg1);

            __asm__ volatile("sd %0, 80(x8)" : : "r"(ret));
        }
        else if(codeOp == 0x02)
        {
            void* arg1;
        __asm__ volatile("ld %0, 88(x8)" : "=r" (arg1));

            int ret = MemoryAllocator::mem_free(arg1);

            __asm__ volatile("sd %0, 80(x8)" : : "r"(ret));
        }
        else if(codeOp == 0x11)
        {
            uint32 success = 0;
            uint32 fail = -1;
            TCB** handle;
            TCB::Body start_routine;
            void* arguments;
                uint64* stack;
            __asm__ volatile("ld %0, 88(x8)" : "=r" (handle));
            __asm__ volatile("ld %0, 96(x8)" : "=r" (start_routine));
            __asm__ volatile("ld %0, 104(x8)" : "=r" (arguments));
            __asm__ volatile("ld %0, 112(x8)" : "=r" (stack));

            *handle = TCB::createThread(start_routine,arguments,stack);

            if(*handle != nullptr) {
                __asm__ volatile("sd %0, 80(x8)" : : "r"(success));
            }
            else {
                __asm__ volatile("sd %0, 80(x8)" : : "r"(fail));
            }
        }
        else if(codeOp == 0x12)
        {
            TCB::exit();
            __asm__ volatile("sd %0, 80(x8)" : : "r"(-1));
        }
        else if(codeOp == 0x13)
        {
            TCB::dispatch();
        }
        else if(codeOp == 0x14)
        {
            TCB* other;
            __asm__ volatile("ld %0, 88(x8)" : "=r" (other));
            other->join();
        }
        else if(codeOp == 0x15)
        {
            TCB::joinAll();
        }
        else if(codeOp == 0x16)
        {
            TCB* child;
            __asm__ volatile("ld %0, 88(x8)" : "=r" (child));
            TCB::addChild(child);
        }
        else if(codeOp == 0x17)
        {
            int threadId = TCB::running->getId();
            TCB::dispatch();
            __asm__ volatile("sd %0, 80(x8)" : : "r"(threadId));
        }
        else if(codeOp == 0x18)
        {
            int max;
            __asm__ volatile("ld %0, 88(x8)" : "=r" (max));
            TCB::setMax(max);
        }
        else if(codeOp == 0x21)
        {
            uint32 success = 0;
            uint32 fail = -1;
            Sem** handle;
            uint64 init;
            __asm__ volatile("ld %0, 88(x8)" : "=r" (handle));
            __asm__ volatile("ld %0, 96(x8)" : "=r" (init));

            *handle = Sem::createSemaphore(init);

            if(*handle != nullptr) {
                __asm__ volatile("sd %0, 80(x8)" : : "r"(success));
            }
            else {
                __asm__ volatile("sd %0, 80(x8)" : : "r"(fail));
            }
        }
        else if(codeOp == 0x22)
        {
            Sem* handle;
            uint32 fail = -2;

            __asm__ volatile("ld %0, 88(x8)" : "=r" (handle));

            if(handle!=nullptr){
                int ret = handle->close();
                __asm__ volatile("sd %0, 80(x8)" : : "r"(ret));
            }
            else {
                __asm__ volatile("sd %0, 80(x8)" : : "r"(fail));
            }
        }
        else if(codeOp == 0x23)
        {
            Sem* id;
            uint32 fail = -2;

            __asm__ volatile("ld %0, 88(x8)" : "=r" (id));

            if(id!=nullptr){
                int ret = id->wait();
                __asm__ volatile("sd %0, 80(x8)" : : "r"(ret));
            }
            else {
                __asm__ volatile("sd %0, 80(x8)" : : "r"(fail));
            }
        }
        else if(codeOp == 0x24)
        {
            Sem* id;
            uint32 fail = -2;

            __asm__ volatile("ld %0, 88(x8)" : "=r" (id));

            if(id!=nullptr){
                int ret = id->signal();
                __asm__ volatile("sd %0, 80(x8)" : : "r"(ret));
            }
            else {
                __asm__ volatile("sd %0, 80(x8)" : : "r"(fail));
            }
        }
        else if(codeOp == 0x25)
        {

        }
        else if(codeOp == 0x26)
        {
            Sem* id;
            uint32 fail = -2;

            __asm__ volatile("ld %0, 88(x8)" : "=r" (id));

            if(id!=nullptr){
                int ret = id->tryWait();
                __asm__ volatile("sd %0, 80(x8)" : : "r"(ret));
            }
            else {
                __asm__ volatile("sd %0, 80(x8)" : : "r"(fail));
            }
        }
        else if(codeOp == 0x31)
        {

        }
        else if(codeOp == 0x41)
        {
			int ret = __getc();

                __asm__ volatile("sd %0, 80(x8)" : : "r"(ret));
        }
        else if(codeOp == 0x42)
        {
            char c;
            __asm__ volatile("ld %0, 88(x8)" : "=r" (c));
            __putc(c);
            
        }
        else
        {
            __putc('E');
            __putc('R');
            __putc('R');
            __putc('\n');
            while(true);
        }
        
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
}



//tcb
//class Semaphore;
//TCB* parent;            // pokazivač na roditelja (nullptr ako nema)
//int childCount;         // broj nezavršene dece
//Semaphore* childrenSem; // semafor na koji roditelj čeka u joinAll
//static void addChild(TCB* parent, TCB* child);
//static void joinAllChildren();
//#include "../h/Semaphore.hpp"
//this->parent = nullptr;
//this->childCount = 0;
//this->childrenSem = nullptr;
//void TCB::dispatch() {
//    TCB *old = running;
//    if (!old->isFinished()) {
//        Scheduler::put(old);
//    } else {
//        if (old->parent != nullptr) {
//            old->parent->childCount--;
//            if (old->parent->childCount == 0 && old->parent->childrenSem != nullptr) {
//                old->parent->childrenSem->sem_signal();
//            }
//        }
//    }
//    running = Scheduler::get();
//    contextSwitch(&old->context, &running->context);
//}
// void TCB::addChild(TCB* parent, TCB* child) {
//     child->parent = parent;
//     parent->childCount++;
//     if (parent->childrenSem == nullptr) {
//         Semaphore::createSemaphore(&parent->childrenSem, 0);
//     }
// }

// void TCB::joinAllChildren() {
//     if (running->childCount <= 0) return;
//     running->childrenSem->sem_wait();
// }
//syscall_c
// void thread_add_child(thread_t child) {
//     if (child == nullptr) return;
//     do_syscall(0x16, (uint64)child);
// }

// void thread_join_all() {
//     do_syscall(0x17);
// }
//RISCV.cpp
// case 0x16: {
//     TCB *child = (TCB*)arg1;
//     TCB::addChild(TCB::running, child);
//     break;
// }
// case 0x17: {
//     TCB::joinAllChildren();
//     break;
// }
// void Thread::addChild(Thread* child) {
//     thread_add_child(child->getMyHandle());
// }

// void Thread::joinAll() {
//     thread_join_all();
// }


//syscall_c.cpp
// void thread_join(thread_t* handle)
// {
//     uint64 number = 0x03; // Mogao je bilokoji, slobodan, ulaz

//     __asm__ volatile("mv a1, %0" : : "r" (handle));
//     __asm__ volatile("mv a0, %0" : : "r" (number));

//     __asm__ volatile("ecall");
// }

//TCB.cpp
// void TCB::join(thread_t* handle)
// {
// 	while(!(*handle)->isFinished())
// 		TCB::dispatch();
// }


//riscv.cpp
// case 0x03: // Mogao je bilokoji, slobodan, ulaz
// {
// 	thread_t *handle;
// 	__asm__ volatile ("mv %0, a1" : "=r" (handle));

// 	TCB::join(handle);

//     break;
// }

//syscall_cpp.cpp
// void Thread::join()
// {
// 	if (myHandle)
// 		thread_join(&myHandle);
// }