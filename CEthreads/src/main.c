//#include "CEthreads.h"
//#include "schedulers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ship.h"

#define FLOW_METHOD 'None'

/* Ship speeds */
#define NORMAL_SPEED  1
#define FISHING_SPEED 2
#define PATROL_SPEED  3

#define W_PARAM 0

/* Canal parameters   */
#define CANAL_LENGTH 10  // Length of canal

/*
 * Function: main
 * --------------
 * Entry point for the program.
 * 
 */
int main() {

    // leer el archivo de configuracion
    // define:
    //      metodo de control de flujo
    //      largo de canal
    //      velocidad de barcos
    //      cantidad de barcos en cola
    //      tiempo del cambio de letrero
    //      parametro W

    /*
     * Ship tests
     */
    // Create some ships
    Ship* ship1 = create_ship(1, NORMAL, 0, 1, 5);
    Ship* ship2 = create_ship(2, FISHING, 1, 2, 3);
    Ship* ship3 = create_ship(3, PATROL, 0, 4, 10);


    // Print initial ship details
    printf("Initial state of the ship:\n");
    print_ship(ship1);

    // Initialize the ship's starting position
    ship1->position = 0;  // Start at position 0 in the canal

    // Simulate the ship moving through the canal
    printf("Simulating ship movement through the canal...\n\n");
    for (int i = 1; i <= CANAL_LENGTH; i++) {
        update_position(ship1, i);  // Update the ship's position as it moves
        print_ship(ship1);           // Print the ship's details after moving
    }

    // The ship has reached the end of the canal
    printf("The ship has reached the end of the canal.\n\n");

    // Free allocated memory for the ship
    free_ship(ship1);

    return 0;
}


/*
 * Function: function_name
 * ----------------------
 * what the function does
 * 
 * args: arg explanation
 * 
 * returns: what the function returns
 */