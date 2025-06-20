#ifndef UTHREADS_H
#define UTHREADS_H

#include <setjmp.h>
#include <stddef.h>

#define UTHREAD_MAX_THREADS 10
#define UTHREAD_STACK_BYTES (64 * 1024)

typedef void (*uthread_entry)(void);

// Initialize the user‐thread system
void uthread_system_init(void);

int uthread_init(unsigned int quantum_ms);
// Create a new thread; returns tid or –1
int  uthread_create(uthread_entry entry);

// Voluntarily yield to the next READY thread
void uthread_yield(void);

// Terminate the calling thread
void uthread_exit(void);

#endif // UTHREADS_H
