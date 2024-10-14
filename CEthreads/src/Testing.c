#include "../include/CEthreads.h"
#include "../include/CEmutex.h"
#include <stdio.h>
#include <stddef.h>

void *my_start_routine(void *arg) {
    // Implementación para el hilo
    CEmutex_t *mutex = (CEmutex_t *)arg;
    CEmutex_lock(mutex);
    printf("Hola desde el hilo\n");
    CEmutex_unlock(mutex);
    return NULL;
}

int main() {
    CEmutex_t mutex;
    CEmutex_init(&mutex);
    CEthread_t thread;
    int tid = CEthread_create(my_start_routine, &mutex);
    thread.tid = tid;
    void *retval;
    CEthread_join(thread, &retval);
    CEmutex_destroy(&mutex);
    return 0;
}
