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
#include "flow_methods.h"


/* Program variable to start/stop execution */
int is_executing = 0;

/* Scheduler to be used in program */
SchedulerType SCHEDULER = ROUND_ROBIN;

/* Ships count */
int ship_count = 0;


/* ----------------------------- Configuration file variables ----------------------------- */

/* Metodo de control de flujo */
FlowMethod FLOW_METHOD;

/* Length of canal */
int CANAL_LENGTH = 1;

/* Ship speeds */
int NORMAL_SPEED = 1;
int FISHING_SPEED = 2;
int PATROL_SPEED = 3;

/* Maximum ship quantity */
#define MAX_SHIPS 100
/* Array of Ships */
Ship ships[MAX_SHIPS];

/* Time in which the sign changes */
float SIGN_CHANGE_TIME = 0.0;

/* W param for EQUITY flow method */
int W_PARAM = 0;

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
            FLOW_METHOD = get_flow_method(method_str);
        } else if (strncmp(line, "canal_length:", 13) == 0) {
            sscanf(line, "canal_length: %d", &CANAL_LENGTH);
        } else if (strncmp(line, "normal_speed:", 13) == 0) {
            sscanf(line, "normal_speed: %d", &NORMAL_SPEED);
        } else if (strncmp(line, "fishing_speed:", 14) == 0) {
            sscanf(line, "fishing_speed: %d", &FISHING_SPEED);
        } else if (strncmp(line, "patrol_speed:", 13) == 0) {
            sscanf(line, "patrol_speed: %d", &PATROL_SPEED);
        } else if (strncmp(line, "sign_change_time:", 17) == 0) {
            sscanf(line, "sign_change_time: %f", &SIGN_CHANGE_TIME);
        } else if (strncmp(line, "w_param:", 8) == 0) {
            sscanf(line, "w_param: %d", &W_PARAM);
        } 
        // Procesar barcos con el formato ship(NORMAL,0)
        else if (strncmp(line, "ship(", 5) == 0) {
            if (ship_count >= MAX_SHIPS) {
                printf("Error: Demasiados barcos en la configuración.\n");
                break;
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
                speed = NORMAL_SPEED;
            } else if (strcmp(type_str, "FISHING") == 0) {
                type = FISHING;
                speed = FISHING_SPEED;
            } else if (strcmp(type_str, "PATROL") == 0) {
                type = PATROL;
                speed = PATROL_SPEED;
            } else {
                printf("Error: Tipo de barco no válido: %s\n", type_str);
                continue;
            }

            // Validar la dirección
            if (direction != 0 && direction != 1) {
                printf("Error: Dirección no válida (%d) para el barco ID = %d\n", direction, ship_count + 1);
                continue;
            }

            // Almacenar los barcos en el array `ships`
            ships[ship_count].id = ship_count + 1;  // Asignar un ID
            ships[ship_count].type = type;
            ships[ship_count].direction = direction;
            ships[ship_count].speed = speed;         // Asignar la velocidad correspondiente
            ships[ship_count].priority = -1;         // No utilizado
            ships[ship_count].position = -1;         // No utilizado

            printf("Barco creado: ID = %d, Tipo = %s, Dirección = %d, Velocidad = %.1f\n", 
                    ships[ship_count].id, type_str, ships[ship_count].direction, ships[ship_count].speed);

            ship_count++;  // Incrementar el contador de barcos
        }
    }

    fclose(file);
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
    printf("Lista de barcos:\n");
    for (int i = 0; i < ship_count; i++) {
        printf("  Barco %d: Tipo = %d, Dirección = %d\n", 
               ships[i].id, ships[i].type, ships[i].direction);
    }
}


/*
 * Function: schedule
 * ------------------
 * Schedules the available ships using the selected scheduling algorithm.
 * 
 * Notes:
 * - This function simulates the scheduling of ships based on the global variable `SCHEDULER`.
 * - Depending on the scheduling algorithm selected (e.g., ROUND_ROBIN, PRIORITY), the
 *   corresponding scheduler function is called.
 * - If the scheduler type is unrecognized, an error is printed, and the program exits.
 */
void schedule() {

    printf("\nSimulating scheduling ships...\n");

    switch (SCHEDULER) {
        case ROUND_ROBIN:
            round_robin_scheduler(ships, ship_count, 4);  // Ejemplo con time quantum de 4
            break;
        case PRIORITY:
            priority_scheduler(ships, ship_count);
            break;
        case SJF:
            sjf_scheduler(ships, ship_count);
            break;
        case FCFS:
            fcfs_scheduler(ships, ship_count);
            break;
        case REAL_TIME:
            real_time_scheduler(ships, ship_count);
            break;
        default:
            printf("Scheduler no reconocido.\n");
            exit(EXIT_FAILURE);
    }
}


/*
 * Function: flow
 * --------------
 * Simulates the flow control method for managing the ships in the canal.
 * 
 * Notes:
 * - The function chooses the flow control method based on the global variable `FLOW_METHOD`.
 * - It calls the corresponding flow control function (e.g., equity_flow, sign_flow, tico_flow).
 * - If no valid flow method is selected, it prints a message indicating that no flow method
 *   was chosen.
 */
