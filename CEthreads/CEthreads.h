#ifndef CETHREADS_H
#define CETHREADS_H

#include <stdint.h>

#define MAX_THREADS 64
#define MAX_WAITERS 64 // Número máximo de hilos en espera para un mutex

typedef enum {
    THREAD_NEW,
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_BLOCKED,
    THREAD_TERMINATED
} ThreadState;

typedef struct CEthread {
    uint32_t id;                  // Identificador del hilo
    ThreadState state;           // Estado del hilo
    uint32_t priority;           // Prioridad del hilo
    void *cpu_info;              // Información de CPU (contexto)
    struct CEthread *parent;      // Puntero al hilo padre
    struct CEthread *children[MAX_THREADS]; // Hilos hijos
    int child_count;             // Contador de hilos hijos
    void (*func)(void *);        // Función que ejecutará el hilo
    void *arg;                   // Argumentos para la función
} CEthread;

typedef struct CEmutex {
    int locked;                  // Estado del mutex
    CEthread *owner;             // Hilo que posee el mutex
    CEthread *waiters[MAX_WAITERS]; // Hilos en espera
    int waiter_count;            // Contador de hilos en espera
} CEmutex;

int CEthread_create(CEthread *t, void (*func)(void *), void *arg);
void CEthread_end(void);
int CEthread_join(CEthread *t);
int CEmutex_init(CEmutex *m);
int CEmutex_destroy(CEmutex *m);
int CEmutex_lock(CEmutex *m);
int CEmutex_unlock(CEmutex *m);

extern CEthread *current_thread; // Variable global para el hilo actual
#endif // CETHREADS_H
