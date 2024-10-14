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

#define MAX_SHIPS 100

// Round Robin Scheduler
void round_robin_scheduler(Ship ships[], int scheduled_ids[]) {
    int completed = 0;
    while (completed < MAX_SHIPS) {
        for (int i = 0; i < MAX_SHIPS; i++) {
            printf("Barco %d ejecutando por %d segundos en RR.\n", i, 69);
            // if (ships[i].thread.state != THREAD_TERMINATED) {
            //     printf("Barco %d ejecutando por %d segundos en RR.\n", i, time_quantum);
            //     sleep(time_quantum); // Simula el tiempo de ejecución
            //     ships[i].position += 1;

            //     if (ships[i].position >= CANAL_LENGTH) {
            //         ships[i].thread.state = THREAD_TERMINATED;
            //         printf("Barco %d ha finalizado en RR.\n", i);
            //         completed++;
            //     }
            // }
        }
    }
}

// Function to simulate Round Robin scheduling
void round_robin_scheduler_(Ship ships[], int ship_count, int time_quantum) {
    int total_time = 0;
    int completed = 0;
    float remaining_time[ship_count];

    // Initialize remaining times
    for (int i = 0; i < ship_count; i++) {
        remaining_time[i] = ships[i].processing_time;
    }

    while (completed < ship_count) {
        for (int i = 0; i < ship_count; i++) {
            if (remaining_time[i] > 0) {
                printf("Ship ID: %d, Time: %d\n", ships[i].id, total_time);
                if (remaining_time[i] <= time_quantum) {
                    total_time += remaining_time[i];
                    remaining_time[i] = 0;
                    completed++;
                } else {
                    remaining_time[i] -= time_quantum;
                    total_time += time_quantum;
                }
            }
        }
    }
    printf("Total Time: %d\n", total_time);
}

// Priority Scheduler
void priority_scheduler(Ship ships[], int scheduled_ids[]) {
    for (int i = 0; i < 10; i++) {
        printf("Ejecutando barco %d en Priority\n", i);
        for (int j = i + 1; j < MAX_SHIPS; j++) {
            if (ships[i].priority < ships[j].priority) {
                Ship temp = ships[i];
                ships[i] = ships[j];
                ships[j] = temp;
            }
        }
    }

    // for (int i = 0; i < ship_count; i++) {
    //     printf("Ejecutando barco %d con prioridad %d\n", i, ships[i].thread.priority);
    //     while (ships[i].position < CANAL_LENGTH) {
    //         printf("Barco %d avanzando en canal.\n", i);
    //         sleep(1); // Simula el trabajo del hilo
    //         ships[i].position += 1;
    //     }
    //     printf("Barco %d ha salido del canal (prioridad).\n", i);
    // }
}

// Function to sort ships using the Priority algorithm
void priority_scheduler_(Ship ships[], int ship_count) {
    // Sort ships based on priority (higher value has higher priority)
    for (int i = 0; i < ship_count - 1; i++) {
        for (int j = i + 1; j < ship_count; j++) {
            if (ships[i].priority < ships[j].priority) {
                Ship temp = ships[i];
                ships[i] = ships[j];
                ships[j] = temp;
            }
        }
    }
    printf("Ships sorted by Priority:\n");
    for (int i = 0; i < ship_count; i++) {
        printf("Ship ID: %d, Priority: %d\n", ships[i].id, ships[i].priority);
    }

    // Simulate Priority scheduling
    float total_time = 0;
    printf("Priority scheduler:\n");
    for (int i = 0; i < ship_count; i++) {
        printf("Ship ID: %d, Time: %.2f\n", ships[i].id, total_time);
        total_time += ships[i].processing_time;
    }
    printf("Total Time: %.2f\n", total_time);
}

