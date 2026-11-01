# riscv-kernel

A preemptive multithreaded kernel for **RISC-V (supervisor mode)**, written in C++ with
assembly-level trap handling. It provides threads, semaphores, a heap allocator and console
I/O on bare metal, and exposes them through both a C and a C++ API.

The kernel runs user code in **U-mode**: on start, each thread's first `sret` clears `SPP`,
drops privilege, and from that point on every kernel entry happens through a trap.

---

## Features

| Area | What is implemented |
|---|---|
| **Threads** | `create`, `exit`, `join`, per-thread stack allocation, `dispatch` |
| **Preemption** | Time-sliced, driven by the supervisor timer interrupt |
| **Scheduler** | FIFO round-robin ready queue |
| **Semaphores** | `wait`, `signal`, `tryWait`, `close`; blocked threads kept in a FIFO queue |
| **Memory** | First-fit allocator over the kernel heap, with block header, used/free lists and coalescing on free |
| **Traps** | `ecall` syscall dispatch, timer interrupt, console (external) interrupt |
| **Console** | `getc` / `putc` through the trap handler |
| **APIs** | C (`syscall_c`) and C++ object-oriented wrapper (`Thread`, `Semaphore`) |

---

## Architecture

```
  user programs (test/)
        │
        │  Thread / Semaphore  (C++ API, syscall_cpp)
        ▼
  syscall_c  ── ecall ──►  supervisorTrap.S        (saves x0–x31, calls C++)
                                │
                                ▼
                     Riscv::handleSupervisorTrap()
                                │
              ┌─────────────────┼──────────────────┐
              ▼                 ▼                  ▼
        TCB / Scheduler      Sem              MemoryAllocator
              │
              ▼
     contextSwitch.S  (ra, sp)
```

**Trap entry.** `supervisorTrap.S` pushes all 32 general-purpose registers onto the stack,
calls the C++ handler and restores them before `sret`. Because the register file is saved as a
flat frame, the handler reads syscall arguments by offset from the saved `s0` — for example
`a0` (the syscall code) is `80(s0)` and `a1` is `88(s0)`.

**Dispatch.** `scause` is decoded for three cases: supervisor software interrupt (timer, which
drives preemption), supervisor external interrupt (console), and `ecall` from U- or S-mode.

**Context switch.** `TCB::contextSwitch` stores only `ra` and `sp` — enough because switching
always happens at a well-defined point inside `TCB::dispatch`.

---

## Syscall table

Arguments are passed in `a0`–`a4`; the return value is written back into the saved `a0`.

| Code | Call | Arguments |
|---|---|---|
| `0x01` | `mem_alloc` | size (blocks) |
| `0x02` | `mem_free` | pointer |
| `0x11` | `thread_create` | handle, body, arguments, stack |
| `0x12` | `thread_exit` | — |
| `0x13` | `thread_dispatch` | — |
| `0x14` | `thread_join` | handle |
| `0x15` | `thread_join_all` | — |
| `0x16` | `thread_add_child` | child |
| `0x17` | `thread_get_id` | — |
| `0x18` | `set_max_threads` | max |
| `0x21` | `sem_open` | handle, initial value |
| `0x22` | `sem_close` | handle |
| `0x23` | `sem_wait` | handle |
| `0x24` | `sem_signal` | handle |
| `0x26` | `sem_trywait` | handle |
| `0x41` | `getc` | — |
| `0x42` | `putc` | char |

---

## Project structure

```
inc/                    kernel headers (TCB, Scheduler, Sem, Riscv, MemoryAllocator, syscalls)
src/
  main.cpp              kernel entry: sets stvec, starts the first threads
  Riscv.cpp             trap handler and syscall dispatch
  supervisorTrap.S      trap entry/exit (saves and restores x0–x31)
  contextSwitch.S       thread context switch (ra, sp)
  TCB.cpp               thread control block, dispatch, exit, join
  Scheduler.cpp         FIFO ready queue
  Sem.cpp               counting semaphore with blocked-thread queue
  MemoryAllocator.cpp   first-fit heap allocator
  syscall_c.cpp         C API (ecall wrappers)
  syscall_cpp.cpp       C++ API on top of the C one
  _new.cpp              global operator new/delete routed to the kernel allocator
```

---

## Building and running

This repository contains **only the kernel sources**. The build expects a `lib/` and `test/`
directory supplied with the assignment, which provide:

- `lib/hw.h`, `lib/mem.h` — hardware register access and heap boundaries
- `lib/console.h` — polling console driver
- `test/printing.hpp`, `test/userMain()` — test programs and the entry point the kernel calls

The kernel also needs a RISC-V toolchain and the linker script from that scaffold. A typical
build looks like:

```bash
riscv64-unknown-elf-g++ -march=rv64ima -mabi=lp64 -nostdlib -nostartfiles \
    -T linker.ld src/*.cpp src/*.S -I inc -o kernel.elf
```

Exact flags, the linker script and the emulator invocation are defined by the course Makefile —
adjust them to your toolchain. `main.cpp` calls `userMain()`, so a test entry point is required
for linking.

---

## Design notes

**Memory allocator.** The heap [`HEAP_START_ADDR`, `HEAP_END_ADDR`] is described by two
address-sorted linked lists: free blocks and used blocks. Allocation is first-fit over the free
list; when a block is larger than requested, the remainder is split off and reinserted. Freeing
returns the block to the free list and coalesces it with the following block if they are
adjacent, so fragmented allocations do not accumulate.

**Scheduler.** A singly linked FIFO queue. `dispatch()` puts the current thread at the tail
unless it is finished or blocked, then takes the head and switches to it. Semaphores and
`join` implement blocking by removing the thread from the ready queue and adding it to the
semaphore's blocked list; `signal` moves the first blocked thread back into the ready queue.

**Semaphores.** A counting semaphore with the invariant `value`: `wait` decrements and blocks
when the result goes negative, `signal` increments and unblocks one thread when it stays
non-positive. `close` marks the semaphore closed and releases every blocked thread.

---

## Limitations and next steps

- **Minimal thread context.** Only `ra` and `sp` are saved, which is sufficient because
  switching happens inside `dispatch()`. A fully general preemption scheme would also need the
  callee-saved registers.
- **Unvalidated user pointers.** Syscalls trust pointers received from U-mode instead of
  checking them against the user address space.
- **Not implemented:** `sem_wait_n` (`0x25`) and `time_sleep` (`0x31`) are declared in the
  syscall table but currently have empty handlers.
- **Static kernel structures.** Scheduler and semaphore nodes are allocated from the same heap
  as user memory; a separate kernel pool would isolate them from user pressure.
- **No SMP support.** All per-CPU state (`running`, scheduler head, allocator lists) is global.

---

## Context

Written as a course project on operating systems for RISC-V, implementing the kernel from the
trap vector up: manual register save/restore, `ecall` dispatch, cooperative-to-preemptive
threading, and a heap allocator on bare metal.
