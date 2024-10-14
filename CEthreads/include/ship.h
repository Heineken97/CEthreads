/*
 * File: ship.h
 *
 */
#ifndef SHIPS_H
#define SHIPS_H

// #include "CEthreads.h"
#include <pthread.h>

// Enumerate ship types
typedef enum {
    NORMAL,
    FISHING,
    PATROL
} ShipType;

// Structure that represents a ship
typedef struct {
    int id;                 // Unique identifier for the ship
    ShipType type;          // Type of ship (NORMAL, FISHING, PATROL)
    int direction;          // Direction (0: left to right, 1: right to left)
    double speed;           // Speed of the ship
    int priority;           // Priority level (for scheduling algorithms)
    int position;           // Position in channel
    int is_done;            // If Ship completed its journey
    // CEthread thread; // Hilo asociado al barco
    pthread_t thread;       // Hilo asociado al barco
} Ship;

// Structure that represents a simplified ship
typedef struct {
    ShipType type;          // Type of ship (int) [0->NORMAL, 1->FISHING, 2->PATROL]
    int position;           // Position in canal from left to right
} SimpleShip;


// Function to create a new ship
Ship* create_ship(int id, ShipType type, int direction, double speed);

// Function to print ship information (for debugging purposes)
void print_ship(Ship* ship);

// Function to free the memory allocated for a ship
void free_ship(Ship* ship);

// Function to move the ship through the canal
void* move_ship(void* arg);

#endif // SHIPS_H
