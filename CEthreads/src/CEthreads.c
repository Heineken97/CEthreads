#include "../include/CEthreads.h"
#include "../include/CEthread_private.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/futex.h>
#include "../include/CEthreads_q.h"
extern CEthread_private_t *CEthread_q_head;
// Función para obtener el puntero al hilo actual
CEthread_private_t *__CEthread_selfptr() {
    static CEthread_private_t main_thread;
    static int initialized = 0;
    if (!initialized) {
        main_thread.tid = syscall(SYS_gettid);
        main_thread.start_routine = NULL;
        main_thread.arg = NULL;
        main_thread.state = 1; // RUNNING
        main_thread.blockedForJoin = NULL;
        main_thread.next = NULL;
        main_thread.returnValue = NULL;
        initialized = 1;
    }
    return &main_thread;
}

// Función para crear un nuevo hilo
int CEthread_create(void *(*start_routine)(void*), void *arg) {
    // Implementación para crear un nuevo hilo
    pid_t pid = syscall(SYS_clone, 0x00000011, 0);
    if (pid < 0) {
        return -1;
    } else if (pid == 0) {
        // Hilo hijo
        CEthread_private_t *self_ptr = __CEthread_selfptr();
        self_ptr->start_routine = start_routine;
        self_ptr->arg = arg;
        self_ptr->state = 1; // RUNNING
        void *retval = start_routine(arg);
        self_ptr->returnValue = retval;
        CEthread_end();
    } else {
        // Hilo padre
        CEthread_private_t *new_thread = malloc(sizeof(CEthread_private_t));
        new_thread->tid = pid;
        new_thread->start_routine = start_routine;
        new_thread->arg = arg;
        new_thread->state = 1; // RUNNING
        new_thread->returnValue = NULL; // Initialize returnValue
        new_thread->next = CEthread_q_head;
        CEthread_q_head = new_thread;
        return pid;
    }
}
// Función para finalizar un hilo
void CEthread_end() {
    // Implementación para finalizar un hilo
    CEthread_private_t *self_ptr = __CEthread_selfptr();
    self_ptr->state = 2; // DEFUNCT
    syscall(SYS_exit, 0);
}
// Función para unir un hilo
int CEthread_join(CEthread_t target_thread, void **status) {
    CEthread_private_t *target, *self_ptr;
    self_ptr = __CEthread_selfptr();
    target = (CEthread_private_t *)CEthread_q_search(target_thread.tid);
    if (target->state == 2) { // DEFUNCT
        *status = target->returnValue;
        if (target == CEthread_q_head) {
            CEthread_q_head = target->next;
        } else {
            CEthread_private_t *current = CEthread_q_head;
            while (current->next != target) {
                current = current->next;
            }
            current->next = target->next;
        }
        free(target);
        return 0;
    } else {
        self_ptr->state = 3; // BLOCKED
        target->blockedForJoin = (struct CEthread_private *)self_ptr;
        syscall(SYS_futex, &target->tid, FUTEX_WAIT);
        *status = target->returnValue;
        if (target == CEthread_q_head) {
            CEthread_q_head = target->next;
        } else {
            CEthread_private_t *current = CEthread_q_head;
            while (current->next != target) {
                current = current->next;
            }
            current->next = target->next;
        }
        free(target);
        return 0;
    }
}

// Función para yield
void CEthread_yield() {
    // Implementación para yield
    syscall(SYS_sched_yield);
}
