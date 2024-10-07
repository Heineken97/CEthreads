#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include "../include/futex.h"
#include "../include/CEthreads.h"
#include "../include/CEthreads_q.h"
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/sched.h>

/*Codigo para solucionar error con sigset_t*/
/*Inicia aqui*/
/*Termina aqui*/

/*---------------------------------------------------------CE_thread_create()---------------------------------------------------------------------------------*/

#define CLONE_SIGNAL            (CLONE_SIGHAND | CLONE_THREAD)
int CEthread_wrapper(void *);
void *CEthread_idle(void *);

/*El puntero externo global definido en CEthread.h apunta a la cabeza del nodo que
esta en cola en el tcb (Thread Control Blocks)*/
CEthread_private_t *CEthread_q_head;

/*El puntero global el cual apunta al tcb del hilo principal*/
CEthread_private_t *main_tcb;

/*Esta estructura es utilizada para poder hacer referencia al hilo inactivo tcb*/
CEthread_t idle_u_tcb;

/*  Futex global el cual usamos cuando la operacion necesita ser realizada la cual a su vez es 
    invocada por la llamada de rendimiento. 
    El futex global es necesario para evitar las condiciones de carrera que puedan ser 
    invocadas por múltiples subprocesos durante el rendimiento.
*/
extern struct futex gfutex;

/* Cuando la primera llamada a CEthread_create es invocada creamos el tcb correspondiente
   al hilo principal y los inactivos. La siguiente funcion agrega el tcb para el hilo principal
   delante de la cola.
*/
static int __CEthread_add_main_tcb()
{
	DEBUG_PRINTF("add_main_tcb: Creating node for Main thread \n");
	main_tcb = (CEthread_private_t *) malloc(sizeof(CEthread_private_t));
	if (main_tcb == NULL) {
		ERROR_PRINTF("add_main_tcb: Error allocating memory for main node\n");
		return -ENOMEM;
	}

	main_tcb->start_func = NULL;
	main_tcb->args = NULL;
	main_tcb->state = READY;
	main_tcb->returnValue = NULL;
	main_tcb->blockedForJoin = NULL;

	/*Obtiene el identificador de hilo tid y lo pone en su tcb correspondiente.*/
	main_tcb->tid = __CEthread_gettid();

	/*Inicializa el futex en cero*/
	futex_init(&main_tcb->sched_futex, 1);

	/*Lo pone en la cola de bloques de hilos*/
	CEthread_q_add(main_tcb);
	return 0;
}

/* CEthread_create.
   Esto crea un contexto de proceso compartido mediante la llamada al sistema de clonación.
   Pasamos el puntero a una función contenedora (CEthread_wrapper.c) que a su vez 
   configura el entorno del hilo y luego llama a la función del hilo.
   El argumento CEthread_attr_t puede especificar opcionalmente el tamaño de la pila que se utilizará.
   el hilo recién creado.
 */
