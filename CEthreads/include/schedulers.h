/*
 * File: schedulers.h
 *
 */
#ifndef SCHEDULERS_H
#define SCHEDULERS_H

#include "ship.h"


typedef enum {
    STEST,           // undefined
    ROUND_ROBIN,    // Round Robin
    PRIORITY,       // Prioridad
    SJF,            // Shortest Job First (SJF)
    FCFS,           // First Come First Serve (FCFS)
    REAL_TIME       // Tiempo Real
} SchedulerType;

// Funciones para los distintos algoritmos de calendarización
void round_robin_scheduler(Ship ships[], int scheduled_ids[]);
void priority_scheduler(Ship ships[], int scheduled_ids[]);
void sjf_scheduler(Ship ships[], int scheduled_ids[]);
void fcfs_scheduler(Ship ships[], int scheduled_ids[]);
void real_time_scheduler(Ship ships[], int scheduled_ids[], float max_time);

void test_schedule(Ship ships[], int scheduled_ids[]);

// Función para convertir una cadena de caracteres a un tipo de calendarizador
SchedulerType get_scheduler_type(const char* scheduler_name);

void schedule(SchedulerType scheduler_type,
              Ship ships_LR[], int scheduled_ids_LR[],
              Ship ships_RL[], int scheduled_ids_RL[], float max_time);

#endif // SCHEDULERS_H