// Shortest Job First (SJF) Scheduler
void sjf_scheduler(Ship ships[], int scheduled_ids[]) {
    for (int i = 0; i < MAX_SHIPS; i++) {
        printf("Ejecutando barco %d en SJF\n", i);
        for (int j = i + 1; j < MAX_SHIPS; j++) {
            if (ships[i].position > ships[j].position) {
                Ship temp = ships[i];
                ships[i] = ships[j];
                ships[j] = temp;
            }
        }
    }

    // for (int i = 0; i < ship_count; i++) {
    //     printf("Ejecutando barco %d con trabajo estimado %d\n", i, ships[i].position);
    //     while (ships[i].position < CANAL_LENGTH) {
    //         printf("Barco %d avanzando en canal (SJF).\n", i);
    //         sleep(1);
    //         ships[i].position += 1;
    //     }
    //     printf("Barco %d ha salido del canal (SJF).\n", i);
    // }
}

// Function to simulate Shortest Job First (SJF) scheduling
void sjf_scheduler_(Ship ships[], int ship_count) {
    printf("Before sorting:\n");
    for (int i = 0; i < ship_count; i++) {
        printf("Ship ID: %d, Processing Time: %.2f\n", ships[i].id, ships[i].processing_time);
    }

    // Sort ships based on processing time (shorter jobs first)
    for (int i = 0; i < ship_count - 1; i++) {
        for (int j = i + 1; j < ship_count; j++) {
            if (ships[i].processing_time > ships[j].processing_time) {
                Ship temp = ships[i];
                ships[i] = ships[j];
                ships[j] = temp;
            }
        }
    }

    printf("After sorting:\n");
    for (int i = 0; i < ship_count; i++) {
        printf("Ship ID: %d, Processing Time: %.2f\n", ships[i].id, ships[i].processing_time);
    }

    // Simulate SJF scheduling
    float total_time = 0;
    printf("SJF scheduler:\n");
    for (int i = 0; i < ship_count; i++) {
        printf("Ship ID: %d, Time: %.2f\n", ships[i].id, total_time);
        total_time += ships[i].processing_time;
    }
    printf("Total Time: %.2f\n", total_time);
}

// First-Come-First-Served (FCFS) Scheduler
void fcfs_scheduler(Ship ships[], int scheduled_ids[]) {
    for (int i = 0; i < MAX_SHIPS; i++) {
        printf("Ejecutando barco %d en FCFS\n", i);
        // while (ships[i].position < CANAL_LENGTH) {
        //     printf("Barco %d avanzando en canal (FCFS).\n", i);
        //     sleep(1); // Simula el trabajo del hilo
        //     ships[i].position += 1;
        // }
        // ships[i].thread.state = THREAD_TERMINATED; // Marcar el barco como terminado
        // printf("Barco %d ha salido del canal (FCFS).\n", i);
    }
}

// Function to simulate First Come First Serve (FCFS) scheduling
void fcfs_scheduler_(Ship ships[], int ship_count) {
    float total_time = 0;
    printf("FCFS scheduler:\n");
    for (int i = 0; i < ship_count; i++) {
        printf("Ship ID: %d, Time: %.2f\n", ships[i].id, total_time);
        total_time += ships[i].processing_time;
    }
    printf("Total Time: %.2f\n", total_time);
}

// Real-Time Scheduler (RTS)
void real_time_scheduler(Ship ships[], int scheduled_ids[], float max_time) {
    for (int i = 0; i < MAX_SHIPS; i++) {
        printf("Ejecutando barco %d en RTS\n", i);
        for (int j = i + 1; j < MAX_SHIPS; j++) {
            if (ships[i].position > ships[j].position) {
                Ship temp = ships[i];
                ships[i] = ships[j];
                ships[j] = temp;
            }
        }
    }

    // for (int i = 0; i < ship_count; i++) {
    //     printf("Ejecutando barco %d en RTS\n", i);
    //     while (ships[i].position < CANAL_LENGTH) {
    //         printf("Barco %d avanzando en canal (RTS).\n", i);
    //         sleep(1);
    //         ships[i].position += 1;
    //     }
    //     ships[i].thread.state = THREAD_TERMINATED; // Marcar el barco como terminado
    //     printf("Barco %d ha salido del canal (RTS).\n", i);
    // }
}

// Function to simulate Real Time scheduler
void real_time_scheduler_(Ship ships[], int ship_count, float time_slice) {
    printf("Real Time scheduler:\n");
    float total_time = 0;
    for (int i = 0; i < ship_count; i++) {
        printf("Ship ID: %d, Time: %.2f\n", ships[i].id, time_slice);
        total_time += time_slice;
    }
    printf("Total Time: %.2f\n", total_time);
}


