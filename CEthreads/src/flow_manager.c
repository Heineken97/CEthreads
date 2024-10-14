/*
 * File: flow_manager.c
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "flow_manager.h"
#include "ship.h"
#include "schedulers.h"

#define BASE_TIME 1000000

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


/*
 * Function: start_timer
 * ---------------------
 * Saves the current time in the provided time_t pointer.
 * 
 * Parameters:
 * - start_time: Pointer to a time_t variable where the current time will be saved.
 */
void start_timer(time_t* start_time) {
    // Guardar la hora actual en la variable a la que apunta start_time
    *start_time = time(NULL);
    printf("Timer started at: %ld seconds since the epoch\n", *start_time);
}



/*
 * Function: has_time_elapsed
 * --------------------------
 * Checks if the specified time interval (as float) has elapsed since the timer started.
 * 
 * Parameters:
 * - start_time: Time when the timer started (time_t).
 * - interval: Time interval in seconds to check (float).
 * 
 * Returns:
 * - 1 (true) if the specified interval has elapsed, 0 (false) otherwise.
 */
int has_time_elapsed(time_t start_time, float interval) {
    time_t current_time = time(NULL);  // Obtener la hora actual

    // Imprimir los tiempos para depuración
    printf("Start time: %ld, Current time: %ld\n", start_time, current_time);

    // Calcular el tiempo transcurrido en segundos
    double elapsed_seconds = difftime(current_time, start_time);

    // Imprimir el tiempo transcurrido
    printf("Elapsed time: %.2f seconds\n", elapsed_seconds);

    // Verificar si ha pasado el tiempo especificado (interval)
    return (elapsed_seconds >= interval);
}



/*
 * Gets how many ships are yet to cross
 */
int get_ships_yet_to_cross(FlowManager* flow_manager) {
    int ships_in_queue = flow_manager->ships_in_queue_LR + flow_manager->ships_in_queue_RL;
    int ships_midcanal = flow_manager->ships_in_midcanal_LR + flow_manager->ships_in_midcanal_RL;
    int ships_yet_to_cross = ships_in_queue + ships_midcanal;
    return ships_yet_to_cross;
}


/*
 * Function: manage_canal
 * ----------------------
 * Determines which ships can proceed based on the current flow method and direction.
 * 
 * Parameters:
 * - flow_manager: pointer to the Flow_Manager struct that holds the current state of the canal
 *   and flow method.
 */
