/*
 * Instituto Tecnologico de Costa Rica
 * Escuela de Ingenieria en Computadores
 * Principios de Sistemas Operativos
 * Proyecto 1: Scheduling Ships (Calendarizador de Hilos)
 * 
 * File: main.c
 * Archivo principal del programa
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//#include "CEthreads.h"
#include "ship.h"
#include "schedulers.h"
#include "flow_manager.h"


#define MAX_SHIPS 100

/* ----------------------------- Configuration      variables ----------------------------- */

/* Program variable to start/stop execution */
int canal_is_open = 0;

/* Ships created */
int ship_count = 0;

/* Flow Manager */
FlowManager flow_manager;

// Crear el hilo para gestionar el canal
pthread_t canal_thread;

/* ---------------------------------------------------------------------------------------- */


/*
 * Function: load_configuration
 * ----------------------------
 * Reads the configuration file and assigns the corresponding values to global variables.
 * Processes flow method, canal length, ship speeds, and a list of ships with types and
 * directions.
 * 
 * Parameters:
 * - filename: name of the configuration file to be read
 * 
 * Notes:
 * - The function reads each line of the configuration file, skipping comments and blank lines.
 * - Ships are created based on the configuration format (e.g., ship(NORMAL,0)).
 * - If the ship count exceeds the maximum allowed (`MAX_SHIPS`), the function prints an error.
 * - The function modifies global variables such as FLOW_METHOD, CANAL_LENGTH, and ships array.
 */
void load_configuration(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error al abrir el archivo de configuración");
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Ignorar líneas vacías y comentarios
        if (line[0] == '\n' || line[0] == '#') {
            continue;
        }

        // Leer las claves principales del archivo
        if (strncmp(line, "flow_method:", 12) == 0) {
            char method_str[20];
            sscanf(line, "flow_method: %s", method_str);
            flow_manager.method = get_flow_method(method_str);
        } else if (strncmp(line, "canal_length:", 13) == 0) {
            sscanf(line, "canal_length: %d", &flow_manager.canal_length);
        } else if (strncmp(line, "normal_speed:", 13) == 0) {
            sscanf(line, "normal_speed: %d", &flow_manager.normal_ship_speed);
        } else if (strncmp(line, "fishing_speed:", 14) == 0) {
            sscanf(line, "fishing_speed: %d", &flow_manager.fishing_ship_speed);
        } else if (strncmp(line, "patrol_speed:", 13) == 0) {
            sscanf(line, "patrol_speed: %d", &flow_manager.patrol_ship_speed);
        } else if (strncmp(line, "w_param:", 8) == 0) {
            sscanf(line, "w_param: %d", &flow_manager.w_param);
        } else if (strncmp(line, "t_param:", 8) == 0) {
            sscanf(line, "t_param: %f", &flow_manager.t_param);
        } 
        // Procesar barcos con el formato ship(NORMAL,0)
        else if (strncmp(line, "ship(", 5) == 0) {
            if (ship_count >= MAX_SHIPS) {
                printf("Error: Demasiados barcos en la configuración.\n");
                break;
            }

            if (!flow_manager.queues_changed) {
                flow_manager.queues_changed = 1;
            }

            // Leer el tipo de barco y la dirección dentro de ship(...)
            char type_str[10];
            int direction = -1;

            sscanf(line, "ship(%[^,],%d)", type_str, &direction);

            // Convertir el tipo de barco a ShipType y asignar la velocidad correspondiente
            ShipType type;
            double speed = 0.0;
            if (strcmp(type_str, "NORMAL") == 0) {
                type = NORMAL;
                speed = flow_manager.normal_ship_speed;
            } else if (strcmp(type_str, "FISHING") == 0) {
                type = FISHING;
                speed = flow_manager.fishing_ship_speed;
            } else if (strcmp(type_str, "PATROL") == 0) {
                type = PATROL;
                speed = flow_manager.patrol_ship_speed;
            } else {
                printf("Error: Tipo de barco no válido: %s\n", type_str);
                continue;
            }

            // Validar la dirección
            if (direction != 0 && direction != 1) {
                printf("Error: Dirección no válida (%d) para el Ship\n", direction);
                continue;
            }

            int temp_pos;
            Ship* new_ship;

            // Agrega el Ship a su respectivo array
            if (direction == 0) {
                // Obtiene la cantidad de barcos en la lista para saber su posicion
                temp_pos = flow_manager.ships_in_queue_LR;
                // Crea un nuevo Ship y se referencia con este puntero
                new_ship = create_ship(ship_count+1, type, direction, speed);
                // Almacena el Ship en el array correspondiente asignando el valor al que apunta new_ship
                flow_manager.queue_LR[temp_pos] = *new_ship;
                // Aumenta la cantidad en la lista al haberlo agregado
                flow_manager.ships_in_queue_LR++;
                // Prints total ships in queue_LR
            }
            else if (direction == 1) {
                // Obtiene la cantidad de barcos en la lista para saber su posicion
                temp_pos = flow_manager.ships_in_queue_RL;
                // Crea un nuevo Ship y se referencia con este puntero
                new_ship = create_ship(ship_count+1, type, direction, speed);
                // Almacena el Ship en el array correspondiente asignando el valor al que apunta new_ship
                flow_manager.queue_RL[temp_pos] = *new_ship;
                // Aumenta la cantidad en la lista al haberlo agregado
                flow_manager.ships_in_queue_RL++;
                // Prints total ships in queue_RL  
            }

            ship_count++;  // Incrementar el contador total de barcos
        }
    }
    printf("Ships in queue_LR = %d\n", flow_manager.ships_in_queue_LR);
    printf("Ships in queue_RL = %d\n", flow_manager.ships_in_queue_RL);

    fclose(file);
}

