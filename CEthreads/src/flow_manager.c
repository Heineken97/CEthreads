/*
 * File: flow_manager.c
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "flow_manager.h"

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
        if (flow_manager->ship_added) {
            // calendarizar

            /* Test example, falta implementar */
            flow_manager->scheduled_queue_LR[0] = 1;
            flow_manager->scheduled_queue_LR[1] = 2;
            flow_manager->scheduled_queue_LR[2] = 3;
            flow_manager->scheduled_queue_RL[0] = 4;
            flow_manager->scheduled_queue_RL[1] = 5;
            flow_manager->scheduled_queue_RL[2] = 6;
            /* Test example, falta implementar */

            flow_manager->ship_added = 0;

        }


        // Método EQUITY: permitir W_PARAM Ships antes de cambiar de dirección
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

                        // Ingresa los id de los Ships en el array que podrian estar en el canal
                        for (int i = 0; i < ships_to_add; i++) {
                            flow_manager->ids_for_canal[i] = flow_manager->queue_LR[i].id;
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
                            flow_manager->ids_for_canal[i] = flow_manager->queue_RL[i].id;
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

            // Si aun no se termina el ciclo y faltan Ships por pasar el canal
            else if (flow_manager->ships_passed_this_cycle < flow_manager->w_param) {

                printf("\n -------- EQUITY Midcycle -------- \n");

            }
            // Si ya pasaron los W_PARAM Ships de este ciclo
            else {

                // Revisar que los ids del array no estén en el array del queue
                int not_out_of_queue = check_duplicate_ids(flow_manager);
                printf("Hay %d Ships aun en el queue y ya terminó su ciclo\n", not_out_of_queue);

                // Quitar los ids del array y pone todos en 0
                for (int i = 0; i < MAX_SHIPS; i++) {
                    flow_manager->ids_for_canal[i] = 0;  // Ids en 0 al inicio
                }

                // Cambiar de dirección para el nuevo ciclo
                flow_manager->current_direction = (flow_manager->current_direction == 0) ? 1 : 0;
                printf("Cambio de dirección: ahora los barcos en la dirección %d podran avanzar.\n",
                                                                    flow_manager->current_direction);

                // Se configuran las variables para iniciar un nuevo ciclo
                flow_manager->ships_for_cycle_ready = 0;
                flow_manager->ships_passed_this_cycle = 0; // Reiniciar contador de barcos

            }

        }
        
        

        
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
        
        

        
        else if (flow_manager->method == TICO) {
            // Método TICO: alternar un barco de cada lado

            flow_manager->current_direction = (flow_manager->current_direction == 0) ? 1 : 0;
            printf("Alternancia TICO: ahora los barcos en la dirección %d pueden avanzar.\n", flow_manager->current_direction);

            // Check for ships in canals and the actual direction
            // LOGICA PARA MANEJAR SIGN
            printf("TICO flow is managing the canal\n");

        }
        usleep(BASE_TIME / flow_manager->patrol_ship_speed);  // Wait for fastest Ship speed time 

    }

    return NULL;

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

    int is_next;

    /*
     *
     *
     * AQUI ES DONDE SE TIENE QUE REVISAR SI VA EN EL ORDEN
     * QUE SE PUSO DESDE EL CALENDARIZADOR
     *
     * 
     */
    // Permiterá saber si el ship que está esperando es el siguiente en la lista
    // DEPENDIENDO DE LA DIRECCION
    if (flow_manager->current_direction == 0) {

        // Direction Left-to-Right (queue_LR)
        if (flow_manager->scheduled_queue_LR[0] == ship->id) {

            // Shift elements in scheduled_queue_LR to the left
            for (int i = 0; i < flow_manager->ships_in_queue_LR - 1; i++) {
                flow_manager->scheduled_queue_LR[i] = flow_manager->scheduled_queue_LR[i + 1];
            }
            printf("El Barco %d is next.\n", ship->id);
            is_next = 1;  // 1 to indicate that the ship is next
        } else {
            printf("El Barco %d is not next.\n", ship->id);
            is_next = 0;  // 0 if no match is found
        }
    } else if (flow_manager->current_direction == 1) {

        // Direction Right-to-Left (queue_RL)
        if (flow_manager->scheduled_queue_RL[0] == ship->id) {

            // Shift elements in scheduled_queue_RL to the left
            for (int i = 0; i < flow_manager->ships_in_queue_RL - 1; i++) {
                flow_manager->scheduled_queue_RL[i] = flow_manager->scheduled_queue_RL[i + 1];
            }

            is_next = 1;  // 1 to indicate that the ship is next
        } else {
            is_next = 0;  // 0 if no match is found
        }
    }
    

    if (is_next) {

        // Dependiendo del método de flujo se aplican restricciones
        if (flow_manager->method == EQUITY) {

            // Verificar si el barco está en la dirección correcta
            if (ship->direction != flow_manager->current_direction) {
                printf("El Barco %d no puede avanzar porque la dirección no está activa.\n", ship->id);
                return 0;  // El barco no puede avanzar porque la dirección no está activa
            }

            // Ya se completaron los W_PARAM Ships de este ciclo, no puede pasar
            if (flow_manager->ships_passed_this_cycle >= flow_manager->w_param) {
                printf("No más barcos pueden avanzar en esta dirección hasta que se cambie.\n");
                return 0;  // No más barcos pueden avanzar en esta dirección hasta que se cambie
            }
            else {
                int will_be_midcanal = 0;
                // Busca el id del Ship en la lista de los barcos que podrian estar en el canal
                for (int i = 0; i < flow_manager->w_param; i++) {
                    if (flow_manager->ids_for_canal[i] == ship->id) {
                        printf("Barco %d esta en la lista de barcos que puede pasar por el canal!\n", ship->id);
                        will_be_midcanal = 1; // Ship esta en la lista de barcos que puede pasar por el canal
                    }
                }

                // Si el Ship deberia estar en medio canal 
                if (will_be_midcanal) {

                    // Si el ship si esta en medio canal
                    if (ship->position > 0) {

                        printf("Barco %d se encuentra mid canal.\n", ship->id);
                        // Puede continuar avanzando
                        return 1;
                    }
                    // El Ship está por comenzar a pasar el canal
                    else {

                        // Para buscar el index del Ship en queue array
                        int ship_index = -1;                        

                        // Si la direccion es igual a 0 (I->D)
                        if (flow_manager->current_direction == 0) {

                            // Finds the index of the ship in the queue_LR array
                            // Loops through the queue_LR array to locate the ship that matches the id of the ship being moved.
                            for (int i = 0; i < flow_manager->ships_in_queue_LR; i++) {
                                if (flow_manager->queue_LR[i].id == ship->id) {
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

                            // Adds the ship to the end of the midcanal_LR array
                            flow_manager->midcanal_LR[flow_manager->ships_in_midcanal_LR] = *ship;      // Place the ship in midcanal_LR's last position
                            flow_manager->ships_in_midcanal_LR++;               // Increment the counter for ships in midcanal_LR

                            // Removes the Ship from the queue_LR array by shifting remaining ships
                            for (int i = ship_index; i < flow_manager->ships_in_queue_LR - 1; i++) {
                                flow_manager->queue_LR[i] = flow_manager->queue_LR[i + 1];  // Shift Ships left to fill the gap
                            }

                            // Decreases the counter for ships in queue_LR as the ship has been removed
                            flow_manager->ships_in_queue_LR--;

                            // Ship was successfully moved
                            printf("Barco %d pasó de queue_LR a midcanal_LR.\n", ship->id);

                            // Ship puede avanzar
                            return 1;

                        }
                        // Si la direccion es igual a 1 (D->I)
                        else if (flow_manager->current_direction == 1) {

                            // Finds the index of the ship in the queue_RL array
                            // Loops through the queue_RL array to locate the ship that matches the id of the ship being moved.
                            for (int i = 0; i < flow_manager->ships_in_queue_RL; i++) {
                                if (flow_manager->queue_RL[i].id == ship->id) {
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
                            flow_manager->midcanal_RL[flow_manager->ships_in_midcanal_RL] = *ship;      // Place the ship in midcanal_RL's last position
                            flow_manager->ships_in_midcanal_RL++;               // Increment the counter for ships in midcanal_RL

                            // Removes the Ship from the queue_RL array by shifting remaining ships
                            for (int i = ship_index; i < flow_manager->ships_in_queue_RL - 1; i++) {
                                flow_manager->queue_RL[i] = flow_manager->queue_RL[i + 1];  // Shift Ships left to fill the gap
                            }

                            // Decreases the counter for ships in queue_RL as the ship has been removed
                            flow_manager->ships_in_queue_RL--;

                            // Ship was successfully moved
                            printf("Barco %d pasó de queue_RL a midcanal_RL.\n", ship->id);

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
        else if (flow_manager->method == SIGN) {

            printf("El Barco %d puede avanzar mientras se configura el metodo SIGN.\n", ship->id);
            return 1;

        }
        else if (flow_manager->method == TICO) {

            printf("El Barco %d puede avanzar mientras se configura el metodo TICO.\n", ship->id);
            return 1;

        }
        

    } else {
        printf("El Barco %d no está calendarizado para avanzar.\n", ship->id);
        return 0;
    }

    printf("Shouldn't reach here @ flow_manager (ship_can_advance)");
    return 0;

}


/*
 * Function: check_duplicate_ids
 * -----------------------------
 * Verifica si los ids en el array ids_for_canal ya existen en el array de ships en su respectiva cola.
 * 
 * Parameters:
 * - ids_for_canal: array de ints con los IDs que se deben verificar
 * - num_ids: cantidad de IDs en el array ids_for_canal
 * - queue_LR: array de Ships de izquierda a derecha (Izq -> Der)
 * - ships_in_queue_LR: cantidad de Ships en el array queue_LR
 * - queue_RL: array de Ships de derecha a izquierda (Der -> Izq)
 * - ships_in_queue_RL: cantidad de Ships en el array queue_RL
 * 
 * Returns:
 * - cantidad de Ships duplicados
 */
// int check_duplicate_ids(int ids_for_canal[], int num_ids, Ship queue_LR[], int ships_in_queue_LR, Ship queue_RL[], int ships_in_queue_RL) {
int check_duplicate_ids(FlowManager* flow_manager) {

    int n = 0;

    // Si la direccion es igual a 0 (I->D)
    if (flow_manager->current_direction == 0) {

        // Recorrer los IDs en ids_for_canal[]
        for (int i = 0; i < MAX_SHIPS; i++) {
        
            // Verificar en la lista queue_LR (Izq -> Der)
            for (int j = 0; j < flow_manager->ships_in_queue_LR; j++) {
                if (flow_manager->ids_for_canal[i] == flow_manager->queue_LR[j].id) {
                    printf("!!! ID duplicado encontrado en queue_LR: %d\n", flow_manager->ids_for_canal[i]);
                    n++;  // Agrega 1 si se encuentra un duplicado en queue_LR
                }
            }

        }

    }
    // Si la direccion es igual a 1 (D->I)
    else if (flow_manager->current_direction == 1) {

        // Recorrer los IDs en ids_for_canal[]
        for (int i = 0; i < MAX_SHIPS; i++) {

            // Verificar en la lista queue_RL (Der -> Izq)
            for (int k = 0; k < flow_manager->ships_in_queue_RL; k++) {
                if (flow_manager->ids_for_canal[i] == flow_manager->queue_RL[k].id) {
                    printf("!!! ID duplicado encontrado en queue_RL: %d\n", flow_manager->ids_for_canal[i]);
                    n++;  // Agrega 1 si se encuentra un duplicado en queue_RL
                }
            }

        }

    }
    // Error en la direccion
    else {
        printf("ERROR @ flow_manager.c (check_duplicate_ids): undefined flow_manager->current_direction\n");
    }

    return n;  // Retorna la cantidad
}
