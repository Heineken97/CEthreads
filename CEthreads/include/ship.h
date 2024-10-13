/*
 * File: ship.h
 *
 */
#ifndef SHIPS_H
#define SHIPS_H

#include "../include/CEthreads.h"

// Enumerate ship types
typedef enum {
    NORMAL,
    FISHING,
    PATROL
} ShipType;

typedef struct {
    int id;                 // Unique identifier for the ship
    ShipType type;          // Type of ship (NORMAL, FISHING, PATROL)
    int direction;          // Direction (0: left to right, 1: right to left)
    double speed;           // Speed of the ship
    int priority;           // Priority level (for scheduling algorithms)
    int position;           // Position in channel
    int is_done;            // If Ship completed its journey
    float processing_time;   // Processing time for the ship
    CEthread_t thread;       // CEthread associated with the ship
} Ship;

// Function to create a new ship
Ship* create_ship(int id, ShipType type, int direction, double speed);

// Function to print ship information (for debugging purposes)
void print_ship(Ship* ship);

// Function to free the memory allocated for a ship
void free_ship(Ship* ship);

// Function to move the ship through the canal
void* move_ship(void* arg);

#endif // SHIPS_H
