/*
 * File: config.h
 * --------------
 * Configuración del Canal de Tránsito de Barcos.
 * Este archivo define los parámetros para la simulación del canal y
 * el control de flujo de los barcos.
 */

#ifndef CONFIG_H
#define CONFIG_H

// Método de control de flujo (opciones: EQUITY, SIGN, TICO)
#define FLOW_METHOD SIGN   // Cambiar a EQUITY o TICO según se necesite

// Largo del canal (en unidades arbitrarias)
#define CANAL_LENGTH 2

// Velocidades de los barcos (en unidades de tiempo)
#define NORMAL_SPEED 1
#define FISHING_SPEED 4
#define PATROL_SPEED 9

// Parámetro W para el control de flujo de equidad
// (cantidad de barcos que cruzan antes de cambiar de dirección, solo aplica para EQUITY)
#define W_PARAM 7

// Tiempo de cambio del letrero (en segundos)
// (solo aplica si se usa el método de control "SIGN")
#define SIGN_CHANGE_TIME 6.9

/*
 * Lista de Barcos
 * ---------------
 * Cada barco tiene un tipo y una dirección.
 * Los barcos se definen en un array con la estructura: {Tipo, Dirección}
 * - Tipo: NORMAL, FISHING, PATROL
 * - Dirección: 
 *   0 -> Izquierda a Derecha
 *   1 -> Derecha a Izquierda
 */
#define MAX_SHIPS 100
#define SHIP_LIST { \
    { NORMAL, 0 }, \
    { FISHING, 1 }, \
    { PATROL, 0 }, \
    { NORMAL, 1 }, \
    { FISHING, 0 } \
}

#endif // CONFIG_H
