/*
 * File: flow_manager.c
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "flow_manager.h"


/*
 * Function: equity_flow
 * ---------------------
 * Simulates the EQUITY flow control method by allowing W_PARAM ships to pass before alternating directions.
 * 
 * Parameters:
 * - W: the number of ships that can pass before switching the direction
 */
void equity_flow(int W) {
    printf("Simulating EQUITY flow control: allowing %d ships to pass before switching direction.\n", W);
}

/*
 * Function: sign_flow
 * -------------------
 * Simulates the SIGN flow control method by changing directions after a specific time period.
 * 
 * Parameters:
 * - change_time: the time period after which the direction should change
 */
void sign_flow(int change_time) {
    printf("Simulating SIGN flow control: changing direction after %d seconds.\n", change_time);
}

/*
 * Function: tico_flow
 * -------------------
 * Simulates the TICO flow control method.
 */
void tico_flow() {
    printf("Simulating TICO flow control: alternating after each ship.\n");
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