void* manage_canal(void* arg) {
    FlowManager* flow_manager = (FlowManager*)arg;  // Convertir el argumento al tipo FlowManager*

    // If theres Ships yet to cross, the canal must be managed
    while (get_ships_yet_to_cross(flow_manager) > 0) {

        // Revisa si es necesario calendarizar
        if (flow_manager->queues_changed) {

            pthread_mutex_lock(&flow_manager->ship_queues);
            printf("Scheduling in progress...\n");

            // Actual implementation:
            schedule(flow_manager->scheduler,
                     flow_manager->queue_LR, flow_manager->scheduled_queue_LR,
                     flow_manager->queue_RL, flow_manager->scheduled_queue_RL, flow_manager->t_param);
            
            printf("[SCHEDULER] flow_manager->scheduled_queue_LR:\n");
            for (int i = 0; i < flow_manager->ships_in_queue_LR; i++) {
                printf("Scheduled Ship: ID = %d, \n", flow_manager->scheduled_queue_LR[i]);
            }

            printf("[SCHEDULER] flow_manager->scheduled_queue_RL:\n");
            for (int i = 0; i < flow_manager->ships_in_queue_RL; i++) {
                printf("Scheduled Ship: ID = %d, \n", flow_manager->scheduled_queue_RL[i]);
            }

            pthread_mutex_unlock(&flow_manager->ship_queues);

            /* Test example, falta implementar */
            // flow_manager->scheduled_queue_LR[0] = 3;
            // flow_manager->scheduled_queue_LR[1] = 2;
            // flow_manager->scheduled_queue_LR[2] = 1;
            // flow_manager->scheduled_queue_RL[0] = 6;
            // flow_manager->scheduled_queue_RL[1] = 4;
            // flow_manager->scheduled_queue_RL[2] = 5;

            /* Configura el next Ship */

            // Mutex que protege la variable de next_ship
            pthread_mutex_lock(&flow_manager->next_ship_mutex); // Bloquea el mutex
            flow_manager->next_ship = (flow_manager->current_direction == 0) ? flow_manager->scheduled_queue_LR[0] : flow_manager->scheduled_queue_RL[0];
            pthread_mutex_unlock(&flow_manager->next_ship_mutex); // Desloquea el mutex

            // Ya no es necesario calendarizar
            flow_manager->queues_changed = 0;

            printf("Scheduling done...\n");
        }



/* -------------------------------------------- FLOW METHOD: EQUITY -------------------------------------------- */

        if (flow_manager->method == EQUITY) {

            printf("EQUITY flow is managing the canal.\n");

            // Si el ciclo no ha iniciado
            if (flow_manager->ships_for_cycle_ready == 0) {

                
                printf("New cycle starting.\n");

                // Si la direccion es igual a 0 (I->D)
                if (flow_manager->current_direction == 0) {
                    
                    // Si existen barcos con esta direccion en el array del queue 
                    if (flow_manager->ships_in_queue_LR > 0) {

                        // La cantidad de Ships por ingresar será w si hay mas de esa cantidad o de igual cantidad
                        int ships_to_add = (flow_manager->ships_in_queue_LR >= flow_manager->w_param) ? flow_manager->w_param : flow_manager->ships_in_queue_LR;

                        // Ingresa los id de los Ships en el array que podrian estar en el canal que ya estan calendarizados
                        for (int i = 0; i < ships_to_add; i++) {
                            flow_manager->ids_for_canal[i] = flow_manager->scheduled_queue_LR[i];
                        }

                        // Como se termina de definir los Ships de este ciclo
                        flow_manager->ships_for_cycle_ready = 1;

                    }
                    // Como no hay barcos en el array del queue
                    else {

                        // Como no hay barcos se podria cambiar de direccion para probar del otro lado a ver si aparecen
                        flow_manager->current_direction = (flow_manager->current_direction == 0) ? 1 : 0;
                    }

                }
                // Si la direccion es igual a 1 (D->I)
                else if (flow_manager->current_direction == 1) {

                    // Si existen barcos con esta direccion en el array del queue 
                    if (flow_manager->ships_in_queue_RL > 0) {

                        // La cantidad de Ships por ingresar será w si hay mas de esa cantidad o de igual cantidad
                        int ships_to_add = (flow_manager->ships_in_queue_RL >= flow_manager->w_param) ? flow_manager->w_param : flow_manager->ships_in_queue_RL;

                        // Ingresa los id de los Ships en el array que podrian estar en el canal
                        for (int i = 0; i < ships_to_add; i++) {
                            flow_manager->ids_for_canal[i] = flow_manager->scheduled_queue_RL[i];
                        }

                        // Como se termina de definir los Ships de este ciclo
                        flow_manager->ships_for_cycle_ready = 1;

                    } else {

                        // Como no hay barcos se podria cambiar de direccion para probar del otro lado a ver si aparecen
                        flow_manager->current_direction = (flow_manager->current_direction == 0) ? 1 : 0;
                    }
                }
                // Error en la direccion
                else {
                    printf("ERROR @ flow_manager.c (manage_canal): undefined flow_manager->current_direction\n");
                }

            }

            // Si el ciclo inició
            else { 

                // Gets total ships that may complete the journey this cycle
                int ships_to_flow = get_ships_on_cycle(flow_manager);
                
                // Si aun faltan Ships por pasar el canal
                if (flow_manager->ships_passed_this_cycle < ships_to_flow) {

                    printf("\n -------- EQUITY Midcycle -------- \n");



                    pthread_mutex_lock(&flow_manager->ship_queues);
                    
                    // Transmits data to interface
                    //transmit_canal_data(flow_manager, 0);
                    // Transmits data to hardware
                    //transmit_canal_data(flow_manager, 1);
                    // Prints actual state of data sent
                    transmit_canal_data(flow_manager, 2);

                    pthread_mutex_unlock(&flow_manager->ship_queues);




                }
                // Si ya pasaron los W_PARAM Ships de este ciclo
                else {

                    pthread_mutex_lock(&flow_manager->ship_queues);
                    move_ships_to_done(flow_manager);
                    flow_manager->queues_changed = 1;

                    // Cambiar de dirección para el nuevo ciclo
                    flow_manager->current_direction = (flow_manager->current_direction == 0) ? 1 : 0;
                    printf("Cambio de dirección: ahora los barcos en la dirección %d podran avanzar.\n",
                                                                        flow_manager->current_direction);

                    // Se configuran las variables para iniciar un nuevo ciclo
                    flow_manager->ships_for_cycle_ready = 0;
                    flow_manager->ships_passed_this_cycle = 0; // Reiniciar contador de barcos

                    pthread_mutex_unlock(&flow_manager->ship_queues);

                    // Configurar el next_ship despues del cambio de direccion
                    // Siguiente iteracion calendarizará de nuevo

                }
            }

        }
        
/* ------------------------------------------------------------------------------------------------------------- */


/* --------------------------------------------- FLOW METHOD: SIGN --------------------------------------------- */

        else if (flow_manager->method == SIGN) {
            // Método SIGN: cambiar de dirección después de cierto tiempo

            printf("SIGN flow is managing the canal.\n");

            // Si el ciclo no ha iniciado
            if (flow_manager->ships_for_cycle_ready == 0) {

                // Inicia el timer del ciclo
                start_timer(&flow_manager->sign_start_time);

                printf("SIGN FLOW Timer started at: %ld seconds since the epoch\n", flow_manager->sign_start_time);


                // Como se termina de definir los Ships de este ciclo (VER SI APLICA PARA SIGN)
                flow_manager->ships_for_cycle_ready = 1;

            } 
            // Si el ciclo inició
            else {

                // Revisa si aun queda tiempo en el ciclo
                if (!has_time_elapsed(flow_manager->sign_start_time, flow_manager->t_param)) {



                    printf("\n --------- SIGN Midcycle --------- \n");

                    printf("Still time for Ships to sail at the current direction: %d!\n\n", flow_manager->current_direction);

                    
                    
                    pthread_mutex_lock(&flow_manager->ship_queues);
                    
                    // Transmits data to interface
                    //transmit_canal_data(flow_manager, 0);
                    // Transmits data to hardware
                    //transmit_canal_data(flow_manager, 1);
                    // Prints actual state of data sent
                    transmit_canal_data(flow_manager, 2);

                    pthread_mutex_unlock(&flow_manager->ship_queues);


                    
                }
                // el tiempo acabo
                else {

                    printf("Time elapsed, changing direction...\n");

                    pthread_mutex_lock(&flow_manager->ship_queues);
                    move_ships_to_done(flow_manager);
                    flow_manager->queues_changed = 1;

                    // Cambiar de dirección para el nuevo ciclo
                    flow_manager->current_direction = (flow_manager->current_direction == 0) ? 1 : 0;
                    printf("Cambio de dirección: ahora los barcos en la dirección %d podran avanzar.\n",
                                                                        flow_manager->current_direction);

                    // Se configuran las variables para iniciar un nuevo ciclo
                    flow_manager->ships_for_cycle_ready = 0;
                    flow_manager->ships_passed_this_cycle = 0; // Reiniciar contador de barcos

                    pthread_mutex_unlock(&flow_manager->ship_queues);

                    // Configurar el next_ship despues del cambio de direccion
                    // Siguiente iteracion calendarizará de nuevo

                }

            }

        }
        
/* ------------------------------------------------------------------------------------------------------------- */


/* --------------------------------------------- FLOW METHOD: TICO --------------------------------------------- */
        
        else if (flow_manager->method == TICO) {
            // Método TICO: alternar un barco de cada lado

            flow_manager->current_direction = (flow_manager->current_direction == 0) ? 1 : 0;
            printf("Alternancia TICO: ahora los barcos en la dirección %d pueden avanzar.\n", flow_manager->current_direction);

            // Check for ships in canals and the actual direction
            // LOGICA PARA MANEJAR SIGN
            printf("TICO flow is managing the canal\n");

        }

/* ------------------------------------------------------------------------------------------------------------- */
        

        // Error en el metodo de flujo
        else {
            printf("ERROR @ flow_manager.c (manage_canal): undefined flow_manager->method\n");
        }
        

        // Se actualiza con la velocidad del Ship mas rapido
        usleep(BASE_TIME / flow_manager->patrol_ship_speed);

    }

    return NULL;

}