int CEthread_create(CEthread_t * new_thread_ID,
		    CEthread_attr_t * attr,
		    void *(*start_func) (void *), void *arg)
{

	/*Puntero a la pila utilizada por el proceso hijo que se creará mediante clonación más adelante*/
	char *child_stack;

	unsigned long stackSize;
	CEthread_private_t *new_node;
	pid_t tid;
	int retval;

	/*Banderas que se pasarán a la llamada al sistema de clonación*/
	int clone_flags = (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGNAL
			   | CLONE_PARENT_SETTID
			   | CLONE_CHILD_CLEARTID | CLONE_SYSVSEM);
    
	if (CEthread_q_head == NULL) {
		/*Primera llamada CEthread_create. Configura Q primero con nodos tcb para el hilo principal.*/
		retval = __CEthread_add_main_tcb();
		if (retval != 0)
			return retval;

		/*Inicializa el futex global*/
		futex_init(&gfutex, 1);

		/*Crea el nodo para el subproceso inactivo con una llamada recursiva a CEthread_create().*/
		DEBUG_PRINTF("create: creating node for Idle thread \n");
		CEthread_create(&idle_u_tcb, NULL, CEthread_idle, NULL);
	}

	new_node = (CEthread_private_t *) malloc(sizeof(CEthread_private_t));
	if (new_node == NULL) {
		ERROR_PRINTF("Cannot allocate memory for node\n");
		return -ENOMEM;
	}

	/* Si no se proporciona el argumento de tamaño de pila, utiliza SIGSTKSZ(sysconf) como tamaño de pila predeterminado. 
       De lo contrario, extrae el argumento del tamaño de pila.
	 */
	if (attr == NULL)
		stackSize = sysconf;
	else
		stackSize = attr->stackSize;

	/*posix_memalign alinea la memoria asignada en un límite de 64 bits.*/
	if (posix_memalign((void **)&child_stack, 8, stackSize)) {
		ERROR_PRINTF("posix_memalign failed! \n");
		return -ENOMEM;
	}

	/*Dejamos espacio para una invocación en la base de la pila.*/
	child_stack = child_stack + stackSize - sizeof(void*);

	/*Guarde el puntero thread_fun y el puntero a los argumentos en el tcb.*/
	new_node->start_func = start_func;
	new_node->args = arg;
	/*Establece el estado como LISTO - LISTO en Q, esperando ser programado.*/
	new_node->state = READY;

	new_node->returnValue = NULL;
	new_node->blockedForJoin = NULL;
	/*Inicializa el sched_futex del tcb a cero */
	futex_init(&new_node->sched_futex, 0);

	/*Pone en la Q de los bloques de hilo.*/
    CEthread_q_add(new_node);

	/*Llamada al clon con puntero a la función contenedora. tcb se pasará como argumento a la función contenedora*/
	if ((tid =
	     clone(CEthread_wrapper, (char *)child_stack, clone_flags,
		   new_node)) == -1) {
		printf("clone failed! \n");
		printf("ERROR: %s \n", strerror(errno));
		return (-errno);
	}
	/*Guarda el tid devuelto por la llamada del sistema clone en el tcb.*/
	new_thread_ID->tid = tid;
	new_node->tid = tid;

	DEBUG_PRINTF("create: Finished initialising new thread: %ld\n",
		     (unsigned long)new_thread_ID->tid);
	return 0;
}

/*---------------------------------------------------------CE_thread_join()----------------------------------------------------------------------------------*/

/* Espera en el hilo especificado por "target_thread". Si el hilo está DEFUNCT,
   simplemente recopile el estado de la devolución. De lo contrario, espere a que el hilo muera y luego
   recoga el estado de la devolución.
 */
int CEthread_join(CEthread_t target_thread, void **status)
{
	CEthread_private_t *target, *self_ptr;

	self_ptr = __CEthread_selfptr();
	DEBUG_PRINTF("Join: Got tid: %ld\n", (unsigned long)self_ptr->tid);
	target = CEthread_q_search(target_thread.tid);

	/* Si el hilo ya está muerto, no es necesario esperar. Solo recoge el valor de 
	   devolucion y sale.
	 */
	if (target->state == DEFUNCT) {
		*status = target->returnValue;
		return 0;
	}

	DEBUG_PRINTF("Join: Checking for blocked for join\n");
	/* Si el hilo no está muerto y alguien más ya lo está esperando
	   devuelve un error
	 */
	if (target->blockedForJoin != NULL)
		return -1;

	/* Nos ponemos en espara para unirnos a este hilo. Establece nuestro estado como
	   BLOCKED para que no nos vuelvan a programar.
	 */
	target->blockedForJoin = self_ptr;
	DEBUG_PRINTF("Join: Setting state of %ld to %d\n",
		     (unsigned long)self_ptr->tid, BLOCKED);
	self_ptr->state = BLOCKED;

	/*Programa otro hilo*/
	CEthread_yield();

	/* El hilo objetivo murió, colleciona el valor de retorno y regresa */
	*status = target->returnValue;
	return 0;
}
/*---------------------------------------------------------CE_thread_exit()----------------------------------------------------------------------------------*/

/* Al llamar a exit() de glibc se sale del proceso. 
   Llame directamente al syscall en vez de eso
 */
static void __CEthread_do_exit()
{
	syscall(SYS_exit, 0);
}

