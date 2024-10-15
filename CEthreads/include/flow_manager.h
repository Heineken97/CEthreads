/*
 * File: flow_manager.h
 *
 */
#ifndef FLOW_MANAGER_H
#define FLOW_MANAGER_H


#include "CEthreads.h"
#include "CEmutex.h"
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

    // 
    int interface_serial_port;      // Descriptor de archivo para el puerto serial de la interfaz
    //
    int hardware_serial_port;       // Descriptor de archivo para el puerto serial del hardware
    //
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
    //CEmutex_t canal_spaces[MAX_LENGTH];   // Mutexes para cada espacio del canal
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
    //CEmutex_t ship_queues;
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
    //CEmutex_t next_ship_mutex;
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

// Structure for interface data (destination 0)
typedef struct {
    SimpleShip queue_LR_data[MAX_SHIPS];        // Ships in queue LR scheduled
    SimpleShip queue_RL_data[MAX_SHIPS];        // Ships in queue RL scheduled
    SimpleShip midcanal_data[MAX_SHIPS];        // Ships in midcanal (current direction only)
    SimpleShip done_LR_ships[MAX_SHIPS];        // Completed ships in LR
    SimpleShip done_RL_ships[MAX_SHIPS];        // Completed ships in RL
    int canal_length;                           // Length of the canal
    int current_direction;                       // Current direction
    FlowMethod method;                          // Flow control method (int)
    SchedulerType scheduler;                    // Scheduling algorithm (int)
} InterfaceData;

// Structure for hardware data (destination 1)
typedef struct {
    SimpleShip not_done_data[MAX_SHIPS];        // Queue and midcanal data
} HardwareData;

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

// Moves ships from the corresponding queue to the appropriate done array 
void move_ships_to_done(FlowManager* flow_manager);

// Defines the quantity of ships to complete their journey this cycle
int get_ships_on_cycle(FlowManager* flow_manager);

//
void transmit_canal_data(FlowManager* flow_manager, int destination);

// Function that determines if the ship is allowed to advance 
int ship_can_advance(Ship* ship, FlowManager* flow_manager);

// Checks if ship is next on the canal
int ship_is_next(Ship* ship, FlowManager* flow_manager);

#endif // FLOW_MANAGER_H