/*
 * Function: move_ships_to_done
 * ----------------------------
 * Moves ships from the corresponding queue (queue_LR or queue_RL) to the 
 * appropriate done array (done_LR or done_RL) based on the current direction 
 * and the IDs present in ids_for_canal.
 * 
 * This function ensures that:
 * 1. Ships identified in ids_for_canal are transferred from the queue to the 
 *    corresponding done array.
 * 2. Remaining ships in the queue are shifted to maintain the order, leaving no gaps.
 * 3. Resets the count of ships in midcanal after processing.
 * 4. Resets the ids_for_canal array after processing to avoid stale data.

 * Parameters:
 * - flow_manager: pointer to the FlowManager structure containing the arrays, 
 *   counters, and current flow direction.
 * 
 * Notes:
 * - This function handles both directions of the canal:
 *   - If the direction is 0 (Left to Right), it processes queue_LR and done_LR.
 *   - If the direction is 1 (Right to Left), it processes queue_RL and done_RL.
 * - If the direction is not valid, an error message is printed.
 * - After processing, the ids_for_canal array is cleared to avoid inconsistencies.
 */
void move_ships_to_done(FlowManager* flow_manager) {
    // Function is called when flow_manager decides its time to change cycle

    int new_queue_size = 0;  // Index for the adjusted queue_LR array

    /* -------------------------------------------- FLOW METHOD: EQUITY -------------------------------------------- */

    if (flow_manager->method == EQUITY) {

        // Si la direccion es igual a 0 (I->D) y hay Ships midcanal
        if (flow_manager->current_direction == 0 && flow_manager->ships_in_midcanal_LR > 0) {

            // Iterate over the current midcanal_LR array
            for (int i = 0; i < flow_manager->ships_in_midcanal_LR; i++) {
                int ship_id = flow_manager->midcanal_LR[i].id;

                // Si el id del ship es posible
                if (0 < ship_id && ship_id <= MAX_SHIPS) {

                    int found = 0;

                    // Check if the current ship's ID is in ids_for_canal
                    for (int j = 0; j < flow_manager->ships_in_midcanal_LR; j++) {
                        if (flow_manager->ids_for_canal[j] == ship_id) {
                            // Change Ships done status
                            flow_manager->midcanal_LR[i].is_done = 1;
                            // Move the ship to the done_LR array
                            int done_index = flow_manager->ships_in_done_LR;
                            flow_manager->done_LR[done_index] = flow_manager->midcanal_LR[i];
                            flow_manager->ships_in_done_LR++;  // Increment the done_LR counter
                            found = 1;  // Mark the ship as found and moved
                            break;
                        }
                    }

                    // If the ship was not found in ids_for_canal, keep it in the adjusted queue
                    if (!found) {
                        new_queue_size++;
                    }

                } else {
                    // Ship no deberia importar
                    continue;
                }
            }

            // Resets the number of ships in the midcanal_LR
            // Porque en EQUITY no quedan Ships en medio canal
            flow_manager->ships_in_midcanal_LR = 0;

        }
        // Si la direccion es igual a 1 (D->I) y hay Ships midcanal
        else if (flow_manager->current_direction == 1 && flow_manager->ships_in_midcanal_RL > 0) {

            // Iterate over the current midcanal_RL array
            for (int i = 0; i < flow_manager->ships_in_midcanal_RL; i++) {
                int ship_id = flow_manager->midcanal_RL[i].id;

                // Si el id del ship es posible
                if (0 < ship_id && ship_id <= MAX_SHIPS) {

                    int found = 0;

                    // Check if the current ship's ID is in ids_for_canal
                    for (int j = 0; j < flow_manager->ships_in_midcanal_RL; j++) {
                        if (flow_manager->ids_for_canal[j] == ship_id) {
                            // Change Ships done status
                            flow_manager->midcanal_RL[i].is_done = 1;
                            // Move the ship to the done_RL array
                            int done_index = flow_manager->ships_in_done_RL;
                            flow_manager->done_RL[done_index] = flow_manager->midcanal_RL[i];
                            flow_manager->ships_in_done_RL++;  // Increment the done_RL counter
                            found = 1;  // Mark the ship as found and moved
                            break;
                        }
                    }

                    // If the ship was not found in ids_for_canal, keep it in the adjusted queue
                    if (!found) {
                        new_queue_size++;
                    }

                } else {
                    // Ship no deberia importar
                    continue;
                }
            }

            // Resets the number of ships in the midcanal_RL
            // Porque en EQUITY no quedan Ships en medio canal
            flow_manager->ships_in_midcanal_RL = 0;
        }
        // Error en la direccion
        else {
            printf("ERROR @ flow_manager.c (move_ships_to_done): undefined flow_manager->current_direction\n");
            printf("OR NO SHIPS MIDCANAL\n");
        }

        // Quitar los ids del array y pone todos en 0
        for (int i = 0; i < MAX_SHIPS; i++) {
            flow_manager->ids_for_canal[i] = 0;  // Ids en 0
        }

    }

/* ------------------------------------------------------------------------------------------------------------- */


/* --------------------------------------------- FLOW METHOD: SIGN --------------------------------------------- */

        else if (flow_manager->method == SIGN) {

            // Si la direccion es igual a 0 (I->D) y hay Ships midcanal
            if (flow_manager->current_direction == 0 && flow_manager->ships_in_midcanal_LR > 0) {


                // Iterate over the current queue_LR array
                for (int i = 0; i < flow_manager->ships_in_queue_LR; i++) {
                    int ship_id = flow_manager->queue_LR[i].id;

                    // Si el id del ship es aceptado
                    if (0 < ship_id && ship_id <= MAX_SHIPS) {

                        int found = 0;

                        // Check if the current ship's ID is in ships_through
                        for (int j = 0; j < flow_manager->ships_passed_this_cycle; j++) {
                            if (flow_manager->ships_through[j] == ship_id) {

                                // Change Ships done status
                                flow_manager->queue_LR[i].is_done = 1;
                                // Move the ship to the done_LR array
                                int done_index = flow_manager->ships_in_done_LR;
                                flow_manager->done_LR[done_index] = flow_manager->queue_LR[i];
                                flow_manager->ships_in_done_LR++;  // Increment the done_LR counter

                                // Resets the number of ships in the midcanal_LR
                                flow_manager->ships_in_midcanal_LR--;

                                found = 1;  // Mark the ship as found and moved
                                break;
                            }
                        }

                        // If the ship was not found in ships_through, keep it in the adjusted queue
                        if (!found) {
                            new_queue_size++;
                        }

                    } else {
                        // Ship no deberia importar
                        continue;
                    }
                }

                // Update the number of ships in the queue_LR array
                flow_manager->ships_in_queue_LR = new_queue_size;

            }
            // Si la direccion es igual a 1 (D->I) y hay Ships midcanal
            else if (flow_manager->current_direction == 1 && flow_manager->ships_in_midcanal_RL > 0) {

            











            }
            // Error en la direccion
            else {
                printf("ERROR @ flow_manager.c (move_ships_to_done): undefined flow_manager->current_direction\n");
                printf("OR NO SHIPS MIDCANAL\n");
            }

        }

        /* ------------------------------------------------------------------------------------------------------------- */


/* --------------------------------------------- FLOW METHOD: TICO --------------------------------------------- */
        
        else if (flow_manager->method == TICO) {
            // Método TICO: alternar un barco de cada lado










        }

/* ------------------------------------------------------------------------------------------------------------- */
        

        // Error en el metodo de flujo
        else {
            printf("ERROR @ flow_manager.c (move_ships_to_done): undefined flow_manager->method\n");
        }

}


