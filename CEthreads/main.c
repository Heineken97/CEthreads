#include "CEthreads.h"
#include "schedulers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ship.h"

#define MAX_SHIPS 100
#define CHANNEL_LENGTH 10
#define NORMAL_SPEED 2
#define PESQUERA_SPEED 1
#define PATRULLA_SPEED 0.5

Ship ships[MAX_SHIPS];
int ship_count = 0;

// Función que simula el comportamiento del barco
void ship_behavior(void *arg) {
    Ship *ship = (Ship *)arg;
    int speed;

    // Asignar velocidad según tipo de barco
    switch (ship->type) {
        case NORMAL: speed = NORMAL_SPEED; break;
        case PESQUERA: speed = PESQUERA_SPEED; break;
        case PATRULLA: speed = PATRULLA_SPEED; break;
    }

    while (ship->position < CHANNEL_LENGTH) {
        printf("Barco de tipo %d en posición %d\n", ship->type, ship->position);
        sleep(speed); // Simular tiempo de movimiento
        ship->position++;
    }
    printf("Barco de tipo %d ha salido del canal\n", ship->type);
}

// Generar barco con tipo definido
void create_ship(ShipType type, int priority) {
    if (ship_count >= MAX_SHIPS) {
        printf("Límite de barcos alcanzado.\n");
        return;
    }

    Ship *new_ship = &ships[ship_count++];
    new_ship->type = type;
    new_ship->position = 0; // Posición inicial

    // Crear hilo para el barco con prioridad
    CEthread_create(&new_ship->thread, ship_behavior, new_ship);
    new_ship->thread.priority = priority;
}

int main() {
    // Crear varios barcos
    create_ship(NORMAL, 1);
    create_ship(PESQUERA, 3);
    create_ship(PATRULLA, 2);
    create_ship(NORMAL, 4);

    // Ejecutar los diferentes algoritmos de calendarización
    printf("\n=== Ejecutando Round Robin ===\n");
    round_robin_scheduler(ships, ship_count, 2);

    printf("\n=== Ejecutando Prioridad ===\n");
    priority_scheduler(ships, ship_count);

    printf("\n=== Ejecutando Shortest Job First ===\n");
    sjf_scheduler(ships, ship_count);

    printf("\n=== Ejecutando First-Come-First-Served (FCFS) ===\n");
    fcfs_scheduler(ships, ship_count);

    printf("\n=== Ejecutando Real-Time Scheduler (RTS) ===\n");
    real_time_scheduler(ships, ship_count);

    return 0;
}