/* Ver si alguien nos está bloqueando para unirnos. Si es así, marque ese hilo como READY
   y nos terminamos.
 */
void CEthread_exit(void *return_val)
{
	CEthread_private_t *self_ptr;

	/*Obtenga un puntero a nuestra estructura TCB*/
	self_ptr = __CEthread_selfptr();

    /*No elimine el nodo de la lista todavía. Tenemos que agarrar el valor devuelto */
	self_ptr->state = DEFUNCT;
	self_ptr->returnValue = return_val;

	/* Cambia el estado de cualquier hilo que nos esté esperando. El despachador FIFO hará lo 
	   necesario
	 */
	if (self_ptr->blockedForJoin != NULL)
		self_ptr->blockedForJoin->state = READY;

	__CEthread_dispatcher(self_ptr);

	/* Termina el hilo */
	__CEthread_do_exit();

}
/*---------------------------------------------------------------CEmutex_init()------------------------------------------------------------------------*/

/*
 * CEmutex_init - Inicializa un mutex, preparando su futex interno y 
 * configurando las variables para indicar que no está bloqueado.
 *
 * @param mutex: Puntero al mutex que se va a inicializar.
 *
 * return: Siempre retorna 0, indicando que la operación fue exitosa.
 */
int CEmutex_init(CEmutex_t *mutex) {
	if (mutex->locked) {
        	// El mutex ya ha sido inicializado
        	return -1;
	}
	// El mutex comienza sin estar bloqueado.
	mutex->locked = 0;
	// No hay ningún hilo que sea el dueño del mutex al inicio.
	mutex->owner = NULL;
	// No hay hilos en la lista de espera al inicio.
	 mutex->waiters = NULL;
	// Inicializa el futex asociado al mutex con un valor inicial de 0.
	futex_init(&mutex->mutex_futex, 0);
    return 0;
}
/*---------------------------------------------------------------CEmutex_destroy()------------------------------------------------------------------------*/

/*
 * CEmutex_destroy - Destruye un mutex.
 *
 * Dado que no se asignan recursos dinámicos durante la vida útil del mutex,
 * no es necesario realizar ninguna acción específica en esta función.
 *
 * @param mutex: Puntero al mutex que se va a destruir.
 *
 * return: Siempre retorna 0, indicando que la operación fue exitosa.
 */
int CEmutex_destroy(CEmutex_t *mutex) {
	if (mutex->locked) {
        	// El mutex está bloqueado, no se puede destruir
        	return -1;
	}
	free(mutex);
	return 0;
}

/*---------------------------------------------------------------CEmutex_unlock()------------------------------------------------------------------------*/

/*
 * CEmutex_unlock - Libera el mutex, permitiendo que otros hilos lo adquieran.
 *
 * Si el hilo actual es el propietario del mutex, lo libera y despierta al primer hilo
 * en la lista de espera, si existe alguno.
 *
 * @param mutex: Puntero al mutex que se va a liberar.
 *
 * return: 0 si se libera el mutex exitosamente, -1 si el hilo que llama
 * a esta función no es el propietario del mutex.
 */
int CEmutex_unlock(CEmutex_t *mutex) {
	// Obtiene un puntero al TCB (Thread Control Block) del hilo actual.
	CEthread_private_t *self = __CEthread_selfptr();
	// Si el hilo actual no es el propietario del mutex, devuelve un error.
	if (mutex->owner != self) {
		// El hilo actual no es el dueño del mutex.
		return -1;
	}
	// Marca el mutex como desbloqueado.
	mutex->locked = 0;
	// El hilo actual ya no es el dueño del mutex.
	mutex->owner = NULL;
	// Si hay hilos en espera, despierta al primero de la lista.
	if (mutex->waiters != NULL) {
		// Obtiene el primer hilo de la lista de espera.
		CEthread_private_t *waiter = mutex->waiters;
       		// Actualiza la lista de espera, eliminando el primer hilo.
        	mutex->waiters = waiter->next;
		// Libera el futex para permitir que el hilo despertado continúe.
		futex_up(&mutex->mutex_futex);
	}
	return 0;
}
