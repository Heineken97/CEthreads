#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_SHIPS 5

typedef enum {
    NORMAL,
    PATROL,
    FISHING
} ShipType;

typedef struct {
    ShipType type;          // Type of ship (int)
    int position;           // Position in canal L->R
} SimpleShip;

typedef struct {
    SimpleShip queue_LR_data[MAX_SHIPS];        // Ships L->R
    SimpleShip queue_RL_data[MAX_SHIPS];        // Ships R->L
    SimpleShip midcanal_data[MAX_SHIPS];        // Ships in mid-canal
    SimpleShip done_LR_ships[MAX_SHIPS];        // Completed L->R ships
    SimpleShip done_RL_ships[MAX_SHIPS];        // Completed R->L ships
    int canal_length;                   // Length of the canal
    int actual_direction;               // Current direction
    int method;                         // Flow control method (int)
    int scheduler;                      // Scheduler (int)
} InterfaceData;

int main() {
    // Create a mock serial port
    int serial_fd = open("mock_serial_port", O_RDWR);

    if (serial_fd == -1) {
        perror("Unable to open mock serial port");
        return 1;
    }

    // Create an InterfaceData struct
    InterfaceData interface_data;

    // Initial state: all ships at position 0
    interface_data.canal_length = 5; // Channel length
    interface_data.actual_direction = 0; // Initial direction (L->R)
    interface_data.method = 1; // Flow control method
    interface_data.scheduler = 2; // Scheduler

    // Initialize ships
    interface_data.queue_LR_data[0].type = NORMAL;
    interface_data.queue_LR_data[0].position = 0;
    interface_data.queue_LR_data[1].type = PATROL;
    interface_data.queue_LR_data[1].position = 0;
    interface_data.queue_LR_data[2].type = FISHING;
    interface_data.queue_LR_data[2].position = 0;
    interface_data.queue_LR_data[3].type = NORMAL;
    interface_data.queue_LR_data[3].position = 0;
    interface_data.queue_LR_data[4].type = PATROL;
    interface_data.queue_LR_data[4].position = 0;

    interface_data.queue_RL_data[0].type = FISHING;
    interface_data.queue_RL_data[0].position = 0;
    interface_data.queue_RL_data[1].type = NORMAL;
    interface_data.queue_RL_data[1].position = 0;
    interface_data.queue_RL_data[2].type = PATROL;
    interface_data.queue_RL_data[2].position = 0;
    interface_data.queue_RL_data[3].type = FISHING;
    interface_data.queue_RL_data[3].position = 0;
    interface_data.queue_RL_data[4].type = NORMAL;
    interface_data.queue_RL_data[4].position = 0;

    interface_data.midcanal_data[0].type = NORMAL;
    interface_data.midcanal_data[0].position = 0;
    interface_data.midcanal_data[1].type = PATROL;
    interface_data.midcanal_data[1].position = 0;
    interface_data.midcanal_data[2].type = FISHING;
    interface_data.midcanal_data[2].position = 0;
    interface_data.midcanal_data[3].type = NORMAL;
    interface_data.midcanal_data[3].position = 0;
    interface_data.midcanal_data[4].type = PATROL;
    interface_data.midcanal_data[4].position = 0;

    interface_data.done_LR_ships[0].type = NORMAL;
    interface_data.done_LR_ships[0].position = 0;
    interface_data.done_LR_ships[1].type = PATROL;
    interface_data.done_LR_ships[1].position = 0;
    interface_data.done_LR_ships[2].type = FISHING;
    interface_data.done_LR_ships[2].position = 0;
    interface_data.done_LR_ships[3].type = NORMAL;
    interface_data.done_LR_ships[3].position = 0;
    interface_data.done_LR_ships[4].type = PATROL;
    interface_data.done_LR_ships[4].position = 0;

    interface_data.done_RL_ships[0].type = FISHING;
    interface_data.done_RL_ships[0].position = 0;
    interface_data.done_RL_ships[1].type = NORMAL;
    interface_data.done_RL_ships[1].position = 0;
    interface_data.done_RL_ships[2].type = PATROL;
    interface_data.done_RL_ships[2].position = 0;
    interface_data.done_RL_ships[3].type = FISHING;
    interface_data.done_RL_ships[3].position = 0;
    interface_data.done_RL_ships[4].type = NORMAL;
    interface_data.done_RL_ships[4].position = 0;

    // Print the size of InterfaceData struct
    printf("Size of InterfaceData: %lu\n", sizeof(InterfaceData));

    // Send the struct with padding to ensure correct alignment
    printf("Sending initial InterfaceData struct...\n");

    write(serial_fd, &interface_data, sizeof(InterfaceData));

    sleep(2);

    // Move ships one position to the right
    for (int i = 0; i < MAX_SHIPS; i++) {
        interface_data.queue_LR_data[i].position = (interface_data.queue_LR_data[i].position + 1) % interface_data.canal_length;
        interface_data.queue_RL_data[i].position = (interface_data.queue_RL_data[i].position + 1) % interface_data.canal_length;
        interface_data.midcanal_data[i].position = (interface_data.midcanal_data[i].position + 1) % interface_data.canal_length;
        interface_data.done_LR_ships[i].position = (interface_data.done_LR_ships[i].position + 1) % interface_data.canal_length;
        interface_data.done_RL_ships[i].position = (interface_data.done_RL_ships[i].position + 1) % interface_data.canal_length;
    }

    printf("Sending updated InterfaceData struct...\n");

    write(serial_fd, &interface_data, sizeof(InterfaceData));

    sleep(2);

    // Change direction
    interface_data.actual_direction = 1; // Change direction to R->L

    // Move ships one position to the left
    for (int i = 0; i < MAX_SHIPS; i++) {
        interface_data.queue_LR_data[i].position = (interface_data.queue_LR_data[i].position - 1 + interface_data.canal_length) % interface_data.canal_length;
        interface_data.queue_RL_data[i].position = (interface_data.queue_RL_data[i].position - 1 + interface_data.canal_length) % interface_data.canal_length;
        interface_data.midcanal_data[i].position = (interface_data.midcanal_data[i].position - 1 + interface_data.canal_length) % interface_data.canal_length;
        interface_data.done_LR_ships[i].position = (interface_data.done_LR_ships[i].position - 1 + interface_data.canal_length) % interface_data.canal_length;
        interface_data.done_RL_ships[i].position = (interface_data.done_RL_ships[i].position - 1 + interface_data.canal_length) % interface_data.canal_length;
    }

    printf("Sending final InterfaceData struct...\n");

    write(serial_fd, &interface_data, sizeof(InterfaceData));

    // Close the serial port
    close(serial_fd);

    return 0;
}
