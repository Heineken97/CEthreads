/*
 * File: flow_manager.h
 *
 */
#ifndef FLOW_MANAGER_H
#define FLOW_MANAGER_H


// #include "CEthreads.h"
#include <pthread.h>
#include "ship.h"
#include "schedulers.h"

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

    SchedulerType scheduler;        // Calendarizador que ordenara los Ships
    // Initialized in load_configuration(filename) @ main.c
    FlowMethod method;              // Metodo por el cual se regira el programa
    // Initialized in load_configuration(filename) @ main.c
    int canal_length;               // Cantidad de espacios que cubrira el canal
    // Initialized in ready_up_canal() @ main.c
    int current_direction;          // Direccion actual desde donde estan saliendo los barcos
    // Initialized in ready_up_canal() @ main.c
    int ships_passed_this_cycle;    // Barcos que han pasado antes de alternar
    // Initialized in ready_up_canal() @ main.c
    int total_ships_passed;         // Barcos que han pasado en total
    // Initialized in ready_up_canal() @ main.c
    pthread_mutex_t canal_spaces[MAX_LENGTH];   // Mutexes para cada espacio del canal
    // Initialized in ready_up_canal() @ main.c
    int space_state[MAX_LENGTH];    // Estado de los espacios del canal: 0 = libre, 1 = ocupado
    /* Ship's speeds*/
    // Initialized in load_configuration(filename) @ main.c
    int normal_ship_speed;          // Velocidad del Normal Ship
    // Initialized in load_configuration(filename) @ main.c
    int fishing_ship_speed;         // Velocidad del Fishing Ship
    // Initialized in load_configuration(filename) @ main.c
    int patrol_ship_speed;          // Velocidad del Patrol Ship
    /* Arrays of Ships */
    // Initialized in load_configuration(filename) @ main.c
    int ships_in_queue_LR;          // Cantidad de Ships en queue_LR
    Ship queue_LR[MAX_SHIPS];       // Array de Ships en cola (Izq -> Der)
    // Initialized in ready_up_canal() @ main.c
    pthread_mutex_t ship_queues;
    // Initialized in load_configuration(filename) @ main.c
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
    /* Schedulers queue */
    int queues_changed;             // Dice si han cambiado los queues
    // Initialized in ready_up_canal() @ main.c
    pthread_mutex_t next_ship_mutex;
    int next_ship;
    int scheduled_queue_LR[MAX_SHIPS];  // Array con los ids de los Ships ordenados (Izq -> Der)
    int scheduled_queue_RL[MAX_SHIPS];  // Array con los ids de los Ships ordenados (Der -> Izq)
    /* EQUITY */
    // Initialized in load_configuration(filename) @ main.c
    int w_param;                    // Cantidad de barcos que pasan por direccion antes de alternar
    // Initialized in ready_up_canal() @ main.c
    int ids_for_canal[MAX_SHIPS];   // Ids de Ships que estan permitidos a pasar por el canal
    // Initialized in ready_up_canal() @ main.c
    int ships_for_cycle_ready;
    /* SIGN */
    // Initialized in load_configuration(filename) @ main.c
    float t_param;                  // Cantidad de tiempo que pasa antes de alternar la direccion    

} FlowManager;

// Declaraciones de funciones para cada método de flujo
void equity_flow(int W);                // Método de flujo: Equidad
void sign_flow(int change_time);        // Método de flujo: Letrero
void tico_flow();                       // Método de flujo: Tico

// Función para convertir un string a FlowMethod
FlowMethod get_flow_method(const char* method_name);

// Function that gets how many ships are yet to cross
int get_ships_yet_to_cross(FlowManager* flow_manager);

// Function to manage the canal and the Ships that flows through it
void* manage_canal(void* arg);

// Function that determines if the ship is allowed to advance 
int ship_can_advance(Ship* ship, FlowManager* flow_manager);

//
void move_ships_to_done(FlowManager* flow_manager);

#endif // FLOW_MANAGER_H
