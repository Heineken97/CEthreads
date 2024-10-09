/*
 * File: flow_methods.c
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "flow_methods.h"
#include "schedulers.h"
#include "ship.h"
#include "main.c"

// Función simulada que mueve un barco a través del canal
extern void simulate_ship_movement(Ship* ship, int canal_length);

/*
 * Function: equity_flow
 * ---------------------
 * Implementa el método de flujo de Equidad.
 * 
 * Parámetros:
 * - W: Número de barcos que deben pasar en cada dirección antes de cambiar el flujo.
 */
void equity_flow(int W) {
    printf("Método de flujo: Equidad. Permitimos que %d barcos pasen de cada lado.\n", W);

    int passed_ships_left = 0;   // Contador de barcos que pasaron de izquierda a derecha
    int passed_ships_right = 0;  // Contador de barcos que pasaron de derecha a izquierda

    while (passed_ships_left + passed_ships_right < ship_count) {
        // Permitir el paso de W barcos de izquierda a derecha
        int ships_in_current_round = 0;
        for (int i = 0; i < ship_count && ships_in_current_round < W; i++) {
            if (ships[i].direction == 0) {  // Barcos de izquierda a derecha (&& ships[i].position == -1)
                schedule();
                ships_in_current_round++;
                passed_ships_left++;
            }
        }

        // Permitir el paso de W barcos de derecha a izquierda
        ships_in_current_round = 0;
        for (int i = 0; i < ship_count && ships_in_current_round < W; i++) {
            if (ships[i].direction == 1) {  // Barcos de derecha a izquierda (&& ships[i].position == -1)
                schedule();
                ships_in_current_round++;
                passed_ships_right++;
            }
        }
    }
}

/*
 * Function: sign_flow
 * -------------------
 * Implementa el método de flujo controlado por letrero.
 * 
 * Parámetros:
 * - change_time: Tiempo en segundos para cambiar la dirección del letrero.
 */
void sign_flow(int change_time) {
    printf("Método de flujo: Letrero. El letrero cambia cada %d segundos.\n", change_time);

    int current_direction = 0;  // 0 = izquierda a derecha, 1 = derecha a izquierda
    int total_passed = 0;       // Número total de barcos procesados

    while (total_passed < ship_count) {
        int passed_ships = 0;
        printf("Letrero está en dirección: %s\n", current_direction == 0 ? "Izquierda a Derecha" : "Derecha a Izquierda");

        // Permitir paso de barcos en la dirección actual
        for (int i = 0; i < ship_count; i++) {
            if (ships[i].direction == current_direction && ships[i].position == -1) {
                schedule();
                passed_ships++;
                total_passed++;
            }
        }

        // Esperar el tiempo de cambio del letrero
        sleep(change_time);
        // Cambiar la dirección del letrero
        current_direction = 1 - current_direction;
    }
}

/*
 * Function: tico_flow
 * -------------------
 * Implementa el método de flujo "Tico".
 * 
 * Notas:
 * - No hay control explícito de flujo, pero evita colisiones permitiendo alternancia.
 * - Permite el flujo continuo si sólo hay barcos de un lado.
 */
void tico_flow() {
    printf("Método de flujo: Tico. No hay control de flujo explícito, pero evitamos colisiones.\n");

    int left_count = 0;
    int right_count = 0;

    while (left_count + right_count < ship_count) {
        int passed_ship = 0;

        // Priorizar barcos de izquierda a derecha si no hay conflicto
        for (int i = 0; i < ship_count && !passed_ship; i++) {
            if (ships[i].direction == 0 && ships[i].position == -1) {  // Izquierda a derecha
                schedule();
                left_count++;
                passed_ship = 1;  // Indicar que un barco ha pasado
            }
        }

        // Si no hay barcos de izquierda, permitir barcos de derecha a izquierda
        for (int i = 0; i < ship_count && !passed_ship; i++) {
            if (ships[i].direction == 1 && ships[i].position == -1) {  // Derecha a izquierda
                schedule();
                right_count++;
                passed_ship = 1;
            }
        }
    }
}

/*
 * Function: get_flow_method
 * -------------------------
 * Converts a string representing a flow control method into the corresponding FlowMethod enum.
 * 
 * Parameters:
 * - method_name: string that represents the flow method (e.g., "EQUITY", "SIGN", "TICO")
 * 
 * Returns:
 * - The corresponding FlowMethod enum value (EQUITY, SIGN, TICO).
 * 
 * Notes:
 * - If the provided method_name is invalid, the function prints an error message and exits the program.
 * - Ensure that the method_name matches one of the predefined flow method names ("EQUITY", "SIGN", "TICO").
 */
FlowMethod get_flow_method(const char* method_name) {
    if (strcmp(method_name, "EQUITY") == 0) {
        return EQUITY;
    } else if (strcmp(method_name, "SIGN") == 0) {
        return SIGN;
    } else if (strcmp(method_name, "TICO") == 0) {
        return TICO;
    } else {
        printf("Error: Método de flujo no válido.\n");
        exit(EXIT_FAILURE);
    }
}
