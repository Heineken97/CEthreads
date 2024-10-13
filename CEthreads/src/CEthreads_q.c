#include "../include/CEthread_private.h"
#include <stddef.h>
#include "../include/CEthreads_q.h"

CEthread_private_t *CEthread_q_head = NULL;

CEthread_private_t *CEthread_q_search(unsigned long tid) {
    // Implementación para buscar un hilo en la cola
    CEthread_private_t *current = CEthread_q_head;
    while (current != NULL) {
        if (current->tid == tid) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