/*
 * Define la cantidad de Ships que se espera que pasen en este ciclo.
 * Verifica si son los W_PARAM o si hay menos en el queue.
 */
int get_ships_on_cycle(FlowManager* flow_manager) {

    int total_ships_on_cycle;

    // La cantidad de Ships por pasar por el canal será w si hay mas de esa cantidad o de igual cantidad
    if (flow_manager->current_direction == 0){

        total_ships_on_cycle = (flow_manager->ships_in_queue_LR >= flow_manager->w_param) ? flow_manager->w_param : flow_manager->ships_in_queue_LR;

    } else if (flow_manager->current_direction == 1){

        total_ships_on_cycle = (flow_manager->ships_in_queue_RL >= flow_manager->w_param) ? flow_manager->w_param : flow_manager->ships_in_queue_RL;

    } else {

        total_ships_on_cycle = flow_manager->w_param;

    }

    return total_ships_on_cycle;

}


/*
 * Function: transmit_canal_data
 * -----------------------------
 * Transmits the canal's state and relevant ship information to a specific destination.
 * 
 * Parameters:
 * - flow_manager: Pointer to the FlowManager structure containing the canal's state and ships.
 * - destination: The target destination for the data transmission.
 *                (0 = Interface, 1 = Hardware, 2 = Console).
 * 
 * Description:
 * - When sending to the interface (destination 0), the function fills an `InterfaceData` struct 
 *   with ship and canal information, including ships in queue, mid-canal, and done lists, as well as 
 *   flow and scheduling parameters. The data is serialized and transmitted through the interface’s 
 *   serial port.
 * - When sending to the hardware (destination 1), it prepares a simpler `HardwareData` struct with
 *   only the ship IDs and their positions, which is then transmitted via the hardware's serial port.
 * - If the destination is console (destination 2), the function prints relevant canal and ship 
 *   information to the console for debugging or monitoring purposes.
 * - If the destination is invalid, an error message is printed, and the function exits.
 * 
 * Notes:
 * - The function uses `write()` to send serialized data through the respective serial ports.
 * - Ensure that the serial ports are correctly configured and open before calling this function.
 * - This function is crucial for synchronizing the canal's state with external systems in real-time.
 */

