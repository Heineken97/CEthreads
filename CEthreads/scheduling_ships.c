#include "CEthreads.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_SHIPS 100
#define CHANNEL_LENGTH 10
#define NORMAL_SPEED 2
#define PESQUERA_SPEED 1
#define PATRULLA_SPEED 0.5
#define MAX_QUEUE_SIZE 10

typedef enum {
    NORMAL,
    PESQUERA,
    PATRULLA
} ShipType;

typedef struct Ship {
    ShipType type;
    int position; // Posición en el canal
    CEthread thread; // Hilo asociado al barco
} Ship;

typedef struct Channel {
    int length;
    float speed; // Velocidad del barco
    Ship *right_queue[MAX_QUEUE_SIZE]; // Cola de barcos que llegan desde la derecha
    Ship *left_queue[MAX_QUEUE_SIZE]; // Cola de barcos que llegan desde la izquierda
    int right_front;
    int right_rear;
    int left_front;
    int left_rear;
} Channel;

Ship ships[MAX_SHIPS];
int ship_count = 0;
Channel canal;

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

    while (ship->position < canal.length) {
        printf("Barco de tipo %d en posición %d\n", ship->type, ship->position);
        sleep(speed); // Simular tiempo de movimiento
        ship->position++;
    }
    printf("Barco de tipo %d ha salido del canal\n", ship->type);
}

// Generar barco con carga definida
void create_ship(ShipType type, int direction) {
    if (ship_count >= MAX_SHIPS) {
        printf("Límite de barcos alcanzado.\n");
        return;
    }

    Ship *new_ship = &ships[ship_count++];
    new_ship->type = type;
    new_ship->position = 0; // Posición inicial

    // Crear hilo para el barco
    CEthread_create(&new_ship->thread, ship_behavior, new_ship);

    // Añadir el barco a la cola correspondiente
    if (direction == 0) { // Derecha
        canal.right_queue[canal.right_rear] = new_ship;
        canal.right_rear = (canal.right_rear + 1) % MAX_QUEUE_SIZE;
	printf("Barco creado desde la derecha: tipo %d\n", type);
    } else { // Izquierda
        canal.left_queue[canal.left_rear] = new_ship;
        canal.left_rear = (canal.left_rear + 1) % MAX_QUEUE_SIZE;
	printf("Barco creado desde la izquierda: tipo %d\n", type);
    }
}

// Procesar barcos en la cola
void process_queue() {
    while (canal.right_front != canal.right_rear || canal.left_front != canal.left_rear) {
        if (canal.right_front != canal.right_rear) {
            Ship *ship = canal.right_queue[canal.right_front];
            canal.right_front = (canal.right_front + 1) % MAX_QUEUE_SIZE;
            CEthread_join(&ship->thread); // Esperar a que termine
        }

        if (canal.left_front != canal.left_rear) {
            Ship *ship = canal.left_queue[canal.left_front];
            canal.left_front = (canal.left_front + 1) % MAX_QUEUE_SIZE;
            CEthread_join(&ship->thread); // Esperar a que termine
	}
    }
}

int main() {
    // Inicializar el canal
    canal.length = CHANNEL_LENGTH;
    canal.speed = 1.0; // Velocidad base
    canal.right_front = 0;
    canal.right_rear = 0;
    canal.left_front = 0;
    canal.left_rear = 0;

    // Generar varios barcos con entrada de usuario
    create_ship(NORMAL, 0); // Barco desde la derecha
    create_ship(PESQUERA, 1); // Barco desde la izquierda
    create_ship(PATRULLA, 0); // Barco desde la derecha
    create_ship(NORMAL, 1); // Otro barco desde la izquierda
    create_ship(PESQUERA, 0); // Otro barco desde la derecha

    // Procesar la cola de barcos
    process_queue();

    return 0;
}

