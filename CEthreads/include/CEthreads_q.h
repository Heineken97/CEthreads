#include "CEthreads.h"
void CEthread_q_init(CEthread_private_t *node);
void CEthread_q_add(CEthread_private_t *node);
void CEthread_q_delete(CEthread_private_t *node);
void CEthread_q_state_display();
void CEthread_q_add_waiter(CEthread_private_t *thread, CEthread_private_t **waiters);
CEthread_private_t * CEthread_q_search(unsigned long);
