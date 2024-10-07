//Interfaz de la libreria de hilos CEthreads

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "futex.h"
#include <string.h>

#ifndef CETHREAD_H
#define CETHREAD_H

#define FALSE 0
#define TRUE 1

#define RUNNING 0
#define READY 	1 /*Listo*/
#define BLOCKED 2 /*Esperando para unirse a otro hilo*/
#define DEFUNCT 3 /*Muerto*/

typedef struct CEthread_attr {
  unsigned long stackSize;     /*Tamaño de pila que utilizará este hilo. El valor predeterminado es SIGSTKSZ(sysconf)*/
}CEthread_attr_t;

/*Manejo de hilos expuesto al usuario.*/
typedef struct CEthread {
  pid_t tid; /*El identificador del hilo(ID) */
}CEthread_t;

/*Estructura del bloque de control de subprocesos*/
typedef struct CEthread_private {

  pid_t tid; 				/*El ID del hilo*/
  int state; 				/*El estado en el que estará el hilo correspondiente*/
  void * (*start_func) (void *); 	/*El puntero de función a la función de subproceso que se ejecutará.*/
  void *args; 				/*Los argumentos que se pasarán a la función de subproceso.*/
  void *returnValue; 			/*El valor de retorno que devuelve el hilo.*/
  struct CEthread_private *blockedForJoin; 	/*Bloqueo de hilo en este hilo*/
  struct futex sched_futex;		/*Futex utilizado por el despachador para programar este hilo*/
  struct CEthread_private *prev, *next; 

}CEthread_private_t;

extern CEthread_private_t *CEthread_q_head; /* The pointer pointing to head node of the TCB queue */

/*
 * CEthread_self - identificador del hilo que esta corriendo actualmente.
 */
CEthread_t CEthread_self(void);

/*
 * CEthread_create - prepares context of new_thread_ID as start_func(arg),
 * attr es ignorado por ahora.
 * Threads are activated (run) according to the number of available LWPs
 * or are marked as ready.
 */
int CEthread_create(CEthread_t *new_thread_ID,
					CEthread_attr_t *attr,
					void * (*start_func)(void *),
					void *arg);

/*
 * CEthread_yield - switch from running thread to the next ready one
 */
int CEthread_yield(void);

/*
 * mythread_join - suspend calling thread if target_thread has not finished,
 * enqueue on the join Q of the target thread, then dispatch ready thread;
 * once target_thread finishes, it activates the calling thread / marks it
 * as ready.
 */
int CEthread_join(CEthread_t target_thread, void **status);

/*
 * mythread_exit - exit thread, awakes joiners on return
 * from thread_func and dequeue itself from run Q before dispatching run->next
 */
void CEhread_end(void *retval);

/* Private functions */

/* Get the tid of the current thread */
pid_t __CEthread_gettid();

/* Get the pointer to the private structure of this thread */
CEthread_private_t *__CEthread_selfptr();

/* Dispatcher - The thread scheduler */
int __CEthread_dispatcher(CEthread_private_t *);
void __CEthread_debug_futex_init();

/* Debugging */
extern char debug_msg[1000];
extern struct futex debug_futex;

#ifdef DEBUG
#define DEBUG_PRINTF(...) __CEthread_debug_futex_init(); \
			futex_down(&debug_futex); \
			sprintf(debug_msg, __VA_ARGS__); \
			write(1, debug_msg, strlen(debug_msg)); \
			futex_up(&debug_futex);
#else
#define DEBUG_PRINTF(...) do {} while(0);
#endif

#define ERROR_PRINTF(...) __CEthread_debug_futex_init(); \
			futex_down(&debug_futex); \
			sprintf(debug_msg, __VA_ARGS__); \
			write(1, debug_msg, strlen(debug_msg)); \
			futex_up(&debug_futex);

#define LOG_PRINTF(...) __CEthread_debug_futex_init(); \
			futex_down(&debug_futex); \
			sprintf(debug_msg, __VA_ARGS__); \
			write(1, debug_msg, strlen(debug_msg)); \
			futex_up(&debug_futex);

#endif