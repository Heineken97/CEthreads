/*
 * File: schedulers.c
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "schedulers.h"
#include "ship.h"

// Round Robin Scheduler
void round_robin_scheduler(Ship ships[], int ship_count, int time_quantum) {
    printf("Simulando calendarizador Round Robin con un time quantum de %d\n", time_quantum);
    for (int i = 0; i < ship_count; i++) {
        printf("Barco ID: %d, Procesado en Round Robin\n", ships[i].id);
    }
}

// Priority Scheduler
void priority_scheduler(Ship ships[], int ship_count) {
    printf("Simulando calendarizador de Prioridad\n");
    for (int i = 0; i < ship_count; i++) {
        printf("Barco ID: %d, Procesado según Prioridad\n", ships[i].id);
    }
}

// Shortest Job First (SJF) Scheduler
void sjf_scheduler(Ship ships[], int ship_count) {
    printf("Simulando calendarizador Shortest Job First\n");
    for (int i = 0; i < ship_count; i++) {
        printf("Barco ID: %d, Procesado en SJF\n", ships[i].id);
    }
}

// First-Come-First-Served (FCFS) Scheduler
void fcfs_scheduler(Ship ships[], int ship_count) {
    printf("Simulando calendarizador First-Come-First-Served\n");
    for (int i = 0; i < ship_count; i++) {
        printf("Barco ID: %d, Procesado en FCFS\n", ships[i].id);
    }
}

// Real-Time Scheduler (RTS)
void real_time_scheduler(Ship ships[], int ship_count) {
    printf("Simulando calendarizador Real-Time Scheduler\n");
    for (int i = 0; i < ship_count; i++) {
        printf("Barco ID: %d, Procesado en RTS\n", ships[i].id);
    }
}

/*
 * Function: get_scheduler_type
 * ----------------------------
 * Converts a string representing a scheduling algorithm into the corresponding SchedulerType
 * enum.
 * 
 * Parameters:
 * - scheduler_name: string representing the name of the scheduling algorithm
 *   (e.g., "round_robin", "priority", "sjf", "fcfs", "real_time")
 * 
 * Returns:
 * - The corresponding SchedulerType enum value (e.g., ROUND_ROBIN, PRIORITY, SJF).
 * 
 * Notes:
 * - If the provided scheduler_name does not match any valid scheduler type, the function prints
 *   an error message and terminates the program.
 */
SchedulerType get_scheduler_type(const char* scheduler_name) {
    if (strcmp(scheduler_name, "round_robin") == 0) {
        return ROUND_ROBIN;
    } else if (strcmp(scheduler_name, "priority") == 0) {
        return PRIORITY;
    } else if (strcmp(scheduler_name, "sjf") == 0) {
        return SJF;
    } else if (strcmp(scheduler_name, "fcfs") == 0) {
        return FCFS;
    } else if (strcmp(scheduler_name, "real_time") == 0) {
        return REAL_TIME;
    } else {
        printf("Error: Tipo de calendarizador no válido.\n");
        exit(EXIT_FAILURE);  // Terminar el programa si el argumento no es válido
    }
}
