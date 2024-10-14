/*
 * File: ship.c
 *
 */
#include <stdio.h>
#include <stdlib.h>
// #include "CEthreads.h"
#include <pthread.h>
#include <unistd.h>

#include "ship.h"
#include "flow_manager.h"

#define BASE_TIME 1000000

/*
 * Function: create_ship
 * ----------------------
 * Creates a new ship.
 * 
 * Parameters:
 * - id: ship's unique identifier
 * - type: ship's type (NORMAL, FISHING, PATROL)
 * - direction: 0 -> left to right, 1 -> right to left)
 * - speed: ship's speed depending on its type
 * - priority: level of priority for scheduling algorithms
 * 
 * Returns:
 *   Pointer to Ship struct
 */
Ship* create_ship(int id, ShipType type, int direction, double speed) {
    Ship* new_ship = (Ship*) malloc(sizeof(Ship));  // Allocate memory for the ship
    if (new_ship == NULL) {
        perror("Error allocating memory for the ship");
        exit(EXIT_FAILURE);
    }

    new_ship->id = id;
    new_ship->type = type;
    new_ship->direction = direction;
    new_ship->speed = speed;
    new_ship->priority = -1;
    new_ship->position = 0;
    new_ship->is_done = 0;

    return new_ship;
}

/*
 * Function: print_ship
 * ----------------------
 * Prints ship information (for debugging purposes).
 * 
 * Parameters:
 * - ship: pointer to Ship struct
 * 
 */
void print_ship(Ship* ship) {
    const char* type_str[] = {"Normal", "Fishing", "Patrol"};
    const char* direction_str = ship->direction == 0 ? "Left to Right" : "Right to Left";

    printf("Ship ID: %d\n", ship->id);
    printf("Type: %s\n", type_str[ship->type]);
    printf("Direction: %s\n", direction_str);
    printf("Speed: %.2f\n", ship->speed);
    printf("Priority: %d\n", ship->priority);
    printf("Actual position: %d\n", ship->position);
    printf("\n");
}

/*
 * Function: free_ship
 * ----------------------
 * Frees the memory allocated for a ship.
 * 
 * Parameters:
 * - ship: pointer to Ship struct
 * 
 */
void free_ship(Ship* ship) {
    free(ship);
}







/*
 * Function: move_ship
 * -------------------
 * Simulates the movement of a ship through the canal.
 * Each ship checks if the next space is free before advancing.
 * 
 * Parameters:
 * - arg: pointer to a struct containing both the Ship and the Flow_Manager (passed as void*
 *        for thread compatibility)
 * 
 * Notes:
 * - Ships cannot overtake each other and must wait until the next space is free.
 */
