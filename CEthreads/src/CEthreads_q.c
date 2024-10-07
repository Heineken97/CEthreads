#include "../include/CEthreads.h"

/*El puntero externo global definido en CEthread.h apunta a la cabeza del nodo que
esta en cola en el tcb (Thread Control Blocks)*/
CEthread_private_t *CEthread_q_head;

/*Inicializa la cola con un solo nodo*/
void CEthread_q_init(CEthread_private_t * node){

    node->prev = node;
    node->next = node;

    CEthread_q_head = node;
}

/*Añade un nodo a la cola, al final de esta misma*/
void CEthread_q_add(CEthread_private_t * node)
{

	if (CEthread_q_head == NULL) {
		//Q(cola) todavia no esta inicializado entonces lo crea
		CEthread_q_init(node);
		return;
	}
	//Inserta el nodo al final de Q
	node->next = CEthread_q_head;
	node->prev = CEthread_q_head->prev;
	CEthread_q_head->prev->next = node;
	CEthread_q_head->prev = node;

	return;

}

/*Esta funcion borra un nodo especifico (pasado como un parametro) de la cola*/
void CEthread_q_delete(CEthread_private_t * node)
{

	CEthread_private_t *p;
	if (node == CEthread_q_head && node->next == CEthread_q_head) {
		//Solo hay un unico nodo y esta siendo borrado
		printf("La cola esta vacia\n");
		CEthread_q_head = NULL;
	}

	if (node == CEthread_q_head)
		CEthread_q_head = node->next;

	p = node->prev;

	p->next = node->next;
	node->next->prev = p;

	return;

}
/*Itera sobre toda la cola e imprime el estado de todos los miembros del tcb*/
void CEthread_q_state_display()
{

	if (CEthread_q_head != NULL) {

		//Muestra la cola 
		printf("\n The Q contents are -> \n");
		CEthread_private_t *p;
		p = CEthread_q_head;
		do {		//Viaja hasta el ultimo nodo de la cola
			printf(" %d\n", p->state);
			p = p->next;
		} while (p != CEthread_q_head);

	}

}
/*Itera sobre la cola e imprime el estado de un hilo en especifico*/
CEthread_private_t *CEthread_q_search(unsigned long new_tid)
{

	CEthread_private_t *p;
	if (CEthread_q_add != NULL) {

		p = CEthread_q_head;
		do {		//Viaja hasta el ultimo nodo de la cola
			if (p->tid == new_tid)
				return p;
			p = p->next;
		} while (p != CEthread_q_head);

	}
	return NULL;

}
// Función para agregar un hilo a la lista de esperadores de un mutex
void CEthread_q_add_waiter(CEthread_private_t *thread, CEthread_private_t **waiters) {
	thread->next = *waiters;
	*waiters = thread;
}
