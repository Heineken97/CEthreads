#ifndef SCHEDULERS_H
#define SCHEDULERS_H

typedef enum {
    ROUND_ROBIN,
    PRIORITY,
    SJF,
    FCFS,
    REAL_TIME,
    STEST
} SchedulerType;

typedef struct {
    int id;
    int priority;
    int position;
    float processing_time;
} Ship;

void priority_scheduler(Ship ships[], int ship_count);
void sjf_scheduler(Ship ships[], int ship_count);
void real_time_scheduler(Ship ships[], int ship_count, float max_time);
void round_robin_scheduler(Ship ships[], int ship_count, int time_quantum);
void fcfs_scheduler(Ship ships[], int ship_count);
#endif