void transmit_canal_data(FlowManager* flow_manager, int destination) {

    // Sending to interface
    if (destination == 0) {

        InterfaceData i_serial_data;

        int nShips = 0;

        /*
         * Se agregan las variables al struct InterfaceData
         */


        // Ships in queue LR scheduled
        // Iterate over the IDs in the scheduled_queue_LR
        for (int i = 0; i < MAX_SHIPS; i++) {
            int current_id = flow_manager->scheduled_queue_LR[i]; // Get the current ID

            // Skip if the ID is less or 0 (empty slot)
            if (current_id <= 0 && current_id > MAX_SHIPS) {
                continue;
            }

            // Search for the current ID in the queue_LR array
            for (int j = 0; j < MAX_SHIPS; j++) {
                if (flow_manager->queue_LR[j].id == current_id) {
                    // If a match is found, save the ship's details in struct
                    i_serial_data.queue_LR_data[nShips].type = flow_manager->queue_LR[j].type;
                    i_serial_data.queue_LR_data[nShips].position = flow_manager->queue_LR[j].position;
                    // Keeps count of Ships in this array
                    nShips++;
                    break;  // Exit the inner loop as the ship has been found
                }
            }
        }

        nShips = 0;
        // Ships in queue RL scheduled
        for (int i = 0; i < MAX_SHIPS; i++) {
            int current_id = flow_manager->scheduled_queue_RL[i]; // Get the current ID

            // Skip if the ID is less or 0 (empty slot)
            if (current_id <= 0 && current_id > MAX_SHIPS) {
                continue;
            }

            // Search for the current ID in the queue_RL array
            for (int j = 0; j < MAX_SHIPS; j++) {
                if (flow_manager->queue_RL[j].id == current_id) {
                    // If a match is found, save the ship's details in struct
                    i_serial_data.queue_RL_data[nShips].type = flow_manager->queue_RL[j].type;
                    i_serial_data.queue_RL_data[nShips].position = flow_manager->queue_RL[j].position;
                    // Keeps count of Ships in this array
                    nShips++;
                    break;  // Exit the inner loop as the ship has been found
                }
            }

        }

        nShips = 0;
        // Ships in midcanal (current direction only)


        // Completed ships in LR


        // Completed ships in RL


        // Length of the canal


        // Current direction


        // Flow control method (int)


        // Scheduling algorithm (int)





        // Sending the InterfaceData struct to update the Ship positions
        printf("\nSending InterfaceData struct...\n");
        //write(flow_manager->interface_serial_port, &i_serial_data, sizeof(InterfaceData));

    }
    // Sending to hardware
    else if (destination == 1) {

        HardwareData h_serial_data;

        /*
         * Se agregan las variables al struct HardwareData
         */




        // Sending the HardwareData struct to update the Ship positions
        printf("\nSending HardwareData struct...\n");
        //write(flow_manager->hardware_serial_port, &h_serial_data, sizeof(HardwareData));

    }
    // Sending to console
    else if (destination == 2) {

        printf("\nPrinting Ships to console...\n");

        // Ships in queue LR scheduled
        // Iterate over the IDs in the scheduled_queue_LR
        printf("\n<-------------------Ships in queue LR scheduled------------------->\n");
        for (int i = 0; i < MAX_SHIPS; i++) {
            int current_id = flow_manager->scheduled_queue_LR[i]; // Get the current ID

            // Skip if the ID is less tan 1 or higher tan MAX (empty slot)
            if (0 < current_id && current_id <= MAX_SHIPS) {
                
                // Search for the current ID in the queue_LR array
                for (int j = 0; j < MAX_SHIPS; j++) {
                    if (flow_manager->queue_LR[j].id == current_id && flow_manager->queue_LR[j].position == 0) {
                        // If a match is found, print it
                        printf("    -> Ship: id = %d, type = %d, direction = %d, position = %d\n", 
                            flow_manager->queue_LR[j].id, flow_manager->queue_LR[j].type,
                            flow_manager->queue_LR[j].direction, flow_manager->queue_LR[j].position);
                        break;  // Exit the inner loop as the ship has been found
                    }
                }
            }
        }
        printf("\n<----------------------------------------------------------------->\n");

        // Ships in queue RL scheduled
        printf("\n<-------------------Ships in queue RL scheduled------------------->\n");
        for (int i = 0; i < MAX_SHIPS; i++) {
            int current_id = flow_manager->scheduled_queue_RL[i]; // Get the current ID

            // Skip if the ID is less tan 1 or higher tan MAX (empty slot)
            if (0 < current_id && current_id <= MAX_SHIPS) {

                // Search for the current ID in the queue_RL array
                for (int j = 0; j < MAX_SHIPS; j++) {
                    if (flow_manager->queue_RL[j].id == current_id && flow_manager->queue_RL[j].position == 0) {
                        // If a match is found, save the ship's details in struct
                        printf("    -> Ship: id = %d, type = %d, direction = %d, position = %d\n", 
                            flow_manager->queue_RL[j].id, flow_manager->queue_RL[j].type,
                            flow_manager->queue_RL[j].direction, flow_manager->queue_RL[j].position);
                        break;  // Exit the inner loop as the ship has been found
                    }
                }
            }
        }
        printf("\n<----------------------------------------------------------------->\n");

        // Ships in midcanal LR
        printf("\n<------------------Ships in midcanal LR------------------->\n");
        for (int i = 0; i < MAX_SHIPS; i++) {
            int current_id = flow_manager->ships_still_going[i]; // Get the current ID
            
            // Skip if the ID is less tan 1 or higher tan MAX (empty slot)
            if (0 < current_id && current_id <= MAX_SHIPS) {
                
                if (flow_manager->current_direction == 0) {

                    //printf(" *** Ships in midcanal LR (ships_still_going): current_id = %d\n",current_id);

                    // Search for the current ID in the queue_LR array
                    for (int j = 0; j < MAX_SHIPS; j++) {
                        if (flow_manager->midcanal_LR[j].id == current_id) {
                            // If a match is found, save the ship's details in struct
                            printf("    -> Ship: id = %d, type = %d, direction = %d, position = %d\n", 
                                flow_manager->midcanal_LR[j].id, flow_manager->midcanal_LR[j].type,
                                flow_manager->midcanal_LR[j].direction, flow_manager->midcanal_LR[j].position);
                            break;  // Exit the inner loop as the ship has been found
                        }
                    }

                }

            }

        }
        printf("\n<----------------------------------------------------------------->\n");

        // Ships in midcanal RL
        printf("\n<------------------Ships in midcanal RL------------------->\n");
        for (int i = 0; i < MAX_SHIPS; i++) {
            int current_id = flow_manager->ships_still_going[i]; // Get the current ID
            
            // Skip if the ID is less tan 1 or higher tan MAX (empty slot)
            if (0 < current_id && current_id <= MAX_SHIPS) {
                
                if (flow_manager->current_direction == 1) {

                    //printf(" *** Ships in midcanal RL (ships_still_going): current_id = %d\n",current_id);

                    // Search for the current ID in the queue_RL array
                    for (int j = 0; j < MAX_SHIPS; j++) {
                        if (flow_manager->midcanal_RL[j].id == current_id) {
                            // If a match is found, save the ship's details in struct
                            printf("    -> Ship: id = %d, type = %d, direction = %d, position = %d\n", 
                                flow_manager->midcanal_RL[j].id, flow_manager->midcanal_RL[j].type,
                                flow_manager->midcanal_RL[j].direction, flow_manager->midcanal_RL[j].position);
                            break;  // Exit the inner loop as the ship has been found
                        }
                    }

                }

            }

        }
        printf("\n<----------------------------------------------------------------->\n");
















    } 
    // Destination error
    else {
        printf("TRANSMITTER ERROR @ flow_manager.c (transmit_canal_data): undefined destination\n");
    }

}


