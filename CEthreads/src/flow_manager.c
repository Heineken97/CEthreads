/*
 * File: flow_manager.c
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

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
                    transmit_canal_data(flow_manager, 0);
                    // Transmits data to hardware
                    //transmit_canal_data(flow_manager, 1);
                    // Prints actual state of data sent
                    //transmit_canal_data(flow_manager, 2);

                    pthread_mutex_unlock(&flow_manager->ship_queues);

                }
                // Si ya pasaron los W_PARAM Ships de este ciclo
                else {

                    pthread_mutex_lock(&flow_manager->ship_queues);
                    move_ships_to_done(flow_manager);
                    flow_manager->queues_changed = 1;
                    pthread_mutex_unlock(&flow_manager->ship_queues);

                    // Cambiar de dirección para el nuevo ciclo
                    flow_manager->current_direction = (flow_manager->current_direction == 0) ? 1 : 0;
                    printf("Cambio de dirección: ahora los barcos en la dirección %d podran avanzar.\n",
                                                                        flow_manager->current_direction);

                    // Se configuran las variables para iniciar un nuevo ciclo
                    flow_manager->ships_for_cycle_ready = 0;
                    flow_manager->ships_passed_this_cycle = 0; // Reiniciar contador de barcos

                    // Configurar el next_ship despues del cambio de direccion
                    // Siguiente iteracion calendarizará de nuevo

                }
            }

        }
        
/* ------------------------------------------------------------------------------------------------------------- */


