//
// C++ API for system calls header
//

#ifndef PROJECT_BASE_SYSCALL_CPP_HPP
#define PROJECT_BASE_SYSCALL_CPP_HPP

#include "syscall_c.hpp"

class Thread
{
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static int sleep (time_t);

    void join();

protected:
    Thread ();
    virtual void run () {}

private:
    thread_t myHandle;
    void (*body)(void*);
    void* arg;

    static void runWrapper(void* thread);
};

class Semaphore
{
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
    int timedWait (time_t);
    int tryWait();

private:
    sem_t myHandle;
};

class Console
{
public:
    static char getc ();
    static void putc (char);
};
#endif //PROJECT_BASE_SYSCALL_CPP_HPP
