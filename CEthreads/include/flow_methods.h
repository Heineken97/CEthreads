/*
 * File: flow_methods.h
 *
 */
#ifndef FLOW_METHODS_H
#define FLOW_METHODS_H

// Definición del enum para los métodos de flujo
typedef enum {
    NONE,           // Undefined
    EQUITY,         // Equidad
    SIGN,           // Letrero
    TICO            // Tico
} FlowMethod;

// Declaraciones de funciones para cada método de flujo
void equity_flow(int W);                // Método de flujo: Equidad
void sign_flow(int change_time);        // Método de flujo: Letrero
void tico_flow();                       // Método de flujo: Tico

// Función para convertir un string a FlowMethod
FlowMethod get_flow_method(const char* method_name);

#endif // FLOW_METHODS_H
