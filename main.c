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
    uthread_init(100);  // or whatever quantum you want in milliseconds


    if (uthread_create(thread1) < 0) return 1;
    if (uthread_create(thread2) < 0) return 1;

    // start scheduling by yielding from main
    uthread_yield();

    return 0;
}
