/*
 * File: flow_methods.c
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "flow_methods.h"

// Simulación del método de flujo "Equidad"
void equity_flow(int W) {
    printf("Método de flujo: Equidad. Permitimos que %d barcos pasen de cada lado.\n", W);
}

// Simulación del método de flujo "Letrero"
void sign_flow(int change_time) {
    printf("Método de flujo: Letrero. El letrero cambia cada %d segundos.\n", change_time);
}

// Simulación del método de flujo "Tico"
void tico_flow() {
    printf("Método de flujo: Tico. No hay control de flujo explícito, pero evitamos colisiones.\n");
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
