#include "CEthreads.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

static CEthread threads[MAX_THREADS];
static int thread_count = 0;
CEthread *current_thread; // Inicializa el hilo actual

static void thread_wrapper(int id) {
    current_thread = &threads[id]; // Establecer el hilo actual
    threads[id].state = THREAD_RUNNING;
    threads[id].func(threads[id].arg);
    threads[id].state = THREAD_TERMINATED;
    CEthread_end();
}

int CEthread_create(CEthread *t, void (*func)(void *), void *arg) {
    if (thread_count >= MAX_THREADS) {
        return -1; // Límite de hilos alcanzado
    }

    t->id = thread_count;
    t->state = THREAD_NEW;
    t->priority = 1; // Establecer una prioridad por defecto
    t->func = func;
    t->arg = arg;
    t->parent = NULL;
    t->child_count = 0;

    // Crear un nuevo proceso (hilo)
    pid_t pid = fork();
    if (pid < 0) {
        return -1; // Error al crear el hilo
    } else if (pid == 0) {
        thread_wrapper(t->id); // Ejecutar la función del hilo
        exit(0); // Terminar el hilo
    }

    threads[thread_count++] = *t; // Agregar el hilo a la tabla
    t->state = THREAD_READY;
    return 0; // Éxito
}

void CEthread_end(void) {
    exit(0);
}

int CEthread_join(CEthread *t) {
    return wait(NULL); // Esperar a que termine el proceso hijo
}

int CEmutex_init(CEmutex *m) {
    m->locked = 0; // Inicializar el mutex como desbloqueado
    m->owner = NULL; // Sin propietario
    m->waiter_count = 0; // Inicializar contadores de espera
    return 0;
}

int CEmutex_destroy(CEmutex *m) {
    // Eliminar el mutex (implementación pendiente)
    return 0;
}

int CEmutex_lock(CEmutex *m) {
    while (__sync_lock_test_and_set(&m->locked, 1)) {
        // Bloquear el hilo y agregarlo a la lista de espera
        if (m->waiter_count < MAX_WAITERS) {
            m->waiters[m->waiter_count++] = current_thread; // Obtener el hilo actual
            current_thread->state = THREAD_BLOCKED; // Cambiar el estado del hilo actual a bloqueado
        }
        pause(); // Pone el hilo en un estado de espera
    }
    // Guardar el hilo propietario
    m->owner = current_thread; // Obtener el hilo actual
    return 0; // Éxito
}

int CEmutex_unlock(CEmutex *m) {
    if (!m->locked || m->owner != current_thread) {
        return -1; // No se puede desbloquear
    }
    m->locked = 0; // Desbloquear
    m->owner = NULL; // Liberar el propietario

    // Desbloquear el siguiente hilo en espera, si lo hay
    if (m->waiter_count > 0) {
        CEthread *next_thread = m->waiters[0];
        for (int i = 0; i < m->waiter_count - 1; i++) {
            m->waiters[i] = m->waiters[i + 1]; // Mover la lista
        }
        m->waiter_count--; // Reducir el contador
        next_thread->state = THREAD_READY; // Cambiar el estado del hilo a listo
    }
    return 0; // Éxito
}