/*
 * Function: test_scheduler
 * -----------------------
 * Filters the IDs of ships with `position == 0` from a given array of ships and 
 * populates the corresponding scheduled IDs array.
 * 
 * Parameters:
 * - ships[]: Array of ships to filter.
 * - scheduled_ids[]: Array of IDs to store the filtered ships.
 * 
 * Notes:
 * - Only ships with `position == 0` are added to the IDs array.
 * - The remaining slots are filled with 0 to indicate unused slots.
 */
void test_schedule(Ship ships[], int scheduled_ids[]) {

    printf("\nTest scheduler on duty...\n");

    int index = 0;  // Index for the scheduled_ids array

    // Iterate over the ships array
    for (int i = 0; i < MAX_SHIPS; i++) {
        if (ships[i].position == 0) {
            scheduled_ids[index] = ships[i].id;  // Add ship ID to the array
            index++;
        }
    }

    // Fill the remaining positions with 0 to indicate unused slots
    for (int i = index; i < MAX_SHIPS; i++) {
        scheduled_ids[i] = 0;
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
 *   (e.g., "none", "round_robin", "priority", "sjf", "fcfs", "real_time")
 * 
 * Returns:
 * - The corresponding SchedulerType enum value (e.g., NONE, ROUND_ROBIN, PRIORITY, SJF).
 * 
 * Notes:
 * - If the provided scheduler_name does not match any valid scheduler type, the function prints
 *   an error message and terminates the program.
 */
SchedulerType get_scheduler_type(const char* scheduler_name) {
    if (strcmp(scheduler_name, "stest") == 0) {
        return STEST;
    } else if (strcmp(scheduler_name, "round_robin") == 0) {
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
        printf("Error: Tipo de calendarizador no válido: %s\n", scheduler_name);
        exit(EXIT_FAILURE);  // Terminar el programa si el argumento no es válido
    }
}



/*
 * Function: schedule
 * ------------------
 * Schedules the available ships using the selected scheduling algorithm.
 * 
 * Parameters:
 * - ships_LR[]: Array of ships moving Left to Right.
 * - scheduled_ids_LR[]: Array of IDs to store the scheduled ships moving Left to Right.
 * - ships_RL[]: Array of ships moving Right to Left.
 * - scheduled_ids_RL[]: Array of IDs to store the scheduled ships moving Right to Left.
 * - scheduler_type: The selected scheduler algorithm to be used.
 * 
 * Notes:
 * - Depending on the scheduler_type, the appropriate algorithm is applied.
 * - The NONE scheduler simply filters ships with position == 0.
 * - If an unrecognized scheduler type is given, the program exits with an error message.
 */
void schedule(SchedulerType scheduler_type,
              Ship ships_LR[], int scheduled_ids_LR[], 
              Ship ships_RL[], int scheduled_ids_RL[], 
              float max_time) {

    switch (scheduler_type) {
        case ROUND_ROBIN:
            round_robin_scheduler(ships_LR, scheduled_ids_LR);
            round_robin_scheduler(ships_RL, scheduled_ids_RL);
            break;
        case PRIORITY:
            priority_scheduler(ships_LR, scheduled_ids_LR);
            priority_scheduler(ships_RL, scheduled_ids_RL);
            break;
        case SJF:
            sjf_scheduler(ships_LR, scheduled_ids_LR);
            sjf_scheduler(ships_RL, scheduled_ids_RL);
            break;
        case FCFS:
            fcfs_scheduler(ships_LR, scheduled_ids_LR);
            fcfs_scheduler(ships_RL, scheduled_ids_RL);
            break;
        case REAL_TIME:
            real_time_scheduler(ships_LR, scheduled_ids_LR, max_time);
            real_time_scheduler(ships_RL, scheduled_ids_RL, max_time);
            break;
        case STEST:
            test_schedule(ships_LR, scheduled_ids_LR);
            test_schedule(ships_RL, scheduled_ids_RL);
            break;
        default:
            printf("Scheduler no reconocido.\n");
            exit(EXIT_FAILURE);
    }
}

