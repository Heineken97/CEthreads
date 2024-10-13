#include "../include/CEmutex.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/futex.h>

CEmutex_t mutex;

void CEmutex_init(CEmutex_t *mutex) {
    // Implementación para inicializar un mutex
    mutex->value = 0;
}

void CEmutex_destroy(CEmutex_t *mutex) {
    // Implementación para destruir un mutex
    mutex->value = 0;
}

void CEmutex_lock(CEmutex_t *mutex) {
    // Implementación para bloquear un mutex
    while (__sync_lock_test_and_set(&mutex->value, 1)) {
        syscall(SYS_futex, &mutex->value, FUTEX_WAIT);
    }
}

void CEmutex_unlock(CEmutex_t *mutex) {
    // Implementación para desbloquear un mutex
    __sync_lock_release(&mutex->value);
    syscall(SYS_futex, &mutex->value, FUTEX_WAKE);
}