/*
 * Function: can_ship_advance
 * --------------------------
 * Called in move_ship @ ship.c
 * Determines if the ship is allowed to advance based on the flow_manager's current state.
 * 
 * Parameters:
 * - ship: pointer to the Ship struct representing the ship trying to advance.
 * - flow_manager: pointer to the Flow_Manager struct controlling the canal.
 * 
 * Returns:
 * - 1 if the ship is allowed to advance, 0 otherwise.
 */
int ship_can_advance(Ship** ship, FlowManager* flow_manager) {

    // Checks if ship is next so it can flow o if its midcanal already
    if (ship_is_next(ship, flow_manager) || (*ship)->position > 0) {

/* -------------------------------------------- FLOW METHOD: EQUITY -------------------------------------------- */

        if (flow_manager->method == EQUITY) {

            // Verificar si el barco está en la dirección correcta
            if ((*ship)->direction != flow_manager->current_direction) {
                // El barco no puede avanzar porque la dirección no está activa
                printf("El Barco %d no puede avanzar porque su dirección no está activa.\n", (*ship)->id);
                return 0;
            }

            // Gets total ships that may complete the journey this cycle
            int ships_to_flow = get_ships_on_cycle(flow_manager);

            // Si ya se completaron los W_PARAM Ships de este ciclo, no puede pasar
            if (flow_manager->ships_passed_this_cycle >= ships_to_flow) {
                printf("No más barcos pueden avanzar en esta dirección hasta que se cambie.\n");
                return 0;  // No más barcos pueden avanzar en esta dirección hasta que se cambie
            }
            else {
                int will_be_midcanal = 0;
                // Busca el id del Ship en la lista de los barcos que podrian estar midcanal
                for (int i = 0; i < flow_manager->w_param; i++) {
                    // Revisa si el Ship esta en la lista de barcos que puede pasar por el canal
                    if (flow_manager->ids_for_canal[i] == (*ship)->id) {
                        printf("Barco %d esta en la lista de barcos que puede pasar por el canal!\n", (*ship)->id);
                        will_be_midcanal = 1; 
                    }
                }

                // Si el Ship debiera estar en medio canal 
                if (will_be_midcanal) {

                    // Determines if ship is mid-canal or needs to be moved to it 
                    return ship_is_midcanal(ship, flow_manager);

                } else {
                    printf("NOT_ADVANCING: Barco %d no está entre los que pasarán este ciclo...\n", (*ship)->id);
                    return 0;
                }
   
            }

        }

/* ------------------------------------------------------------------------------------------------------------- */


/* --------------------------------------------- FLOW METHOD: SIGN --------------------------------------------- */
        
        else if (flow_manager->method == SIGN) {

            // Verificar si el barco está en la dirección correcta
            if ((*ship)->direction != flow_manager->current_direction) {
                // El barco no puede avanzar porque la dirección no está activa
                printf("El Barco %d no puede avanzar porque su dirección no está activa.\n", (*ship)->id);
                return 0;
            }
            // si lo esta
            else {

                // Determines if ship is mid-canal or needs to be moved to it 
                return ship_is_midcanal(ship, flow_manager);

                // Mientras el flow_manager esta verificando el tiempo que queda

            }

        }

/* ------------------------------------------------------------------------------------------------------------- */


/* --------------------------------------------- FLOW METHOD: TICO --------------------------------------------- */
        
        else if (flow_manager->method == TICO) {

            printf("El Barco %d puede avanzar mientras se configura el metodo TICO.\n", (*ship)->id);
            return 1;

        }

/* ------------------------------------------------------------------------------------------------------------- */


        // Error en el metodo de flujo
        else {
            printf("ERROR @ flow_manager.c (ship_can_advance): undefined flow_manager->method\n");
            return 0;
        }

    } else {
        printf("El Barco %d no está calendarizado para avanzar.\n", (*ship)->id);
        return 0;
    }

}


/*
 * Permitira saber si el ship que está esperando es el siguiente en la lista o no
 */
