#ifndef THREADLIB_H
#define THREADLIB_H

#include <ucontext.h>
#include <signal.h>

#define MAX_THREADS 10
#define STACK_SIZE (64 * 1024)  /* 64 KB per thread */

typedef void (*thread_fn)(void);

/**
 * Initialize the thread system (quantum in milliseconds).
 */
int threadlib_init(unsigned int quantum_ms);

/**
 * Create a new thread running `fn`. Returns thread ID or -1.
 */
int thread_create(thread_fn fn);

/**
 * Yield to the next thread.
 */
void thread_yield(void);

/**
 * Exit the current thread.
 */
void thread_exit(void);

#endif // THREADLIB_H
