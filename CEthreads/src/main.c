/*
 *
 * Instituto Tecnologico de Costa Rica
 * Escuela de Ingenieria en Computadores
 * Principios de Sistemas Operativos
 * Proyecto 1: Scheduling Ships (Calendarizador de Hilos)
 * 
 * File: main.c
 * Archivo principal del programa
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//#include "CEthreads.h"
#include "ship.h"
#include "schedulers.h"
#include "flow_methods.h"

/* ----------------------------- Configuration file variables ----------------------------- */

/* Metodo de control de flujo */
FlowMethod FLOW_METHOD;  // Define el método de flujo

/* Canal parameters   */
int CANAL_LENGTH = 1;  // Length of canal

/* Ship speeds */
int NORMAL_SPEED = 1;
int FISHING_SPEED = 2;
int PATROL_SPEED = 3;

/* Ships */
// Linked list of ships (?)
#define MAX_SHIPS 100
Ship ships[MAX_SHIPS];  // Array para almacenar los barcos

// Tiempo del cambio de letrero
float SIGN_CHANGE_TIME = 0.0;

// Parámetro W para el método de equidad
int W_PARAM = 0;

/* ---------------------------------------------------------------------------------------- */

/* Scheduler to be used in program */
SchedulerType SCHEDULER;

/* Ships created count */
int ship_count = 0;     // Contador de barcos


// Función para leer el archivo de configuración y asignar las variables
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
 * Función para simular el movimiento de un barco en el canal
 * Actualiza la posición del barco y lo imprime
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

    // Asignar el argumento de la línea de comandos a SchedulerType
    // Funcion se encuentra en schedulers.h
    SCHEDULER = get_scheduler_type(argv[1]);

    printf("\nParametro obtenido al correr el programa:\n");
    printf("Calendarizador: %d\n\n", SCHEDULER);



    // leer el archivo de configuracion
    // define:
    //      metodo de control de flujo
    //      largo de canal
    //      velocidad de barcos
    //      cantidad de barcos en cola
    //      tiempo del cambio de letrero
    //      parametro W
    // Cargar la configuración desde el archivo
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




    /*
     * Flow control method simulation
     */
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




    /*
     * Simulando el movimiento de cada barco en el canal usando el array `ships[]`
     */
    printf("\nSimulating ship movements through the canal...\n\n");
    for (int i = 0; i < ship_count; i++) {
        simulate_ship_movement(&ships[i], CANAL_LENGTH);  // Simular el movimiento del barco
    }

    // No es necesario liberar memoria manualmente ya que los barcos están almacenados en un array estático
    return 0;
}




/*
 * Function: function_name
 * ----------------------
 * what the function does
 * 
 * args: arg explanation
 * 
 * returns: what the function returns
 */