void flow() {
    
    printf("\nSimulating flow control method...\n");

    switch (FLOW_METHOD) {
        case EQUITY:
            equity_flow(W_PARAM);  // Método de equidad
            break;
        case SIGN:
            sign_flow(SIGN_CHANGE_TIME);  // Método de letrero con cambio
            break;
        case TICO:
            tico_flow();  // Método Tico
            break;
        case NONE:
        default:
            printf("No flow method selected\n");
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
    Ship new_ship;

    // Asignar un ID
    new_ship.id = ship_count + 1;

    // Asignar un tipo de barco aleatorio
    int random_type = rand() % 3;  // Valores entre 0 y 2
    new_ship.type = (ShipType)random_type;

    // Asignar una dirección aleatoria (0 o 1)
    new_ship.direction = rand() % 2;

    // Asignar la velocidad correspondiente al tipo de barco
    switch (new_ship.type) {
        case NORMAL:
            new_ship.speed = NORMAL_SPEED;
            break;
        case FISHING:
            new_ship.speed = FISHING_SPEED;
            break;
        case PATROL:
            new_ship.speed = PATROL_SPEED;
            break;
    }

    // Inicializar la posición y prioridad con valores predeterminados
    new_ship.position = 0;
    new_ship.priority = -1;

    // Agrega el nuevo Ship
    ships[ship_count] = new_ship;

    // Incrementa la cantidad de Ships
    ship_count++;
}


/*
 * Function: simulate_ship_movement
 * --------------------------------
 * Simulates the movement of a ship through the canal and updates its position along the way.
 * 
 * Parameters:
 * - ship: pointer to the Ship struct representing the ship to be moved
 * - canal_length: the length of the canal, which determines how far the ship will move
 * 
 * Notes:
 * - The function initializes the ship's position to 0 (start of the canal).
 * - The ship's position is updated iteratively as it moves through the canal, and its details
 *   are printed after each movement.
 * - Once the ship reaches the end of the canal, a message is printed indicating that the ship
 *   has completed its journey.
 */
void simulate_ship_movement(Ship* ship, int canal_length) {
    printf("Simulating ship movement for Ship ID = %d...\n", ship->id);
    
    // Inicializar la posición inicial del barco
    ship->position = 0;  // Comienza en la posición 0 del canal

    // Simular el movimiento del barco a través del canal
    for (int i = 1; i <= canal_length; i++) {
        update_position(ship, i);  // Actualiza la posición del barco a medida que avanza
        print_ship(ship);           // Imprimir los detalles del barco después de moverse
    }

    printf("The ship (ID = %d) has reached the end of the canal.\n\n", ship->id);
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

    // Asigna el argumento de la línea de comandos a SchedulerType
    // Funcion se encuentra en schedulers.h
    SCHEDULER = get_scheduler_type(argv[1]);
    printf("\nParametro obtenido al correr el programa:\n");
    printf("Calendarizador: %d\n\n", SCHEDULER);



    // Lee y carga la configuración desde el archivo
    load_configuration("config/canal_config.txt");

    // Se imprimen las variables leídas para verificar
    printf("\nVariables leidas del archivo de configuracion:\n");
    printf("Método de flujo: %d\n", FLOW_METHOD);
    printf("Largo del canal: %d\n", CANAL_LENGTH);
    printf("Velocidad de barcos NORMAL: %d\n", NORMAL_SPEED);
    printf("Velocidad de barcos FISHING: %d\n", FISHING_SPEED);
    printf("Velocidad de barcos PATROL: %d\n", PATROL_SPEED);
    printf("Tiempo de cambio del letrero: %.2f\n", SIGN_CHANGE_TIME);
    printf("Parámetro W: %d\n", W_PARAM);

    printf("Lista de barcos:\n");
    for (int i = 0; i < ship_count; i++) {
        printf("  Barco %d: Tipo = %d, Dirección = %d\n", 
               ships[i].id, ships[i].type, ships[i].direction);
    }


    printf("\n\nSTARTING EXECUTION\n\n");
    // Starts the execution
    is_executing = 1;

    
    /* --------------------------------- Testing simulations --------------------------------- */

    // Simulando el movimiento de cada barco en el canal usando el array `ships[]`
    printf("\nSimulating ship movements through the canal...\n\n");
    for (int i = 0; i < ship_count; i++) {
        simulate_ship_movement(&ships[i], CANAL_LENGTH);  // Simular el movimiento del barco
    }
    // No es necesario liberar memoria manualmente ya que los barcos están almacenados en un array estático
    
    /* --------------------------------------------------------------------------------------- */

    /* main program cycle */
    while (is_executing) {

        /* Send actual state of program*/
        send_state();

        /* Schedule ships */
        schedule();

        /* Make ships flow */
        flow();

        /* Check if new ships have been added */
        manage_new_ships();

        // Solo para testing y que termine el programa mientras
        if (ship_count > 9)
            is_executing = !is_executing;

    }

    return 0;
}