/*
 * Alista el canal
 */
void ready_up_canal(){

    printf("\nInitializaing variables on flow_manager for canal...\n");

    // Inicialmente la primera direccion será de Izq a Der
    flow_manager.current_direction = 0;
    // Se inicializa la variable en 0
    flow_manager.ships_passed_this_cycle = 0;
    // Se inicializa la variable en 0
    flow_manager.total_ships_passed = 0;


    printf("\nInitializaing mutexes on canal spaces...\n");

    // Inicializar los mutexes para cada espacio del canal
    for (int i = 0; i <= flow_manager.canal_length; i++) {
        pthread_mutex_init(&flow_manager.canal_spaces[i], NULL);
        flow_manager.space_state[i] = 0;  // Espacios libres al inicio
    }

    printf("\nInitializaing variables on schedules...\n");
    pthread_mutex_init(&flow_manager.next_ship_mutex, NULL);
    flow_manager.next_ship = 0;  // Id que iniciará

    printf("\nInitializaing variables on flow_manager for flow methods...\n");

    pthread_mutex_init(&flow_manager.ship_queues, NULL);
    // Inicializar el array para saber los ids que van al canal en EQUITY
    // Ningun barco tendrá id 0 
    for (int i = 0; i < MAX_SHIPS; i++) {
        flow_manager.ids_for_canal[i] = 0;  // Ids en 0 al inicio
    }

    // Se inicializa la variable en 0 ya que no están listos
    flow_manager.ships_for_cycle_ready = 0;


    // Crea hilo para el flow_manager
    pthread_create(&canal_thread, NULL, manage_canal, &flow_manager);

    usleep(1000000);

    printf("\nOpening canal...\n");
    // Ships are now allowed to flow
    canal_is_open = 1;
    printf("\nShips are now allowed to flow...\n\n");
}


/*
 * Function: start_ships_in_queue
 * ------------------------------
 * Creates threads for each ship in a given queue and starts their movement in the canal.
 * 
 * Parameters:
 * - flow_manager: pointer to the Flow_Manager struct
 * - ships_queue: array of ships (either queue_LR or queue_RL)
 * - num_ships: number of ships in the queue
 */
