#include "CEthreads_q.c"

extern void CEthread_q_init(CEthread_private_t *node);
extern void CEthread_q_add(CEthread_private_t *node);
extern void CEthread_q_delete(CEthread_private_t *node);
extern void CEthread_q_state_display();
extern CEthread_private_t * CEthread_q_search(unsigned long);