#ifndef FUTEX_H
#define FUTEX_H

void futex_init(int *futex, int value);
void futex_down(int *futex);
void futex_up(int *futex);

#endif  // FUTEX_H
