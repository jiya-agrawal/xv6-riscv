#ifndef THREAD_H
#define THREAD_H

// Define the ticket lock structure
typedef struct __lock_t {
    uint ticket;
    uint turn;
} lock_t;

// Ticket lock functions
void lock_init(lock_t *lock);
void lock_acquire(lock_t *lock);
void lock_release(lock_t *lock);

#endif // THREAD_H