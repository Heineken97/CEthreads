#ifndef CETHREADS_H
#define CETHREADS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/futex.h>
#include "CEthread_private.h"
#include "CEthreads_q.h"

typedef struct {
    int tid;
} CEthread_t;

CEthread_private_t *__CEthread_selfptr();

int CEthread_create(void *(*start_routine)(void*), void *arg);
void CEthread_end();
int CEthread_join(CEthread_t target_thread, void **status);
void CEthread_yield();

#endif
