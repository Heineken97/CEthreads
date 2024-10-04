/*
 * File: ship.c
 *
 */
#include "ship.h"
#include <stdio.h>
#include <stdlib.h>

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
Ship* create_ship(int id, ShipType type, int direction, double speed, int priority) {
    Ship* new_ship = (Ship*) malloc(sizeof(Ship));  // Allocate memory for the ship
    if (new_ship == NULL) {
        perror("Error allocating memory for the ship");
        exit(EXIT_FAILURE);
    }

    new_ship->id = id;
    new_ship->type = type;
    new_ship->direction = direction;
    new_ship->speed = speed;
    new_ship->priority = priority;
    new_ship->position = -1;


    return new_ship;
}

/*
 * Function: update_position
 * -------------------------
 * Updates the position of the ship in the canal.
 * 
 * Parameters:
 * - ship: pointer to the Ship struct
 * - new_position: the new position of the ship in the canal
 * 
 * Notes:
 * - This function modifies the position attribute of the Ship struct.
 * - Ensure that the ship pointer is valid (not NULL) before calling this function.
 */
void update_position(Ship* ship, int new_position) {
    if (ship == NULL) {
        perror("Ship pointer is NULL");
        exit(EXIT_FAILURE);
    }

    ship->position = new_position;
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
