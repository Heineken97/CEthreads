#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "schedulers.h"
#include "ship.h"

// Round Robin Scheduler
void round_robin_scheduler(/*Ship ships[],*/ int ship_count, int time_quantum) {
    // int completed = 0;
    // while (completed < ship_count) {
    //     for (int i = 0; i < ship_count; i++) {
    //         if (ships[i].thread.state != THREAD_TERMINATED) {
    //             printf("Barco %d ejecutando por %d segundos en RR.\n", i, time_quantum);
    //             sleep(time_quantum); // Simula el tiempo de ejecución
    //             ships[i].position += 1;

    //             if (ships[i].position >= CHANNEL_LENGTH) {
    //                 ships[i].thread.state = THREAD_TERMINATED;
    //                 printf("Barco %d ha finalizado en RR.\n", i);
    //                 completed++;
    //             }
    //         }
    //     }
    // }
}

// Priority Scheduler
void priority_scheduler(/*Ship ships[],*/ int ship_count) {
    // for (int i = 0; i < ship_count; i++) {
    //     for (int j = i + 1; j < ship_count; j++) {
    //         if (ships[i].thread.priority < ships[j].thread.priority) {
    //             Ship temp = ships[i];
    //             ships[i] = ships[j];
    //             ships[j] = temp;
    //         }
    //     }
    // }

    // for (int i = 0; i < ship_count; i++) {
    //     printf("Ejecutando barco %d con prioridad %d\n", i, ships[i].thread.priority);
    //     while (ships[i].position < CHANNEL_LENGTH) {
    //         printf("Barco %d avanzando en canal.\n", i);
    //         sleep(1); // Simula el trabajo del hilo
    //         ships[i].position += 1;
    //     }
    //     printf("Barco %d ha salido del canal (prioridad).\n", i);
    // }
}

// Shortest Job First (SJF) Scheduler
void sjf_scheduler(/*Ship ships[],*/ int ship_count) {
    // for (int i = 0; i < ship_count; i++) {
    //     for (int j = i + 1; j < ship_count; j++) {
    //         if (ships[i].position > ships[j].position) {
    //             Ship temp = ships[i];
    //             ships[i] = ships[j];
    //             ships[j] = temp;
    //         }
    //     }
    // }

    // for (int i = 0; i < ship_count; i++) {
    //     printf("Ejecutando barco %d con trabajo estimado %d\n", i, ships[i].position);
    //     while (ships[i].position < CHANNEL_LENGTH) {
    //         printf("Barco %d avanzando en canal (SJF).\n", i);
    //         sleep(1);
    //         ships[i].position += 1;
    //     }
    //     printf("Barco %d ha salido del canal (SJF).\n", i);
    // }
}

// Función para convertir una cadena de caracteres a un tipo de calendarizador
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