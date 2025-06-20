#include "uthreads.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void thread_func1() {
    for (int i = 0; i < 5; ++i) {
        printf("Thread 1: iteration %d\n", i);
        usleep(100000); // 100 ms
    }
    uthread_exit();
}

void thread_func2() {
    for (int i = 0; i < 5; ++i) {
        printf("Thread 2: iteration %d\n", i);
        usleep(100000);
    }
    uthread_exit();
}

int main() {
    if (uthread_init(100) < 0) {
        fprintf(stderr, "Failed to initialize threads\n");
        return 1;
    }

    int tid1 = uthread_create(thread_func1);
    int tid2 = uthread_create(thread_func2);

    if (tid1 < 0 || tid2 < 0) {
        fprintf(stderr, "Thread creation failed\n");
        return 1;
    }

    while (1) {
        pause(); // Wait for signals (SIGALRM)
    }

    return 0;
}
