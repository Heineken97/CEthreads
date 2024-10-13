#ifndef _FUTEX_INLINE_H
#define _FUTEX_INLINE_H

static inline int __atomic_dec(int *counter) {
    return __sync_sub_and_fetch(counter, 1);
}

static inline int __atomic_inc(int *counter) {
    return __sync_add_and_fetch(counter, 1);
}

static inline int __futex_down(int *counter) {
    return futex_down(counter);
}

static inline int __futex_up(int *counter) {
    return futex_up(counter);
}

static inline void __futex_commit(void) {
    // no-op
}

#endif /* _FUTEX_INLINE_H */
