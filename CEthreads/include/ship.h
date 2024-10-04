/*
 * File: ship.h
 *
 */
#ifndef SHIPS_H
#define SHIPS_H

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

} Ship;

// Function to create a new ship
Ship* create_ship(int id, ShipType type, int direction, double speed, int priority);

// Function to update the position of the ship in the canal
void update_position(Ship* ship, int new_position);

// Function to print ship information (for debugging purposes)
void print_ship(Ship* ship);

// Function to free the memory allocated for a ship
void free_ship(Ship* ship);

#endif // SHIPS_H
