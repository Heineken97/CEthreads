
#include <unistd.h>
#include <CEthreads.h>
#include <futex.h>


/* Se crea un nuevo hilo con este puntero contenedor. El objetivo es suspender el nuevo
 * hilo hasta que sea programado por el operador.
 */
int CEthread_wrapper(void *thread_tcb)
{
	CEthread_private_t *new_tcb;
	new_tcb = (CEthread_private_t *) thread_tcb;

	DEBUG_PRINTF("Wrapper: will sleep on futex: %ld %d\n",
		     (unsigned long)__mythread_gettid(),
		     new_tcb->sched_futex.count);

	/*Suspender hasta que se despierte explícitamente*/
	futex_down(&new_tcb->sched_futex);

	DEBUG_PRINTF("Wrapper: futex value: %ld %d\n",
		     (unsigned long)new_tcb->tid, new_tcb->sched_futex.count);

	/*Nos han despertado. Llama a la función definida por el usuario */
	new_tcb->start_func(new_tcb->args);

	
    /*Idealmente no deberíamos llegar hasta aquí*/
	return 0;
}