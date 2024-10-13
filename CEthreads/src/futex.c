#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <linux/futex.h>
#include "../include/futex.h"

void futex_init(int *futex, int value) {
    printf("futex_init called\n");
    *futex = value;
}

void futex_down(int *futex) {
    printf("futex_down called\n");
    while (*futex != 0) {
        syscall(SYS_futex, futex, FUTEX_WAIT, 0, NULL, NULL, 0);
    }
}

void futex_up(int *futex) {
    printf("futex_up called\n");
    *futex = 1;
    syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 0);
}
