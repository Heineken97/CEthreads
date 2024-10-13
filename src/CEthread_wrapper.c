#include "../include/CEthread_private.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/futex.h>
#include "../include/CEthreads.h"

void CEthread_wrapper(void *(*start_routine)(void*), void *arg) {
    CEthread_private_t *self_ptr = (CEthread_private_t *)__CEthread_selfptr();
    self_ptr->start_routine = start_routine;
    self_ptr->arg = arg;
    self_ptr->state = 1; // RUNNING
    start_routine(arg);
    CEthread_end();
 }