void ready_up_ships() {
    // Estructura de contexto para pasar el barco y el flow_manager a cada hilo
    struct {
        Ship* ship;
        FlowManager* flow_manager;
    } contexts[MAX_SHIPS];  // Usar un array de estructuras para cada barco

    

    // Crear hilos para cada barco en queue_LR
    for (int i = 0; i < flow_manager.ships_in_queue_LR; i++) {
        contexts[i].ship = &flow_manager.queue_LR[i];        // Asignar el barco
        contexts[i].flow_manager = &flow_manager;  // Asignar el Flow_Manager

        // Crear un hilo para el barco actual
        pthread_create(&flow_manager.queue_LR[i].thread, NULL, move_ship, &contexts[i]);

        // usleep(1000000);
    }

    // Crear hilos para cada barco en queue_RL
    for (int i = 0; i < flow_manager.ships_in_queue_RL; i++) {
        contexts[i].ship = &flow_manager.queue_RL[i];        // Asignar el barco
        contexts[i].flow_manager = &flow_manager;  // Asignar el Flow_Manager

        // Crear un hilo para el barco actual
        pthread_create(&flow_manager.queue_RL[i].thread, NULL, move_ship, &contexts[i]);

        // usleep(1000000);
    }

    // Esperar a que todos los hilos terminen en queue_LR
    for (int i = 0; i < flow_manager.ships_in_queue_LR; i++) {
        pthread_join(flow_manager.queue_LR[i].thread, NULL);
    }

    // Esperar a que todos los hilos terminen en queue_RL
    for (int i = 0; i < flow_manager.ships_in_queue_RL; i++) {
        pthread_join(flow_manager.queue_RL[i].thread, NULL);
    }
}

/*
 * Cierra el canal
 */
void close_canal() {

    // Esperar a que el hilo del canal termine
    pthread_join(canal_thread, NULL);

    printf("\nDestroying mutexes on canal spaces...\n");

    // Destruir los mutexes
    for (int i = 0; i < flow_manager.canal_length; i++) {
        pthread_mutex_destroy(&flow_manager.canal_spaces[i]);
    }

    printf("\nClosing canal...\n");
    // Ships are not allowed to flow
    canal_is_open = 0;
    printf("\nShips are not allowed to flow anymore...\n");
}


/*
 * Function: send_state
 * --------------------
 * Simulates sending the current program state (ship data) to an interface and hardware module.
 * 
 * Notes:
 * - This function prints the current state of all ships in the system, including their ID, type,
 *   and direction.
 * - In future implementations, it will send actual data to a user interface and a hardware
 *   module.
 */
