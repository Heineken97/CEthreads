#ifndef CEMUTEX_H
#define CEMUTEX_H

typedef struct {
    int value;
} CEmutex_t;

void CEmutex_init(CEmutex_t *mutex);
void CEmutex_destroy(CEmutex_t *mutex);
void CEmutex_lock(CEmutex_t *mutex);
void CEmutex_unlock(CEmutex_t *mutex);

#endif