void* move_ship(void* arg) {
    struct {
        Ship* ship;
        FlowManager* flow_manager;
    } *context = arg;

    Ship* ship = context->ship;
    FlowManager* flow_manager = context->flow_manager;
    int next_position;

    // int actual_ship_id = ship->id;

    while (ship->position < flow_manager->canal_length) {

        // Simular el tiempo que tarda en moverse según la velocidad
        usleep(BASE_TIME / ship->speed);  // Esperar en microsegundos (según la velocidad)

        // Verificar si el barco puede avanzar
        while (!ship_can_advance(&ship, flow_manager)) {
            printf("Barco %d esperando permiso para avanzar.\n", ship->id);
            usleep(BASE_TIME / ship->speed);  // Esperar antes de volver a verificar
        }



        // Gets the position that the Ship is trying to access
        next_position = ship->position + 1;

        // Intentar moverse al siguiente espacio
        pthread_mutex_lock(&flow_manager->canal_spaces[next_position]); // Bloquea el mutex

        // Si la direccion es igual a 0 (I->D)
        if (flow_manager->current_direction == 0) {

            // Verifica si el espacio está libre
            if (flow_manager->space_state[next_position] == 0) {

                // Como no hay Ships en este, ocupa el espacio
                flow_manager->space_state[next_position] = 1;
                printf("Barco %d ocupando la posición %d en direccion %d\n",
                        ship->id, next_position, flow_manager->current_direction);
                pthread_mutex_unlock(&flow_manager->canal_spaces[next_position]);   // Desbloquea el mutex

                // Actualiza la posicion del struct del Ship
                ship->position = next_position;

                // Liberar el espacio anterior
                if (ship->position > 1) {
                    pthread_mutex_lock(&flow_manager->canal_spaces[ship->position - 1]);
                    flow_manager->space_state[ship->position - 1] = 0;  // Liberar el espacio anterior
                    printf("Barco %d liberando la posición %d en direccion %d\n",
                            ship->id, ship->position - 1, flow_manager->current_direction);
                    pthread_mutex_unlock(&flow_manager->canal_spaces[ship->position - 1]);
                }

            } else {
                // Si el espacio está ocupado, liberar el mutex y esperar antes de intentar de nuevo
                pthread_mutex_unlock(&flow_manager->canal_spaces[next_position]);
                printf("Barco %d esperando la posición %d en direccion %d\n",
                        ship->id, next_position, flow_manager->current_direction);
            }

        }
        // Si la direccion es igual a 1 (D->I)
        else if (flow_manager->current_direction == 1) {

            // Verifica si el espacio está libre
            if (flow_manager->space_state_RL[next_position] == 0) {

                // Como no hay Ships en este, ocupa el espacio
                flow_manager->space_state_RL[next_position] = 1;
                printf("Barco %d ocupando la posición %d en direccion %d\n",
                        ship->id, next_position, flow_manager->current_direction);
                pthread_mutex_unlock(&flow_manager->canal_spaces[next_position]);   // Desbloquea el mutex

                // Actualiza la posicion del struct del Ship
                ship->position = next_position;

                // Liberar el espacio anterior
                if (ship->position > 1) {
                    pthread_mutex_lock(&flow_manager->canal_spaces[ship->position - 1]);
                    flow_manager->space_state_RL[ship->position - 1] = 0;  // Liberar el espacio anterior
                    printf("Barco %d liberando la posición %d en direccion %d\n",
                            ship->id, ship->position - 1, flow_manager->current_direction);
                    pthread_mutex_unlock(&flow_manager->canal_spaces[ship->position - 1]);
                }

            } else {
                // Si el espacio está ocupado, liberar el mutex y esperar antes de intentar de nuevo
                pthread_mutex_unlock(&flow_manager->canal_spaces[next_position]);
                printf("Barco %d esperando la posición %d en direccion %d\n",
                        ship->id, next_position, flow_manager->current_direction);
            }

        }
        // Error en la direccion
        else {
            pthread_mutex_unlock(&flow_manager->canal_spaces[next_position]);
            printf("ERROR @ ship.c (move_ship): undefined flow_manager->current_direction @ first ifs\n");
            return 0;
        }

    }


    // Desbloquear el último mutex cuando el barco termine su recorrido
    // if (ship->position >= flow_manager->canal_length) {


    // Si la direccion es igual a 0 (I->D)
    if (flow_manager->current_direction == 0) {

        // Libera la posicion en la que está
        pthread_mutex_lock(&flow_manager->canal_spaces[ship->position]);
        flow_manager->space_state[ship->position] = 0;  // Liberar el último espacio
        printf("Barco %d liberando la posición %d en direccion %d\n",
                ship->id, ship->position, flow_manager->current_direction);
        pthread_mutex_unlock(&flow_manager->canal_spaces[ship->position]);

    }
    // Si la direccion es igual a 1 (D->I)
    else if (flow_manager->current_direction == 1) {

        // Libera la posicion en la que está
        pthread_mutex_lock(&flow_manager->canal_spaces[ship->position]);
        flow_manager->space_state_RL[ship->position] = 0;  // Liberar el último espacio
        printf("Barco %d liberando la posición %d en direccion %d\n",
                ship->id, ship->position, flow_manager->current_direction);
        pthread_mutex_unlock(&flow_manager->canal_spaces[ship->position]);

    }
    // Error en la direccion
    else {

        printf("ERROR @ ship.c (move_ship): undefined flow_manager->current_direction @ first ifs\n");
        return 0;
    }


    // Terminar recorrido
    
    pthread_mutex_lock(&flow_manager->ship_queues);  // Bloquea el mutex
    end_sail(&ship, flow_manager);
    pthread_mutex_unlock(&flow_manager->ship_queues);   // Desbloquea el mutex

    return NULL;
}