/* --------------------------------------------- FLOW METHOD: SIGN --------------------------------------------- */

        else if (flow_manager->method == SIGN) {
            // Método SIGN: cambiar de dirección después de cierto tiempo

            static int elapsed_time = 0;
            if (elapsed_time >= flow_manager->t_param) {
                // Cambiar de dirección
                flow_manager->current_direction = (flow_manager->current_direction == 0) ? 1 : 0;
                elapsed_time = 0;  // Reiniciar el tiempo
                printf("Cambio de dirección después de %f s: ahora Ships en la dirección %d avanzan.\n",
                                                flow_manager->t_param, flow_manager->current_direction);
            } else {
                elapsed_time++;
                // LOGICA PARA MANEJAR SIGN
                printf("SIGN flow is managing the canal\n");
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

    int new_queue_size = 0;  // Index for the adjusted queue_LR array

    // Si la direccion es igual a 0 (I->D)
    if (flow_manager->current_direction == 0) {

        // Iterate over the current queue_LR array
        for (int i = 0; i < flow_manager->ships_in_queue_LR; i++) {
            int ship_id = flow_manager->queue_LR[i].id;
            int found = 0;

            // Check if the current ship's ID is in ids_for_canal
            for (int j = 0; j < flow_manager->ships_in_midcanal_LR; j++) {
                if (flow_manager->ids_for_canal[j] == ship_id) {
                    // Change Ships done status
                    flow_manager->queue_LR[i].is_done = 1;
                    // Move the ship to the done_LR array
                    int done_index = flow_manager->ships_in_done_LR;
                    flow_manager->done_LR[done_index] = flow_manager->queue_LR[i];
                    flow_manager->ships_in_done_LR++;  // Increment the done_LR counter
                    found = 1;  // Mark the ship as found and moved
                    break;
                }
            }

            // If the ship was not found in ids_for_canal, keep it in the adjusted queue
            if (!found) {
                new_queue_size++;
            }
        }

        // Update the number of ships in the queue_LR array
        flow_manager->ships_in_queue_LR = new_queue_size;

        // Resets the number of ships in the midcanal_LR
        flow_manager->ships_in_midcanal_LR = 0;

    }
    // Si la direccion es igual a 1 (D->I)
    else if (flow_manager->current_direction == 1) {

        // Iterate over the current queue_RL array
        for (int i = 0; i < flow_manager->ships_in_queue_RL; i++) {
            int ship_id = flow_manager->queue_RL[i].id;
            int found = 0;

            // Check if the current ship's ID is in ids_for_canal
            for (int j = 0; j < flow_manager->ships_in_midcanal_RL; j++) {
                if (flow_manager->ids_for_canal[j] == ship_id) {
                    // Move the ship to the done_RL array
                    int done_index = flow_manager->ships_in_done_RL;
                    flow_manager->done_RL[done_index] = flow_manager->queue_RL[i];
                    flow_manager->ships_in_done_RL++;  // Increment the done_RL counter
                    found = 1;  // Mark the ship as found and moved
                    // Change Ships done status
                    flow_manager->queue_RL[i].is_done = 1;
                    break;
                }
            }

            // If the ship was not found in ids_for_canal, keep it in the adjusted queue
            if (!found) {
                new_queue_size++;
            }
        }

        // Update the number of ships in the queue_RL array
        flow_manager->ships_in_queue_RL = new_queue_size;

        // Resets the number of ships in the midcanal_RL
        flow_manager->ships_in_midcanal_RL = 0;
    }
    // Error en la direccion
    else {
        printf("ERROR @ flow_manager.c (move_ships_to_done): undefined flow_manager->current_direction\n");
    }

    // Quitar los ids del array y pone todos en 0
    for (int i = 0; i < MAX_SHIPS; i++) {
        flow_manager->ids_for_canal[i] = 0;  // Ids en 0
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
        i_serial_data.canal_length = flow_manager->canal_length;

        // Current direction
        i_serial_data.current_direction = flow_manager->current_direction;

        // Flow control method (int)
        i_serial_data.method = flow_manager->method;

        // Scheduling algorithm (int)
        i_serial_data.scheduler = flow_manager->scheduler;





        // Sending the InterfaceData struct to update the Ship positions
        printf("\nSending InterfaceData struct...\n");
        write(flow_manager->interface_serial_port, &i_serial_data, sizeof(InterfaceData));





    } else if (destination == 1) {


        HardwareData h_serial_data;

        /*
         * Se agregan las variables al struct HardwareData
         */




        // Sending the HardwareData struct to update the Ship positions
        printf("\nSending HardwareData struct...\n");
        //write(flow_manager->hardware_serial_port, &h_serial_data, sizeof(HardwareData));


    } 

    // Destination error
    else {
        printf("TRANSMITTER ERROR @ flow_manager.c (transmit_canal_data): undefined destination\n");
    }

}




/*
 * Function: can_ship_advance
 * --------------------------
 * Determines if the ship is allowed to advance based on the flow_manager's current state.
 * 
 * Parameters:
 * - ship: pointer to the Ship struct representing the ship trying to advance.
 * - flow_manager: pointer to the Flow_Manager struct controlling the canal.
 * 
 * Returns:
 * - 1 if the ship is allowed to advance, 0 otherwise.
 */
int ship_can_advance(Ship* ship, FlowManager* flow_manager) {

    // Checks if ship is next so it can flow o if its midcanal already
    if (ship_is_next(ship, flow_manager) || ship->position > 0) {

/* -------------------------------------------- FLOW METHOD: EQUITY -------------------------------------------- */

        if (flow_manager->method == EQUITY) {

            // Verificar si el barco está en la dirección correcta
            if (ship->direction != flow_manager->current_direction) {
                // El barco no puede avanzar porque la dirección no está activa
                printf("El Barco %d no puede avanzar porque la dirección no está activa.\n", ship->id);
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
                    if (flow_manager->ids_for_canal[i] == ship->id) {
                        printf("Barco %d esta en la lista de barcos que puede pasar por el canal!\n", ship->id);
                        will_be_midcanal = 1; 
                    }
                }

                // Si el Ship debiera estar en medio canal 
                if (will_be_midcanal) {

                    // Si el ship si esta en medio canal
                    if (ship->position > 0) {

                        printf("Barco %d se encuentra mid canal.\n", ship->id);
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
                                if (flow_manager->scheduled_queue_LR[i] == ship->id) {
                                    ship_index = i;
                                    printf("Ship with ID %d was found in queue_LR @ i = %d.\n", ship->id, i);
                                    break;
                                }
                            }

                            // If the ship is not found in queue_LR, print an error and exit the function
                            if (ship_index == -1) {
                                printf("ERROR: Ship with ID %d not found in queue_LR.\n", ship->id);
                                return 0;
                            }

                            /*
                             *
                             * ESTO PODRIA FUNCIONAR SI AL AÑADIR EL SHIP AL ARRAY DE MIDCANAL
                             * SE CAMBIA LA REFERENCIA DE LA VARIABLE ship QUE SE PASA DEL ARRAY
                             * DEL QUEUE AL QUE ESTARIA AHORA EN EL ARRAY DE MIDCANAL, QUE SERIA
                             * EL MISMO PERO EN SU RESPECTIVA PARTE DEL CANAL CORRECTAMENTE.
                             * 
                             */

                            // Adds the ship to the end of the midcanal_LR array
                            //flow_manager->midcanal_LR[flow_manager->ships_in_midcanal_LR] = *ship;
                            
                            // Increment the counter for ships in midcanal_LR
                            flow_manager->ships_in_midcanal_LR++;

                            // Removes the Ship from the queue_LR array by shifting remaining ships
                            // for (int i = ship_index; i < flow_manager->ships_in_queue_LR - 1; i++) {
                            //    // Shift Ships left to fill the gap
                            //    flow_manager->queue_LR[i] = flow_manager->queue_LR[i + 1];
                            // }

                            // Decreases the counter for ships in queue_LR as the ship has been removed
                            //flow_manager->ships_in_queue_LR--;

                            // Ship was successfully moved
                            printf("Barco %d pasaria de queue_LR a midcanal_LR.\n", ship->id);

                            //flow_manager->queues_changed = 1;
                            pthread_mutex_unlock(&flow_manager->ship_queues);   // Desbloquea el mutex

                            // Ship puede avanzar
                            return 1;

                        }
                        // Si la direccion es igual a 1 (D->I)
                        else if (flow_manager->current_direction == 1) {

                            pthread_mutex_lock(&flow_manager->ship_queues); // Bloquea el mutex

                            // Finds the index of the ship in scheduled_queue_RL array
                            // Loops through scheduled_queue_RL array to locate the ship
                            // that matches the id of the ship being moved.
                            for (int i = 0; i < flow_manager->ships_in_queue_RL; i++) {
                                if (flow_manager->scheduled_queue_RL[i] == ship->id) {
                                    ship_index = i;
                                    break;
                                }
                            }

                            // If the ship is not found in queue_RL, print an error and exit the function
                            if (ship_index == -1) {
                                printf("Error: Ship with ID %d not found in queue_RL.\n", ship->id);
                                return 0;
                            }

                            // Adds the ship to the end of the midcanal_RL array
                            //flow_manager->midcanal_RL[flow_manager->ships_in_midcanal_RL] = *ship;
                            
                            // Increment the counter for ships in midcanal_RL
                            flow_manager->ships_in_midcanal_RL++;

                            // Removes the Ship from the queue_RL array by shifting remaining ships
                            //for (int i = ship_index; i < flow_manager->ships_in_queue_RL - 1; i++) {
                            //    // Shift Ships left to fill the gap
                            //    flow_manager->queue_RL[i] = flow_manager->queue_RL[i + 1];
                            //}

                            // Decreases the counter for ships in queue_RL as the ship has been removed
                            //flow_manager->ships_in_queue_RL--;

                            // Ship was successfully moved
                            printf("Barco %d pasó de queue_RL a midcanal_RL.\n", ship->id);

                            //flow_manager->queues_changed = 1;
                            pthread_mutex_unlock(&flow_manager->ship_queues);   // Desbloquea el mutex

                            // Ship puede avanzar
                            return 1;
                        }
                        // Error en la direccion
                        else {

                            printf("ERROR @ flow_manager.c (can_ship_advance @ EQUITY): undefined flow_manager->current_direction\n");
                            return 0;
                        }

                    }

                } else {
                    printf("NOT_ADVANCING: Barco %d no está entre los que pasarán este ciclo...\n", ship->id);
                    return 0;
                }
   
            }

        }

