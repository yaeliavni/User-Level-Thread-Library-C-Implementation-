#include "uthreads.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>


#define STACK_SIZE 8192
#define MAX_THREADS 10

typedef enum { READY, RUNNING, TERMINATED } thread_state;

typedef struct {
    ucontext_t context;
    thread_state state;
    void *stack;
} Thread;

static Thread threads[MAX_THREADS];
static int current = -1;
static int total_threads = 0;
static struct itimerval timer;
static ucontext_t main_context;

static void scheduler(int sig);

int uthread_init(unsigned int quantum_ms) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = scheduler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGALRM, &sa, NULL) < 0) {
        perror("sigaction");
        return -1;
    }

    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = quantum_ms * 1000;
    timer.it_interval = timer.it_value;

    if (setitimer(ITIMER_REAL, &timer, NULL) < 0) {
        perror("setitimer");
        return -1;
    }

    return 0;
}

int uthread_create(void (*func)(void)) {
    if (total_threads >= MAX_THREADS)
        return -1;

    int tid = total_threads++;
    getcontext(&threads[tid].context);
    threads[tid].stack = malloc(STACK_SIZE);
    threads[tid].context.uc_stack.ss_sp = threads[tid].stack;
    threads[tid].context.uc_stack.ss_size = STACK_SIZE;
    threads[tid].context.uc_link = &main_context;
    threads[tid].state = READY;

    makecontext(&threads[tid].context, func, 0);
    return tid;
}

void uthread_yield() {
    int prev = current;

    // Look for the next READY thread
    int found = 0;
    for (int i = 0; i < total_threads; ++i) {
        int next = (current + 1 + i) % total_threads;
        if (threads[next].state == READY) {
            current = next;
            found = 1;
            break;
        }
    }

    if (!found) {
        // No READY threads left — return to main
        setcontext(&main_context);
        return;
    }

    if (prev != -1 && threads[prev].state == RUNNING) {
        threads[prev].state = READY;
        threads[current].state = RUNNING;
        swapcontext(&threads[prev].context, &threads[current].context);
    } else {
        threads[current].state = RUNNING;
        setcontext(&threads[current].context);
    }
}




void uthread_exit() {
    threads[current].state = TERMINATED;
    free(threads[current].stack);
    setcontext(&main_context);  // safely return to main
}


static void scheduler(int sig) {
    uthread_yield();
}
