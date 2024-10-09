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
#include "config.h"

// Round Robin Scheduler
void round_robin_scheduler(Ship ships[], int ship_count, int time_quantum) {
    int completed = 0;
    while (completed < ship_count) {
        for (int i = 0; i < ship_count; i++) {
            if (ships[i].thread.state != THREAD_TERMINATED) {
                printf("Barco %d ejecutando por %d segundos en RR.\n", i, time_quantum);
                sleep(time_quantum); // Simula el tiempo de ejecución
                ships[i].position += 1;

                if (ships[i].position >= CANAL_LENGTH) {
                    ships[i].thread.state = THREAD_TERMINATED;
                    printf("Barco %d ha finalizado en RR.\n", i);
                    completed++;
                }
            }
        }
    }
}

// Priority Scheduler
void priority_scheduler(Ship ships[], int ship_count) {
    for (int i = 0; i < ship_count; i++) {
        for (int j = i + 1; j < ship_count; j++) {
            if (ships[i].thread.priority < ships[j].thread.priority) {
                Ship temp = ships[i];
                ships[i] = ships[j];
                ships[j] = temp;
            }
        }
    }

    for (int i = 0; i < ship_count; i++) {
        printf("Ejecutando barco %d con prioridad %d\n", i, ships[i].thread.priority);
        while (ships[i].position < CANAL_LENGTH) {
            printf("Barco %d avanzando en canal.\n", i);
            sleep(1); // Simula el trabajo del hilo
            ships[i].position += 1;
        }
        printf("Barco %d ha salido del canal (prioridad).\n", i);
    }
}

// Shortest Job First (SJF) Scheduler
void sjf_scheduler(Ship ships[], int ship_count) {
    for (int i = 0; i < ship_count; i++) {
        for (int j = i + 1; j < ship_count; j++) {
            if (ships[i].position > ships[j].position) {
                Ship temp = ships[i];
                ships[i] = ships[j];
                ships[j] = temp;
            }
        }
    }

    for (int i = 0; i < ship_count; i++) {
        printf("Ejecutando barco %d con trabajo estimado %d\n", i, ships[i].position);
        while (ships[i].position < CANAL_LENGTH) {
            printf("Barco %d avanzando en canal (SJF).\n", i);
            sleep(1);
            ships[i].position += 1;
        }
        printf("Barco %d ha salido del canal (SJF).\n", i);
    }
}

// First-Come-First-Served (FCFS) Scheduler
void fcfs_scheduler(Ship ships[], int ship_count) {
    for (int i = 0; i < ship_count; i++) {
        printf("Ejecutando barco %d en FCFS\n", i);
        while (ships[i].position < CANAL_LENGTH) {
            printf("Barco %d avanzando en canal (FCFS).\n", i);
            sleep(1); // Simula el trabajo del hilo
            ships[i].position += 1;
        }
        ships[i].thread.state = THREAD_TERMINATED; // Marcar el barco como terminado
        printf("Barco %d ha salido del canal (FCFS).\n", i);
    }
}

// Real-Time Scheduler (RTS)
void real_time_scheduler(Ship ships[], int ship_count) {
    for (int i = 0; i < ship_count; i++) {
        for (int j = i + 1; j < ship_count; j++) {
            if (ships[i].position > ships[j].position) {
                Ship temp = ships[i];
                ships[i] = ships[j];
                ships[j] = temp;
            }
        }
    }

    for (int i = 0; i < ship_count; i++) {
        printf("Ejecutando barco %d en RTS\n", i);
        while (ships[i].position < CANAL_LENGTH) {
            printf("Barco %d avanzando en canal (RTS).\n", i);
            sleep(1);
            ships[i].position += 1;
        }
        ships[i].thread.state = THREAD_TERMINATED; // Marcar el barco como terminado
        printf("Barco %d ha salido del canal (RTS).\n", i);
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
