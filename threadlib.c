#include "threadlib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

static ucontext_t contexts[MAX_THREADS];
static ucontext_t main_context;
static int current_tid = -1;
static int total_threads = 0;
static struct itimerval timer;

static void scheduler(int signum);

int threadlib_init(unsigned int quantum_ms) {
    struct sigaction sa;
    sa.sa_handler = scheduler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGALRM, &sa, NULL) < 0) {
        perror("sigaction");
        return -1;
    }

    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = quantum_ms * 1000;
    timer.it_value = timer.it_interval;

    if (setitimer(ITIMER_REAL, &timer, NULL) < 0) {
        perror("setitimer");
        return -1;
    }

    return 0;
}

int thread_create(thread_fn fn) {
    if (total_threads >= MAX_THREADS)
        return -1;

    int tid = total_threads++;

    if (getcontext(&contexts[tid]) == -1) {
        perror("getcontext");
        return -1;
    }

    contexts[tid].uc_stack.ss_sp = malloc(STACK_SIZE);
    contexts[tid].uc_stack.ss_size = STACK_SIZE;
    contexts[tid].uc_link = &main_context;

    makecontext(&contexts[tid], fn, 0);

    return tid;
}

void thread_yield(void) {
    int next_tid = (current_tid + 1) % total_threads;
    int prev_tid = current_tid;
    current_tid = next_tid;

    if (prev_tid >= 0)
        swapcontext(&contexts[prev_tid], &contexts[current_tid]);
    else
        setcontext(&contexts[current_tid]);
}

void thread_exit(void) {
    // Basic: just go back to main
    setcontext(&main_context);
}

static void scheduler(int signum) {
    thread_yield();
}
