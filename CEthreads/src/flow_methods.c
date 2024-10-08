#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "flow_methods.h"

// Simulación del método de flujo "Equidad"
void equity_flow(int W) {
    printf("Método de flujo: Equidad. Permitimos que %d barcos pasen de cada lado.\n", W);
    // Aquí agregarías la lógica para permitir que W barcos pasen antes de cambiar de lado.
}

// Simulación del método de flujo "Letrero"
void sign_flow(int change_time) {
    printf("Método de flujo: Letrero. El letrero cambia cada %d segundos.\n", change_time);
    // Aquí agregarías la lógica para cambiar el sentido del flujo basado en el tiempo.
}

// Simulación del método de flujo "Tico"
void tico_flow() {
    printf("Método de flujo: Tico. No hay control de flujo explícito, pero evitamos colisiones.\n");
    // Aquí agregarías la lógica que evita colisiones sin un control de flujo específico.
}

// Implementación de la función para convertir un string a FlowMethod
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