void send_state() {
    printf("\nSimulating sending program state...\n");

    printf("Array de Ships en cola (Izq -> Der):\n");
    for (int i = 0; i < flow_manager.ships_in_queue_LR; i++) {
        printf("Ship: ID = %d, Tipo = %d, Dirección = %d, Velocidad = %.1f\n", 
                    flow_manager.queue_LR[i].id, flow_manager.queue_LR[i].type,
                    flow_manager.queue_LR[i].direction, flow_manager.queue_LR[i].speed);
    }

    printf("Array de Ships en medio canal (Izq -> Der):\n");
    for (int i = 0; i < flow_manager.ships_in_midcanal_LR; i++) {
        printf("Ship: ID = %d, Tipo = %d, Dirección = %d, Velocidad = %.1f\n", 
                    flow_manager.midcanal_LR[i].id, flow_manager.midcanal_LR[i].type,
                    flow_manager.midcanal_LR[i].direction, flow_manager.midcanal_LR[i].speed);
    }

    printf("Array de Ships que ya pasaron el canal (Izq -> Der):\n");
    for (int i = 0; i < flow_manager.ships_in_done_LR; i++) {
        printf("Ship: ID = %d, Tipo = %d, Dirección = %d, Velocidad = %.1f\n", 
                    flow_manager.done_LR[i].id, flow_manager.done_LR[i].type,
                    flow_manager.done_LR[i].direction, flow_manager.done_LR[i].speed);
    }

    printf("Array de Ships en cola (Der -> Izq):\n");
    for (int i = 0; i < flow_manager.ships_in_queue_RL; i++) {
        printf("Ship: ID = %d, Tipo = %d, Dirección = %d, Velocidad = %.1f\n", 
                    flow_manager.queue_RL[i].id, flow_manager.queue_RL[i].type,
                    flow_manager.queue_RL[i].direction, flow_manager.queue_RL[i].speed);
    }

    printf("Array de Ships en medio canal (Der -> Izq):\n");
    for (int i = 0; i < flow_manager.ships_in_midcanal_RL; i++) {
        printf("Ship: ID = %d, Tipo = %d, Dirección = %d, Velocidad = %.1f\n", 
                    flow_manager.midcanal_LR[i].id, flow_manager.midcanal_LR[i].type,
                    flow_manager.midcanal_LR[i].direction, flow_manager.midcanal_LR[i].speed);
    }

    printf("Array de Ships que ya pasaron el canal (Der -> Izq):\n");
    for (int i = 0; i < flow_manager.ships_in_done_RL; i++) {
        printf("Ship: ID = %d, Tipo = %d, Dirección = %d, Velocidad = %.1f\n", 
                    flow_manager.done_RL[i].id, flow_manager.done_RL[i].type,
                    flow_manager.done_RL[i].direction, flow_manager.done_RL[i].speed);
    }
    
}



/*
 * Function: manage_new_ships
 * --------------------------
 * Simulates the addition of new ships by creating a new ship with random attributes and adding
 * it to the ships array.
 * 
 * Notes:
 * - The new ship is assigned an ID, type, direction, and speed. The type and direction are
 *   chosen randomly.
 * - The speed is determined based on the ship's type (NORMAL, FISHING, PATROL).
 * - The ship is added to the ships array, and the ship count is incremented.
 * - The position and priority of the new ship are initialized to default values (0 and -1
 *   respectively).
 * - For now, this function simulates adding ships, but in the future, it will obtain input from
 *   an interface (via key presses) to add new ships.
 */
void manage_new_ships() {
    
    printf("\nSimulating adding new ship...\n");

    // Asignar un tipo de barco aleatorio
    int random_type = rand() % 3;  // Valores entre 0 y 2
    ShipType random_ShipType = (ShipType)random_type;

    // Asignar una dirección aleatoria (0 o 1)
    int random_direction = rand() % 2;

    // Asignar la velocidad correspondiente al tipo de barco
    int speed;
    switch (random_ShipType) {
        case NORMAL:
            speed = flow_manager.normal_ship_speed;
            break;
        case FISHING:
            speed = flow_manager.fishing_ship_speed;
            break;
        case PATROL:
            speed = flow_manager.patrol_ship_speed;
            break;
    }

    int temp_pos;
    Ship* new_ship;

    // Agrega el Ship a su respectivo array
    if (random_direction == 0) {
        // Obtiene la cantidad de barcos en la lista para saber su posicion
        temp_pos = flow_manager.ships_in_queue_LR;
        // Crea un nuevo Ship y se referencia con este puntero
        new_ship = create_ship(temp_pos+1, random_ShipType, random_direction, speed);
        // Almacena el Ship en el array correspondiente asignando el valor al que apunta new_ship
        flow_manager.queue_LR[temp_pos] = *new_ship;
        // Aumenta la cantidad en la lista al haberlo agregado
        flow_manager.ships_in_queue_LR++;
        // Prints total ships in queue_LR
        printf("Ships in queue_LR = %d\n", flow_manager.ships_in_queue_LR);
    }
    else if (random_direction == 1) {
        // Obtiene la cantidad de barcos en la lista para saber su posicion
        temp_pos = flow_manager.ships_in_queue_RL;
        // Crea un nuevo Ship y se referencia con este puntero
        new_ship = create_ship(temp_pos+1, random_ShipType, random_direction, speed);
        // Almacena el Ship en el array correspondiente asignando el valor al que apunta new_ship
        flow_manager.queue_RL[temp_pos] = *new_ship;
        // Aumenta la cantidad en la lista al haberlo agregado
        flow_manager.ships_in_queue_RL++;
        // Prints total ships in queue_RL
        printf("Ships in queue_RL = %d\n", flow_manager.ships_in_queue_RL);
    }

    ship_count++;  // Incrementar el contador total de barcos
}



