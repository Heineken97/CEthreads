/*
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *entry_point(void *value) {
    printf("hello from the second thread :)\n");

    int *num = (int *) value;

    printf("the value of num is %d\n", *num);

    return NULL;
}

int main(int argc, char **argv) {
    pthread_t thread;

    printf("hello from the first thread :D\n");

    int num = 123;

    pthread_create(&thread, NULL, entry_point, &num);

    pthread_join(thread, NULL);

    return EXIT_SUCCESS;
}
*/
#include <stdio.h>
#include <stdlib.h>
#include "../include/CEthreads.h"  // Include your custom CEthread header

CEmutex_t mutex;

void *entry_point(void *value) {
    printf("Hello from the second thread :)\n");

    int *num = (int *) value;
    printf("The value of num is %d\n", *num);

    return NULL;
}

int main(int argc, char **argv) {
    CEthread_t thread;  // Change from pthread_t to CEthread_t

    printf("Hello from the first thread :D\n");

    int num = 123;

    CEmutex_init(&mutex);

    // Replace pthread_create with CEthread_create
    CEthread_create(&thread, NULL, entry_point, &num);

    // Replace pthread_join with CEthread_join
    CEthread_join(thread, NULL);

    CEmutex_unlock(&mutex);

    CEmutex_destroy(&mutex);

    return EXIT_SUCCESS;
}
