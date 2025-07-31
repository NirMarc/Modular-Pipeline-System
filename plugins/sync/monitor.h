#ifndef MONITOR_H
#define MONITOR_H

#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex; // Mutex for thread safety
    pthread_cond_t cond; // Condition variable
    int signaled; // Flag to remember if monitor has been signaled
} monitor_t;

/**
 * Initialize a monitor
 * @param monitor Pointer to the monitor structure
 * @return 0 on success, -1 on failure
 */
int monitor_init(monitor_t* monitor);

/**
 * Destroy a monitor and free its resources
 * @param monitor Pointer to the monitor structure
 */
void monitor_destroy(monitor_t* monitor);

/**
 * Signal a monitor (set the monitor state)
 * @param monitor Pointer to the monitor structure
 */
void monitor_signal(monitor_t* monitor);

/**
 * Reset a monitor (clear the monitor state)
 * @param monitor Pointer to the monitor structure
 */
void monitor_reset(monitor_t* monitor);

/**
 * Wait for a monitor to be signaled (infinite wait)
 * @param monitor Pointer to the monitor structure
 * @return 0 on success, -1 on failure
 */
int monitor_wait(monitor_t* monitor);

#endif