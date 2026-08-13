//
// C++ API for system calls implementation
//

#include "../h/syscall_cpp.hpp"
#include "../h/MemoryAllocator.hpp"
#include "syscall_cpp.hpp"

Thread::Thread (void (*body)(void*), void* arg)
{
        this->body = body;
        this->arg = arg;
        this->myHandle = nullptr;
}


Thread:: ~Thread ()
{
}


int Thread::start ()
{
        return thread_create(&myHandle, body, arg);
}


void Thread::dispatch ()
{
        thread_dispatch();
}


int Thread::sleep (time_t time)
{
        return time_sleep(time);
}


int Thread::getThreadId ()
{
        return ::getThreadId();
}

void Thread::SetMaximumThreads(int max)
{
        setMax(max);
}

Thread::Thread ()
{
        this->body = runWrapper;
        this->arg = this;
        this->myHandle = nullptr;
}


void Thread::runWrapper(void* thread)
{
        if(thread) {
                ((Thread*)thread)->run();
        }
}


void Thread::join ()
{
    thread_join(myHandle);
}


void Thread::joinAll ()
{
    thread_join_all();
}


void Thread::addChild(Thread* child)
{
    thread_add_child(child->myHandle);
}


Semaphore::Semaphore (unsigned init)
{
        sem_open(&myHandle, init);
}


Semaphore::~Semaphore ()
{
        sem_close(myHandle);
}


int Semaphore::wait ()
{
        return sem_wait(myHandle);
}


int Semaphore::signal ()
{
        return sem_signal(myHandle);
}


int Semaphore::timedWait (time_t time)
{
        return sem_timedwait(myHandle, time);
}


int Semaphore::tryWait()
{
        return sem_trywait(myHandle);
}

char Console::getc() {
        char c = ::getc();
        return c;
}

void Console::putc(char c) {
        ::putc(c);
}

