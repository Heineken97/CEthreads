#ifndef CETHREAD_PRIVATE_H
#define CETHREAD_PRIVATE_H

typedef struct CEthread_private {
    int tid;
    void *(*start_routine)(void *);
    void *arg;
    int state; // 0: READY, 1: RUNNING, 2: DEFUNCT, 3: BLOCKED
    struct CEthread_private *blockedForJoin;
    struct CEthread_private *next;
    void *returnValue;
} CEthread_private_t;

#endif
