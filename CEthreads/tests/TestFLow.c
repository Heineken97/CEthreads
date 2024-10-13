#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sleep function

// Constants
#define MAX_SHIPS 10
#define CANAL_LENGTH 100

// Ship structure
typedef struct Ship {
    int id;          // Ship ID
    int direction;   // 0 for left-to-right, 1 for right-to-left
    int position;    // -1 means the ship hasn't entered the canal yet
} Ship;

// Simulate global variables
Ship ships[MAX_SHIPS];
int ship_count = 0;

// Function declarations
void equity_flow(int W);
void sign_flow(int change_time);
void tico_flow();
void simulate_ship_movement(Ship* ship, int canal_length);

// Dummy simulate_ship_movement function
void simulate_ship_movement(Ship* ship, int canal_length) {
    // Move the ship to the end of the canal
    ship->position = canal_length;
    printf("Ship %d moved through the canal. Direction: %s\n", 
           ship->id, ship->direction == 0 ? "Left to Right" : "Right to Left");
}

// Initialize ships for testing
void initialize_ships() {
    ship_count = 6; // Example: 6 ships
    for (int i = 0; i < ship_count; i++) {
        ships[i].id = i;
        ships[i].position = -1; // Initial position before entering the canal
        ships[i].direction = i % 2; // Alternating directions for test
    }
}

// Function: equity_flow
void equity_flow(int W) {
    printf("Método de flujo: Equidad. Permitimos que %d barcos pasen de cada lado.\n", W);

    int passed_ships_left = 0;   // Count ships from left to right
    int passed_ships_right = 0;  // Count ships from right to left

    while (passed_ships_left + passed_ships_right < ship_count) {
        // Allow W ships from left to right
        int ships_in_current_round = 0;
        for (int i = 0; i < ship_count && ships_in_current_round < W; i++) {
            if (ships[i].direction == 0 && ships[i].position == -1) {  // Left to right ships
                simulate_ship_movement(&ships[i], CANAL_LENGTH);
                ships_in_current_round++;
                passed_ships_left++;
            }
        }

        // Allow W ships from right to left
        ships_in_current_round = 0;
        for (int i = 0; i < ship_count && ships_in_current_round < W; i++) {
            if (ships[i].direction == 1 && ships[i].position == -1) {  // Right to left ships
                simulate_ship_movement(&ships[i], CANAL_LENGTH);
                ships_in_current_round++;
                passed_ships_right++;
            }
        }
    }
}

// Function: sign_flow
void sign_flow(int change_time) {
    printf("Método de flujo: Letrero. El letrero cambia cada %d segundos.\n", change_time);

    int current_direction = 0;  // 0 = Left to right, 1 = Right to left
    int total_passed = 0;       // Total ships processed

    while (total_passed < ship_count) {
        int passed_ships = 0;
        printf("Letrero está en dirección: %s\n", current_direction == 0 ? "Izquierda a Derecha" : "Derecha a Izquierda");

        // Allow ships in the current direction
        for (int i = 0; i < ship_count; i++) {
            if (ships[i].direction == current_direction && ships[i].position == -1) {
                simulate_ship_movement(&ships[i], CANAL_LENGTH);
                passed_ships++;
                total_passed++;
            }
        }

        // Wait for the sign to change
        sleep(change_time);
        // Change the direction of the sign
        current_direction = 1 - current_direction;
    }
}

// Function: tico_flow
void tico_flow() {
    printf("Método de flujo: Tico. No hay control de flujo explícito, pero evitamos colisiones.\n");

    int left_count = 0;
    int right_count = 0;

    while (left_count + right_count < ship_count) {
        int passed_ship = 0;

        // Prioritize left-to-right ships if no conflict
        for (int i = 0; i < ship_count && !passed_ship; i++) {
            if (ships[i].direction == 0 && ships[i].position == -1) {  // Left to right
                simulate_ship_movement(&ships[i], CANAL_LENGTH);
                left_count++;
                passed_ship = 1;  // Indicate that a ship has passed
            }
        }

        // If no left-to-right ships passed, allow right-to-left ships
        for (int i = 0; i < ship_count && !passed_ship; i++) {
            if (ships[i].direction == 1 && ships[i].position == -1) {  // Right to left
                simulate_ship_movement(&ships[i], CANAL_LENGTH);
                right_count++;
                passed_ship = 1;  // Indicate that a ship has passed
            }
        }
    }
}

// Test equity_flow
void test_equity_flow() {
    printf("\n=== Testing equity_flow ===\n");
    initialize_ships(); // Setup ships
    
    int W = 2;  // Allow 2 ships to pass per side
    equity_flow(W);  // Call the equity flow function
}

// Test sign_flow
void test_sign_flow() {
    printf("\n=== Testing sign_flow ===\n");
    initialize_ships(); // Setup ships
    
    int change_time = 1;  // Switch direction every second
    sign_flow(change_time);  // Call the sign flow function
}

// Test tico_flow
void test_tico_flow() {
    printf("\n=== Testing tico_flow ===\n");
    initialize_ships(); // Setup ships
    
    tico_flow();  // Call the tico flow function
}

// Main function to run all tests
int main() {
    test_equity_flow();  // Test equity flow
    test_sign_flow();    // Test sign flow
    test_tico_flow();    // Test tico flow
    
    return 0;
}
