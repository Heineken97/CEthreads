#ifndef SCHEDULERS_H
#define SCHEDULERS_H
#include "ship.h"
#include "CEthreads.h"

// Funciones para los distintos algoritmos de calendarización
void round_robin_scheduler(Ship ships[], int ship_count, int time_quantum);
void priority_scheduler(Ship ships[], int ship_count);
void sjf_scheduler(Ship ships[], int ship_count);
void fcfs_scheduler(Ship ships[], int ship_count);
void real_time_scheduler(Ship ships[], int ship_count);

#endif // SCHEDULERS_H
