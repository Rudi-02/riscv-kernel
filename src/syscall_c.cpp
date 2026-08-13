//
// C API for system calls implementation
//

#include "../h/syscall_c.hpp"
#include "../lib/mem.h"

void* mem_alloc(size_t size)
{
	size_t blocksNumber = ((size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);

    __asm volatile("mv a1, %0" : : "r"(blocksNumber));
    __asm volatile("li a0, 0x01");
    __asm volatile("ecall");

    void* returnValue;
    __asm volatile("mv %0, a0" : "=r"(returnValue));
    return returnValue;
}


uint32 mem_free(void* ptr)
{
	__asm volatile("mv a1, %0" : : "r"(ptr));
	__asm volatile("li a0, 0x02");
	__asm volatile("ecall");

	uint32 returnValue;
    __asm volatile("mv %0, a0" : "=r"(returnValue));
	return returnValue;
}


uint32 thread_create(thread_t* handle, void(*startRoutine)(void*), void* arg)
{
	uint8* stack = (uint8*) mem_alloc(DEFAULT_STACK_SIZE);
	__asm volatile("mv a4, %0" : : "r"(stack));
	__asm volatile("mv a3, %0" : : "r"(arg));
	__asm volatile("mv a2, %0" : : "r"(startRoutine));
	__asm volatile("mv a1, %0" : : "r"(handle));

	__asm volatile("li a0, 0x11");
	__asm volatile("ecall");

	uint32 returnValue;
	__asm volatile("mv %0, a0" : "=r"(returnValue));
	return returnValue;
}


uint32 thread_exit()
{
	__asm volatile("li a0, 0x12");
	__asm volatile("ecall");

	uint32 returnValue;
	__asm volatile("mv %0, a0" : "=r"(returnValue));
	return returnValue;
}


void thread_dispatch()
{
	__asm volatile("li a0, 0x13");
	__asm volatile("ecall");
}


void thread_join(thread_t handle)
{
    __asm volatile("mv a1, %0" : : "r"(handle));
	__asm volatile("li a0, 0x14");
	__asm volatile("ecall");
}


//=========================
void thread_join_all()
{
	__asm volatile("li a0, 0x15");
	__asm volatile("ecall");
}
//=========================
void thread_add_child(thread_t handle)
{
	__asm volatile("mv a1, %0" : : "r"(handle));
	__asm volatile("li a0, 0x16");
	__asm volatile("ecall");
}

int getThreadId()
{
	__asm volatile("li a0, 0x17");
	__asm volatile("ecall");

	int returnValue;
	__asm volatile("mv %0, a0" : "=r"(returnValue));
	return returnValue;
}

void setMax(int max)
{
	__asm volatile("mv a1, %0" : : "r"(max));
	__asm volatile("li a0, 0x18");
	__asm volatile("ecall");
}


uint32 sem_open(sem_t* handle, uint64 init)
{
	__asm volatile("mv a2, %0" : : "r"(init));
	__asm volatile("mv a1, %0" : : "r"(handle));
	__asm volatile("li a0, 0x21");
	__asm volatile("ecall");

	uint32 returnValue;
	__asm volatile("mv %0, a0" : "=r"(returnValue));
	return returnValue;
}


uint32 sem_close(sem_t handle)
{
	__asm volatile("mv a1, %0" : : "r"(handle));
	__asm volatile("li a0, 0x22");
	__asm volatile("ecall");

	uint32 returnValue;
	__asm volatile("mv %0, a0" : "=r"(returnValue));
	return returnValue;
}


uint32 sem_wait(sem_t id)
{
	__asm volatile("mv a1, %0" : : "r"(id));
	__asm volatile("li a0, 0x23");
	__asm volatile("ecall");

	uint32 returnValue;
	__asm volatile("mv %0, a0" : "=r"(returnValue));
	return returnValue;
}


uint32 sem_signal(sem_t id)
{
	__asm volatile("mv a1, %0" : : "r"(id));
	__asm volatile("li a0, 0x24");
	__asm volatile("ecall");

	uint32 returnValue;
	__asm volatile("mv %0, a0" : "=r"(returnValue));
	return returnValue;
}


uint32 sem_trywait(sem_t id)
{
	__asm volatile("mv a1, %0" : : "r"(id));
	__asm volatile("li a0, 0x26");
	__asm volatile("ecall");

	uint32 returnValue;
	__asm volatile("mv %0, a0" : "=r"(returnValue));
	return returnValue;
}

uint32 sem_timedwait(sem_t id, time_t time)
{
	return 0;
}


uint32 time_sleep(time_t time)
{
	return 0;
}


char getc()
{
	char c;
	__asm volatile("li a0, 0x41");
	__asm volatile("ecall");
	__asm volatile("mv %0, a0" : "=r"(c));
	return c;
}

void putc(char c)
{
	__asm volatile("mv a1, %0" : : "r"(c));
	__asm volatile("li a0, 0x42");
	__asm volatile("ecall");
}

