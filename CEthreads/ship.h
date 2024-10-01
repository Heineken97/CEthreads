#ifndef SHIP_H
#define SHIP_H

#include "CEthreads.h"

#define CHANNEL_LENGTH 10

typedef enum {
    NORMAL,
    PESQUERA,
    PATRULLA
} ShipType;

typedef struct Ship {
    ShipType type;
    int position; // Posición en el canal
    CEthread thread; // Hilo asociado al barco
} Ship;

#endif // SHIP_H
