#ifndef SCHEDULERS_H
#define SCHEDULERS_H

typedef enum {
    ROUND_ROBIN,     // Round Robin
    PRIORITY,        // Prioridad
    SJF,             // Shortest Job First (SJF)
    FCFS,            // First Come First Serve (FCFS)
    REAL_TIME        // Tiempo Real
} SchedulerType;

// Funciones para los distintos algoritmos de calendarización
void round_robin_scheduler(/*Ship ships[],*/ int ship_count, int time_quantum);
void priority_scheduler(/*Ship ships[],*/ int ship_count);
void sjf_scheduler(/*Ship ships[],*/ int ship_count);

// Función para convertir una cadena de caracteres a un tipo de calendarizador
SchedulerType get_scheduler_type(const char* scheduler_name);

#endif // SCHEDULERS_H
