#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// Define the FlowManager struct
typedef struct {
    int method;
    int canal_length;
    int current_direction;
    int ships_passed;
    int total_ships_passed;
    int normal_ship_speed;
    int fishing_ship_speed;
    int patrol_ship_speed;
    int ships_in_queue_LR;
    int ships_in_queue_RL;
    int ships_in_midcanal_LR;
    int ships_in_midcanal_RL;
    int ships_in_done_LR;
    int ships_in_done_RL;
    int w_param;
    float t_param;
} FlowManager;

int main() {
    // Create a mock serial port
    int serial_fd = open("mock_serial_port", O_RDWR);
    if (serial_fd == -1) {
        perror("Unable to open mock serial port");
        return 1;
    }

    // Create a FlowManager struct
    FlowManager flow_manager;
    flow_manager.method = 1;
    flow_manager.canal_length = 10;
    flow_manager.current_direction = 0;
    flow_manager.ships_passed = 0;
    flow_manager.total_ships_passed = 0;
    flow_manager.normal_ship_speed = 5;
    flow_manager.fishing_ship_speed = 3;
    flow_manager.patrol_ship_speed = 2;
    flow_manager.ships_in_queue_LR = 2;
    flow_manager.ships_in_queue_RL = 1;
    flow_manager.ships_in_midcanal_LR = 0;
    flow_manager.ships_in_midcanal_RL = 0;
    flow_manager.ships_in_done_LR = 0;
    flow_manager.ships_in_done_RL = 0;
    flow_manager.w_param = 2;
    flow_manager.t_param = 1.5;

    // Simulate the sending of the FlowManager struct at different moments of the algorithm
    printf("Sending initial FlowManager struct...\n");
    write(serial_fd, &flow_manager, sizeof(FlowManager));
    sleep(2);

    // Update the FlowManager struct to simulate a change in the algorithm
    flow_manager.ships_in_queue_LR = 1;
    flow_manager.ships_in_queue_RL = 2;
    flow_manager.ships_in_midcanal_LR = 1;
    flow_manager.ships_in_midcanal_RL = 0;

    printf("Sending updated FlowManager struct...\n");
    write(serial_fd, &flow_manager, sizeof(FlowManager));
    sleep(2);

    // Update the FlowManager struct to simulate another change in the algorithm
    flow_manager.ships_in_queue_LR = 0;
    flow_manager.ships_in_queue_RL = 1;
    flow_manager.ships_in_midcanal_LR = 0;
    flow_manager.ships_in_midcanal_RL = 1;
    flow_manager.ships_in_done_LR = 1;
    flow_manager.ships_in_done_RL = 0;

    printf("Sending final FlowManager struct...\n");
    write(serial_fd, &flow_manager, sizeof(FlowManager));

    // Close the serial port
    close(serial_fd);

    return 0;
}