int ship_is_next(Ship** ship, FlowManager* flow_manager){

    // Si la direccion es igual a 0 (I->D) && si no ha empezado el flujo o está en la primera posicion
    if (flow_manager->current_direction == 0 && (*ship)->position <= 1) {

        // If actual next_ship is the current ship
        if (flow_manager->next_ship == (*ship)->id) {

            /* Setting next Ship after this one */

            // Si el Ship ya salio de la espera y está mid canal en position 1
            // Y no es el ultimo del ciclo (faltan para llegar a w_param)
            // Se cambiara al siguiente next de esta direccion
            if ((*ship)->position > 0 && flow_manager->ships_in_midcanal_LR <= flow_manager->w_param) {

                // Mutex que protege la variable de next_ship
                pthread_mutex_lock(&flow_manager->next_ship_mutex); // Bloquea el mutex

                // Elimina de la lista calendarizada al barco que ya esta midcanal en position 1
                for (int i = 0; i < flow_manager->ships_in_queue_LR - 1; i++) {
                    // Shift elements in scheduled_queue_LR to the left
                    flow_manager->scheduled_queue_LR[i] = flow_manager->scheduled_queue_LR[i + 1];
                }

                // Configura el siguiente en lista al proximo next_ship
                flow_manager->next_ship = flow_manager->scheduled_queue_LR[0];
                pthread_mutex_unlock(&flow_manager->next_ship_mutex); // Desbloquea el mutex
            }

            printf("El Barco %d is next.\n", (*ship)->id);
            return 1;  // 1 to indicate that the ship is next

        } else {
            printf("El Barco %d is not next.\n", (*ship)->id);
            return 0;  // 0 if no match is found
        }
    } 
    // Si la direccion es igual a 1 (D->I) && si no ha empezado el flujo o está en la primera posicion
    else if (flow_manager->current_direction == 1 && (*ship)->position <= 1) {

        // If actual next_ship is the current ship
        if (flow_manager->next_ship == (*ship)->id) {

            // Si el Ship ya salio de la espra y está mid canal en position 1
            // Y no es el ultimo del ciclo (faltan para llegar a w_param)
            // Se cambiara al siguiente next de esta direccion
            if ((*ship)->position > 0 && flow_manager->ships_in_midcanal_RL < flow_manager->w_param) {

                // Mutex que protege la variable de next_ship
                pthread_mutex_lock(&flow_manager->next_ship_mutex); // Bloquea el mutex

                // Elimina de la lista calendarizada al barco que ya esta midcanal en position 1
                for (int i = 0; i < flow_manager->ships_in_queue_RL - 1; i++) {
                    // Shift elements in scheduled_queue_RL to the left
                    flow_manager->scheduled_queue_RL[i] = flow_manager->scheduled_queue_RL[i + 1];
                }

                // Configura el siguiente en lista al proximo next_ship
                flow_manager->next_ship = flow_manager->scheduled_queue_RL[0];
                pthread_mutex_unlock(&flow_manager->next_ship_mutex); // Bloquea el mutex
            }

            printf("El Barco %d is next.\n", (*ship)->id);
            return 1;  // 1 to indicate that the ship is next

        } else {
            printf("El Barco %d is not next.\n", (*ship)->id);
            return 0;  // 0 if no match is found
        }
    } else {
        // Ship is already midcanal
        return 0;
    }

}


/*
 * Function: ship_is_midcanal
 * --------------------------
 * Determines whether a ship is already in the mid-canal or needs to be moved 
 * from the queue to the mid-canal based on its current position and the 
 * canal's direction.
 * 
 * Parameters:
 * - ship: Pointer to the Ship struct representing the ship being moved.
 * - flow_manager: Pointer to the FlowManager struct managing the canal and ship queues.
 * 
 * Returns:
 * - 1 (true) if the ship is either already in the mid-canal or successfully moved there.
 * - 0 (false) if the ship is not found in the queue or an error occurs.
 * 
 * Notes:
 * - If the ship's position is greater than 0, it is considered to be in the mid-canal and can proceed.
 * - If the ship's position is 0, it must be moved from the queue to the mid-canal.
 * - The function locks the `ship_queues` mutex to ensure thread-safe modifications 
 *   of the ship queues.
 * - If the ship is not found in its respective queue, an error message is printed, 
 *   and the function returns 0.
 * - Once the ship is moved from the queue to the mid-canal, the queue counters are adjusted accordingly.
 * - If an undefined direction is detected, an error message is printed.
 */

