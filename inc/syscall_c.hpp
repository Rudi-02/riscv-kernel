//
// C API for system calls header
//

#ifndef PROJECT_BASE_SYSCALL_C_HPP
#define PROJECT_BASE_SYSCALL_C_HPP

#include "../lib/hw.h"
#include "../h/TCB.hpp"
#include "../h/Sem.hpp"

class TCB;
typedef TCB* thread_t;
class Sem;
typedef Sem* sem_t;

void* mem_alloc(size_t size);
uint32 mem_free(void* ptr);

uint32 thread_create(thread_t* handle, void(*startRoutine)(void*), void* arg);
uint32 thread_exit();
void thread_dispatch();
void thread_join(thread_t handle);

uint32 sem_open(sem_t* handle, uint64 init);
uint32 sem_close(sem_t handle);
uint32 sem_wait(sem_t id);
uint32 sem_signal(sem_t id);
uint32 sem_trywait(sem_t id);
uint32 sem_timedwait(sem_t id, time_t time);

uint32 time_sleep(time_t time);

char getc();
void putc(char c);

#endif //PROJECT_BASE_SYSCALL_C_HPP
