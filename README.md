![image](https://github.com/user-attachments/assets/2c1a3905-510e-4e94-b542-142e2cd93fd4)# User-Level Thread Library (C Implementation)

## Overview

This project implements a user-level thread manager in C, supporting lightweight preemptive multitasking with a simple API:

- `uthread_init(unsigned int quantum_ms)`: Initialize the system and timer (quantum in milliseconds)
- `uthread_create(void (*func)(void))`: Create a new thread running `func`
- `uthread_yield()`: Voluntarily yield the CPU to the next thread
- `uthread_exit()`: Terminate the current thread

Thread management is achieved using the POSIX `ucontext` API and preemption via a timer signal (`SIGALRM`).  
All scheduling and context switches occur entirely in user space (no pthreads).

---

## Platform & How to Run

> **Note:**  
> This code was developed and tested on **Ubuntu Linux inside Docker** (since we did not have access to a native Linux computer).  
> The code uses POSIX features (`ucontext.h`, `setitimer`) unavailable on Windows.

### How to Run (in Ubuntu or Docker)

1. **Clone or copy all project files.**
2. **Build:**
   ```sh
   gcc -Wall -Iinclude src/uthreads.c src/main.c -o uthreads_main
To compile and run the demo with sleep:
gcc -Wall -Iinclude src/uthreads.c src/uthreads.demo.c -o uthreads_demo
Run:
./uthreads_main
If running in Docker
Use an Ubuntu-based container:
docker run --rm -it -v "$PWD":/work -w /work ubuntu:22.04 bash
apt update && apt install build-essential -y

API Comments & Documentation
All public API functions are documented in uthreads.h:

// Initialize the user-thread system with preemptive scheduling
int uthread_init(unsigned int quantum_ms);

// Create a new thread; returns thread id or –1 on error
int uthread_create(void (*func)(void));

// Voluntarily yield to the next READY thread
void uthread_yield(void);

// Terminate the calling thread
void uthread_exit(void);

Each function in the implementation (uthreads.c) is also commented to explain its purpose and the main logic.

Sample Test Program

#include "uthreads.h"
#include <stdio.h>

void thread1(void) {
    for (int i = 0; i < 5; ++i) {
        printf("Thread 1: %d\n", i);
        uthread_yield();
    }
    uthread_exit();
}

void thread2(void) {
    for (int i = 0; i < 5; ++i) {
        printf("Thread 2: %d\n", i);
        uthread_yield();
    }
    uthread_exit();
}

int main(void) {
    uthread_init(100);  // 100ms time quantum

    uthread_create(thread1);
    uthread_create(thread2);

    uthread_yield();  // Start scheduling

    return 0;
}

Expected Output
When running, you should see output similar to:
Thread 1: 0
Thread 2: 0
Thread 1: 1
Thread 2: 1
Thread 1: 2
Thread 2: 2
Thread 1: 3
Thread 2: 3
Thread 1: 4
Thread 2: 4

This demonstrates correct context switching, with both threads alternating.


Results and Explanation
Thread creation, scheduling, and preemption were demonstrated with two threads, each printing and yielding.

The system preempts or switches threads after every quantum or explicit yield, showing round-robin scheduling.

The code was built and run in Ubuntu (via Docker) to provide the correct POSIX environment.

All public functions are well-commented, and the core logic is documented in the code for clarity and reproducibility.

Limitations & Assumptions
Assumes a POSIX environment (ucontext.h is not available on Windows).

No thread joining or synchronization primitives are implemented (for simplicity).

Each thread must eventually call uthread_exit() or return.

Submission Contents
src/uthreads.c — Implementation

include/uthreads.h — API header with comments

src/main.c — Demo/test program

(Optional) src/uthreads.demo.c — Another test/demo