int ship_is_midcanal(Ship** ship, FlowManager* flow_manager) {

    // Si el ship si esta en medio canal
    if ((*ship)->position > 0) {

        printf("Barco %d se encuentra mid canal.\n", (*ship)->id);
        // Puede continuar avanzando
        return 1;
    }
    // El Ship está por comenzar a pasar el canal
    // Solo entraria una vez
    else {

        // Para buscar el index del Ship en queue array
        int ship_index = -1;                        

        // Si la direccion es igual a 0 (I->D)
        if (flow_manager->current_direction == 0) {

            pthread_mutex_lock(&flow_manager->ship_queues);  // Bloquea el mutex

            // Finds the index of the ship in scheduled_queue_LR array
            // Loops through scheduled_queue_LR array to locate the ship
            // that matches the id of the ship being moved.
            for (int i = 0; i < flow_manager->ships_in_queue_LR; i++) {
                if (flow_manager->scheduled_queue_LR[i] == (*ship)->id) {
                    ship_index = i;
                    printf("Ship with ID %d was found in scheduled_queue_LR @ i = %d.\n", (*ship)->id, i);
                    break;
                }
            }

            // If the ship is not found in queue_LR, print an error and exit the function
            if (ship_index == -1) {
                printf("ERROR: Ship with ID %d not found in scheduled_queue_LR.\n", (*ship)->id);
                return 0;
            }


            // Search for the ship in the queue_LR array
            for (int i = 0; i < flow_manager->ships_in_queue_LR; i++) {
                if (flow_manager->queue_LR[i].id == (*ship)->id) {
                    ship_index = i;
                    break;
                }
            }

            // If the ship wasn't found, return an error
            if (ship_index == -1) {
                printf("Error: Ship %d not found in queue_LR.\n", (*ship)->id);
                return 0;
            }

            // Adds the ship to the end of the midcanal_LR array
            flow_manager->midcanal_LR[flow_manager->ships_in_midcanal_LR] = **ship;

            *ship = &flow_manager->midcanal_LR[flow_manager->ships_in_midcanal_LR];
            
            // Increment the counter for ships in midcanal_LR
            flow_manager->ships_in_midcanal_LR++;

            // Obtiene la cantidad exacta de ships que estan en medio canal
            int ships_midcanal = flow_manager->ships_in_midcanal_LR + flow_manager->ships_in_midcanal_RL; 

            // Adds the ship to array of the ships still going
            flow_manager->ships_still_going[ships_midcanal] = (*ship)->id;

            // Removes the Ship from the queue_LR array by shifting remaining ships

            // Shift remaining ships in queue_LR to fill the gap
            for (int i = ship_index; i < flow_manager->ships_in_queue_LR - 1; i++) {
                flow_manager->queue_LR[i] = flow_manager->queue_LR[i + 1];
            }

            // Set the last elements id to -1 and decrement the queue counter
            flow_manager->queue_LR[flow_manager->ships_in_queue_LR - 1].id = -1;
            // Decreases the counter for ships in queue_LR as the ship has been removed
            flow_manager->ships_in_queue_LR--;

            // Ship was successfully moved
            printf("Barco %d pasaria de queue_LR a midcanal_LR.\n", (*ship)->id);

            //flow_manager->queues_changed = 1;
            pthread_mutex_unlock(&flow_manager->ship_queues);   // Desbloquea el mutex

            // Ship puede avanzar
            return 1;

        }
        // Si la direccion es igual a 1 (D->I)
        else if (flow_manager->current_direction == 1) {

            

            
            pthread_mutex_lock(&flow_manager->ship_queues);  // Bloquea el mutex

            // Finds the index of the ship in scheduled_queue_RL array
            // Loops through scheduled_queue_RL array to locate the ship
            // that matches the id of the ship being moved.
            for (int i = 0; i < flow_manager->ships_in_queue_RL; i++) {
                if (flow_manager->scheduled_queue_RL[i] == (*ship)->id) {
                    ship_index = i;
                    printf("Ship with ID %d was found in scheduled_queue_RL @ i = %d.\n", (*ship)->id, i);
                    break;
                }
            }

            // If the ship is not found in queue_RL, print an error and exit the function
            if (ship_index == -1) {
                printf("ERROR: Ship with ID %d not found in scheduled_queue_RL.\n", (*ship)->id);
                return 0;
            }


            // Search for the ship in the queue_RL array
            for (int i = 0; i < flow_manager->ships_in_queue_RL; i++) {
                if (flow_manager->queue_RL[i].id == (*ship)->id) {
                    ship_index = i;
                    break;
                }
            }

            // If the ship wasn't found, return an error
            if (ship_index == -1) {
                printf("Error: Ship %d not found in queue_RL.\n", (*ship)->id);
                return 0;
            }

            // Adds the ship to the end of the midcanal_RL array
            flow_manager->midcanal_RL[flow_manager->ships_in_midcanal_RL] = **ship;

            *ship = &flow_manager->midcanal_RL[flow_manager->ships_in_midcanal_RL];
            
            // Increment the counter for ships in midcanal_RL
            flow_manager->ships_in_midcanal_RL++;

            // Obtiene la cantidad exacta de ships que estan en medio canal
            int ships_midcanal = flow_manager->ships_in_midcanal_LR + flow_manager->ships_in_midcanal_RL; 

            // Adds the ship to array of the ships still going
            flow_manager->ships_still_going[ships_midcanal] = (*ship)->id;

            // Removes the Ship from the queue_RL array by shifting remaining ships

            // Shift remaining ships in queue_RL to fill the gap
            for (int i = ship_index; i < flow_manager->ships_in_queue_RL - 1; i++) {
                flow_manager->queue_RL[i] = flow_manager->queue_RL[i + 1];
            }

            // Set the last elements id to -1 and decrement the queue counter
            flow_manager->queue_RL[flow_manager->ships_in_queue_RL - 1].id = -1;
            // Decreases the counter for ships in queue_RL as the ship has been removed
            flow_manager->ships_in_queue_RL--;

            // Ship was successfully moved
            printf("Barco %d pasaria de queue_RL a midcanal_RL.\n", (*ship)->id);

            //flow_manager->queues_changed = 1;
            pthread_mutex_unlock(&flow_manager->ship_queues);   // Desbloquea el mutex

            // Ship puede avanzar
            return 1;
        }
        // Error en la direccion
        else {

            printf("ERROR @ flow_manager.c (ship_is_midcanal): undefined flow_manager->current_direction\n");
            return 0;
        }

    }

}


/*
 * Function: end_sail
 * ------------------
 * Called in move_ship @ ship.c
 * Handles the logic for completing the journey of a ship through the canal. 
 * The ship's ID is removed from the array tracking ships still in the canal, 
 * and counters are updated accordingly.
 * 
 * Parameters:
 * - ship: Pointer to the Ship struct representing the ship that has completed its journey.
 * - flow_manager: Pointer to the FlowManager struct managing the canal's state and ship arrays.
 * 
 * Notes:
 * - The function searches for the ship's ID in the `ships_still_going` array to ensure it is tracked properly.
 * - If the ship's ID is not found, an error message is printed, and the function returns.
 * - Once the ship completes its journey, the ship's ID is removed from `ships_still_going`.
 * - Counters such as `ships_passed_this_cycle` and `total_ships_passed` are incremented.
 * - The ship's ID is also stored in the `ships_through` array to track all ships that have passed through the canal.
 * - This function assumes thread-safety is handled externally (e.g., by locking necessary mutexes).
 */

void end_sail(Ship** ship, FlowManager* flow_manager) {

    // Para buscar el index del Ship en ships_still_going array
    int ship_index = -1;

    // Obtiene la cantidad exacta de ships que estan en medio canal
    int ships_midcanal = flow_manager->ships_in_midcanal_LR + flow_manager->ships_in_midcanal_RL; 

    // Finds the index of the ship in ships_still_going array
    // Loops through ships_still_going array to locate the ship
    // that matches the id of the ship being moved.
    for (int i = 0; i < ships_midcanal; i++) {
        if (flow_manager->ships_still_going[i] == (*ship)->id) {
            ship_index = i;
            printf("Ship with ID %d was found in ships_still_going @ i = %d, to be deleted.\n", (*ship)->id, i);
            break;
        }
    }

    // If the ship is not found in ships_still_going, print an error and exit the function
    if (ship_index == -1) {
        printf("ERROR: Ship with ID = %d finished sailing and it is not found in ships_still_going to delete.\n", (*ship)->id);
        return;
    }

    // Deletes the ship from array of the ships still going
    flow_manager->ships_still_going[ship_index] = 0;


    /* Como ya no está en los arrays de ids midcanal */

    printf("Barco %d ha completado su recorrido por el canal.\n", (*ship)->id);

    // aumentar: ships_passed
    flow_manager->ships_passed_this_cycle++;

    // aumentar: total_ships_passed
    flow_manager->total_ships_passed++;

    // adds id to list of Ships through the canal
    flow_manager->ships_through[flow_manager->total_ships_passed-1] = (*ship)->id;

}