/* ------------------------------------------------------------------------------------------------------------- */


/* --------------------------------------------- FLOW METHOD: SIGN --------------------------------------------- */
        
        else if (flow_manager->method == SIGN) {

            printf("El Barco %d puede avanzar mientras se configura el metodo SIGN.\n", ship->id);
            return 1;

        }

/* ------------------------------------------------------------------------------------------------------------- */


/* --------------------------------------------- FLOW METHOD: TICO --------------------------------------------- */
        
        else if (flow_manager->method == TICO) {

            printf("El Barco %d puede avanzar mientras se configura el metodo TICO.\n", ship->id);
            return 1;

        }

/* ------------------------------------------------------------------------------------------------------------- */


        // Error en el metodo de flujo
        else {
            printf("ERROR @ flow_manager.c (ship_can_advance): undefined flow_manager->method\n");
        }

    } else {
        printf("El Barco %d no está calendarizado para avanzar.\n", ship->id);
        return 0;
    }

    printf("Shouldn't reach here @ flow_manager (ship_can_advance)");
    return 0;

}


/*
 * Permitira saber si el ship que está esperando es el siguiente en la lista o no
 */
int ship_is_next(Ship* ship, FlowManager* flow_manager){

    // Si la direccion es igual a 0 (I->D) && si no ha empezado el flujo o está en la primera posicion
    if (flow_manager->current_direction == 0 && ship->position <= 1) {

        // If actual next_ship is the current ship
        if (flow_manager->next_ship == ship->id) {

            /* Setting next Ship after this one */

            // Si el Ship ya salio de la espra y está mid canal en position 1
            // Y no es el ultimo del ciclo (faltan para llegar a w_param)
            // Se cambiara al siguiente next de esta direccion
            if (ship->position > 0 && flow_manager->ships_in_midcanal_LR <= flow_manager->w_param) {

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

            printf("El Barco %d is next.\n", ship->id);
            return 1;  // 1 to indicate that the ship is next

        } else {
            printf("El Barco %d is not next.\n", ship->id);
            return 0;  // 0 if no match is found
        }
    } 
    // Si la direccion es igual a 1 (D->I) && si no ha empezado el flujo o está en la primera posicion
    else if (flow_manager->current_direction == 1 && ship->position <= 1) {

        // If actual next_ship is the current ship
        if (flow_manager->next_ship == ship->id) {

            // Si el Ship ya salio de la espra y está mid canal en position 1
            // Y no es el ultimo del ciclo (faltan para llegar a w_param)
            // Se cambiara al siguiente next de esta direccion
            if (ship->position > 0 && flow_manager->ships_in_midcanal_RL < flow_manager->w_param) {

                // Mutex que protege la variable de next_ship
                pthread_mutex_lock(&flow_manager->next_ship_mutex); // Bloquea el mutex

                // Elimina de la lista calendarizada al barco que ya esta midcanal en position 1
                for (int i = 0; i < flow_manager->ships_in_queue_RL - 1; i++) {
                    // Shift elements in scheduled_queue_LR to the left
                    flow_manager->scheduled_queue_RL[i] = flow_manager->scheduled_queue_RL[i + 1];
                }

                // Configura el siguiente en lista al proximo next_ship
                flow_manager->next_ship = flow_manager->scheduled_queue_RL[0];
                pthread_mutex_unlock(&flow_manager->next_ship_mutex); // Bloquea el mutex
            }

            printf("El Barco %d is next.\n", ship->id);
            return 1;  // 1 to indicate that the ship is next

        } else {
            printf("El Barco %d is not next.\n", ship->id);
            return 0;  // 0 if no match is found
        }
    } else {
        // Ship is already midcanal
        return 0;
    }

}
