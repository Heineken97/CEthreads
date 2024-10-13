// schedulers.c
#include "../include/schedulers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to sort ships using the Priority algorithm
void priority_scheduler(Ship ships[], int ship_count) {
    // Sort ships based on priority (higher value has higher priority)
    for (int i = 0; i < ship_count - 1; i++) {
        for (int j = i + 1; j < ship_count; j++) {
            if (ships[i].priority < ships[j].priority) {
                Ship temp = ships[i];
                ships[i] = ships[j];
                ships[j] = temp;
            }
        }
    }
    printf("Ships sorted by Priority:\n");
    for (int i = 0; i < ship_count; i++) {
        printf("Ship ID: %d, Priority: %d\n", ships[i].id, ships[i].priority);
    }

    // Simulate Priority scheduling
    float total_time = 0;
    printf("Priority scheduler:\n");
    for (int i = 0; i < ship_count; i++) {
        printf("Ship ID: %d, Time: %.2f\n", ships[i].id, total_time);
        total_time += ships[i].processing_time;
    }
    printf("Total Time: %.2f\n", total_time);
}

// Function to simulate Shortest Job First (SJF) scheduling
void sjf_scheduler(Ship ships[], int ship_count) {
    printf("Before sorting:\n");
    for (int i = 0; i < ship_count; i++) {
        printf("Ship ID: %d, Processing Time: %.2f\n", ships[i].id, ships[i].processing_time);
    }

    // Sort ships based on processing time (shorter jobs first)
    for (int i = 0; i < ship_count - 1; i++) {
        for (int j = i + 1; j < ship_count; j++) {
            if (ships[i].processing_time > ships[j].processing_time) {
                Ship temp = ships[i];
                ships[i] = ships[j];
                ships[j] = temp;
            }
        }
    }

    printf("After sorting:\n");
    for (int i = 0; i < ship_count; i++) {
        printf("Ship ID: %d, Processing Time: %.2f\n", ships[i].id, ships[i].processing_time);
    }

    // Simulate SJF scheduling
    float total_time = 0;
    printf("SJF scheduler:\n");
    for (int i = 0; i < ship_count; i++) {
        printf("Ship ID: %d, Time: %.2f\n", ships[i].id, total_time);
        total_time += ships[i].processing_time;
    }
    printf("Total Time: %.2f\n", total_time);
}

// Function to simulate Round Robin scheduling
void round_robin_scheduler(Ship ships[], int ship_count, int time_quantum) {
    int total_time = 0;
    int completed = 0;
    float remaining_time[ship_count];

    // Initialize remaining times
    for (int i = 0; i < ship_count; i++) {
        remaining_time[i] = ships[i].processing_time;
    }

    while (completed < ship_count) {
        for (int i = 0; i < ship_count; i++) {
            if (remaining_time[i] > 0) {
                printf("Ship ID: %d, Time: %d\n", ships[i].id, total_time);
                if (remaining_time[i] <= time_quantum) {
                    total_time += remaining_time[i];
                    remaining_time[i] = 0;
                    completed++;
                } else {
                    remaining_time[i] -= time_quantum;
                    total_time += time_quantum;
                }
            }
        }
    }
    printf("Total Time: %d\n", total_time);
}

// Function to simulate First Come First Serve (FCFS) scheduling
void fcfs_scheduler(Ship ships[], int ship_count) {
    float total_time = 0;
    printf("FCFS scheduler:\n");
    for (int i = 0; i < ship_count; i++) {
        printf("Ship ID: %d, Time: %.2f\n", ships[i].id, total_time);
        total_time += ships[i].processing_time;
    }
    printf("Total Time: %.2f\n", total_time);
}

// Function to simulate Real Time scheduler
void real_time_scheduler(Ship ships[], int ship_count, float time_slice) {
    printf("Real Time scheduler:\n");
    float total_time = 0;
    for (int i = 0; i < ship_count; i++) {
        printf("Ship ID: %d, Time: %.2f\n", ships[i].id, time_slice);
        total_time += time_slice;
    }
    printf("Total Time: %.2f\n", total_time);
}
