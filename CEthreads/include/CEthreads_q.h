#include "../src/CEthreads_q.c"

extern void CEthread_q_init(CEthread_private_t *node);
extern void CEthread_q_add(CEthread_private_t *node);
extern void CEthread_q_delete(CEthread_private_t *node);
extern void CEthread_q_state_display();
extern void CEthread_q_add_waiter(CEthread_private_t *thread, CEthread_private_t **waiters);
extern CEthread_private_t * CEthread_q_search(unsigned long);
