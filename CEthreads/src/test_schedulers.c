#include "../include/schedulers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Función para clonar barcos
void clone_ships(Ship *source, Ship *destination, int ship_count) {
    for (int i = 0; i < ship_count; i++) {
        destination[i] = source[i];
    }
}

int main() {
    // Barcos originales
    Ship ships_LR[] = {
        {1, 10, 2, 5.0f},
        {2, 20, 1, 3.0f},
        {3, 30, 3, 7.0f},
        {4, 40, 2, 4.0f},
        {5, 50, 1, 2.0f}
    };
    int ship_count_LR = sizeof(ships_LR) / sizeof(ships_LR[0]);

    // Crear una copia para cada prueba
    Ship ships_for_test[ship_count_LR];

    printf("Before calling schedulers:\n");
    for (int i = 0; i < ship_count_LR; i++) {
        printf("Ship ID: %d, Processing Time: %.2f\n", ships_LR[i].id, ships_LR[i].processing_time);
    }

    // Round Robin Scheduler Test
    printf("\nTesting Round Robin scheduler:\n");
    clone_ships(ships_LR, ships_for_test, ship_count_LR);
    round_robin_scheduler(ships_for_test, ship_count_LR, 2);

    // Priority Scheduler Test
    printf("\nTesting Priority scheduler:\n");
    clone_ships(ships_LR, ships_for_test, ship_count_LR);
    priority_scheduler(ships_for_test, ship_count_LR);

    // Shortest Job First (SJF) Scheduler Test
    printf("\nTesting Shortest Job First (SJF) scheduler:\n");
    clone_ships(ships_LR, ships_for_test, ship_count_LR);
    sjf_scheduler(ships_for_test, ship_count_LR);

    // First Come First Serve (FCFS) Scheduler Test
    printf("\nTesting First Come First Serve (FCFS) scheduler:\n");
    clone_ships(ships_LR, ships_for_test, ship_count_LR);
    fcfs_scheduler(ships_for_test, ship_count_LR);

    // Real Time Scheduler Test
    printf("\nTesting Real Time scheduler:\n");
    clone_ships(ships_LR, ships_for_test, ship_count_LR);
    real_time_scheduler(ships_for_test, ship_count_LR, 10.0);

    printf("\nAfter calling schedulers:\n");
    for (int i = 0; i < ship_count_LR; i++) {
        printf("Ship ID: %d, Processing Time: %.2f\n", ships_LR[i].id, ships_LR[i].processing_time);
    }

    return 0;
}