/*
 * Function: main
 * --------------
 * Entry point for the program.
 * 
 */
int main(int argc, char *argv[]) {

    // Se revisan los argumentos del programa a la hora de ejecutarlo
    if (argc != 2) {
        printf("Usage: %s <scheduler_type>\n", argv[0]);
        printf("Available scheduler types: round_robin, priority, sjf, fcfs, real_time\n");
        return 1;
    }

    /* Asigna el argumento de la línea de comandos a SchedulerType */
    flow_manager.scheduler = get_scheduler_type(argv[1]);
    printf("\nParametro obtenido al correr el programa:\n");
    printf("Calendarizador: %d\n\n", flow_manager.scheduler);


    /* Lee y carga la configuración desde el archivo */
    load_configuration("config/canal_config.txt");

    // Se imprimen las variables leídas para verificar
    printf("\nVariables leidas del archivo de configuracion:\n");
    printf("Metodo de flujo: %d\n", flow_manager.method);
    printf("Largo del canal: %d\n", flow_manager.canal_length);
    printf("Velocidad de barcos NORMAL: %d\n", flow_manager.normal_ship_speed);
    printf("Velocidad de barcos FISHING: %d\n", flow_manager.fishing_ship_speed);
    printf("Velocidad de barcos PATROL: %d\n", flow_manager.patrol_ship_speed);
    printf("Tiempo de cambio del letrero (Parametro T): %.2f\n", flow_manager.t_param);
    printf("Parametro W: %d\n", flow_manager.w_param);
    // Se imprimen los barcos del archivo
    printf("Array de Ships en cola (Izq -> Der):\n");
    for (int i = 0; i < flow_manager.ships_in_queue_LR; i++) {
        printf("Ship: ID = %d, Tipo = %d, Dirección = %d, Velocidad = %.1f\n", 
                    flow_manager.queue_LR[i].id, flow_manager.queue_LR[i].type,
                    flow_manager.queue_LR[i].direction, flow_manager.queue_LR[i].speed);
    }
    printf("Array de Ships en cola (Der -> Izq):\n");
    for (int i = 0; i < flow_manager.ships_in_queue_RL; i++) {
        printf("Ship: ID = %d, Tipo = %d, Dirección = %d, Velocidad = %.1f\n", 
                    flow_manager.queue_RL[i].id, flow_manager.queue_RL[i].type,
                    flow_manager.queue_RL[i].direction, flow_manager.queue_RL[i].speed);
    }


    // Alista el canal inicializando los mutexes
    ready_up_canal();

    // Alista los barcos creando los threads
    ready_up_ships();
    // Iniciar los barcos en la cola de izquierda a derecha (queue_LR)

    // Cierra el canal
    close_canal();

    
    /* --------------------------------- Testing simulations --------------------------------- */

    /* main program cycle */
    while (canal_is_open) {

        /* Send actual state of program*/
        send_state();

        /* Schedule ships */
        //schedule();

        /* Make ships flow */
        // flow();

        /* Check if new ships have been added */
        manage_new_ships();

        // Solo para testing y que termine el programa mientras
        if (ship_count > 9)
            canal_is_open = 0;
    }

    /* --------------------------------------------------------------------------------------- */

    return 0;
}
