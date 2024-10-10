/*
 * File: flow_manager.h
 *
 */
#ifndef FLOW_MANAGER_H
#define FLOW_MANAGER_H

#include "ship.h"
// #include "CEthreads.h"
#include <pthread.h>

#define MAX_SHIPS 100
#define MAX_LENGTH 100

// Definición del enum para los métodos de flujo
typedef enum {
    NONE,           // Undefined
    EQUITY,         // Equidad
    SIGN,           // Letrero
    TICO            // Tico
} FlowMethod;

// Structure that represents a flow manager
typedef struct {

    FlowMethod method;              // Metodo por el cual se regira el programa
    int canal_length;               // Cantidad de espacios que cubrira el canal
    int current_direction;          // Direccion actual desde donde estan saliendo los barcos
    int ships_passed;               // Barcos que han pasado antes de alternar
    int total_ships_passed;         // Barcos que han pasado en total
    pthread_mutex_t canal_spaces[MAX_LENGTH];   // Mutexes para cada espacio del canal
    int space_state[MAX_LENGTH];    // Estado de los espacios del canal: 0 = libre, 1 = ocupado
    /* Ship's speeds*/
    int normal_ship_speed;          // Velocidad del Normal Ship
    int fishing_ship_speed;         // Velocidad del Fishing Ship
    int patrol_ship_speed;          // Velocidad del Patrol Ship
    /* Arrays of Ships */
    int ships_in_queue_LR;          // Cantidad de Ships en queue_LR
    Ship queue_LR[MAX_SHIPS];       // Array de Ships en cola (Izq -> Der)
    int ships_in_queue_RL;          // Cantidad de Ships en queue_RL
    Ship queue_RL[MAX_SHIPS];       // Array de Ships en cola (Der -> Izq)
    int ships_in_midcanal_LR;       // Cantidad de Ships en midcanal_LR
    Ship midcanal_LR[MAX_SHIPS];    // Array de Ships en medio canal (Izq -> Der)
    int ships_in_midcanal_RL;       // Cantidad de Ships en midcanal_RL
    Ship midcanal_RL[MAX_SHIPS];    // Array de Ships en medio canal (Der -> Izq)
    int ships_in_done_LR;           // Cantidad de Ships en done_LR
    Ship done_LR[MAX_SHIPS];        // Array de Ships que ya pasaron el canal (Izq -> Der)
    int ships_in_done_RL;           // Cantidad de Ships en done_RL
    Ship done_RL[MAX_SHIPS];        // Array de Ships que ya pasaron el canal (Der -> Izq)
    /* EQUITY */
    int w_param;                    // Cantidad de barcos que pasan por direccion antes de alternar
    /* SIGN */
    float t_param;                  // Cantidad de tiempo que pasa antes de alternar la direccion    

} FlowManager;

// Declaraciones de funciones para cada método de flujo
void equity_flow(int W);                // Método de flujo: Equidad
void sign_flow(int change_time);        // Método de flujo: Letrero
void tico_flow();                       // Método de flujo: Tico

// Función para convertir un string a FlowMethod
FlowMethod get_flow_method(const char* method_name);

#endif // FLOW_MANAGER_H