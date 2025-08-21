#include <stdio.h>
#include "monitor.h"

int monitor_init(monitor_t* monitor) {
    monitor->signaled = 0;
    if (pthread_mutex_init(&monitor->mutex, NULL)) return -1;
    if (pthread_cond_init(&monitor->cond, NULL)) {
        pthread_mutex_destroy(&monitor->mutex);
        return -1;
    }
    return 0;
}

void monitor_destroy(monitor_t* monitor) {
    if (!monitor) return;
    pthread_mutex_destroy(&monitor->mutex);
    pthread_cond_destroy(&monitor->cond);
    return;
}

void monitor_signal(monitor_t* monitor) {
    if (!monitor) return;
    pthread_mutex_lock(&monitor->mutex);
    monitor->signaled = 1;
    pthread_cond_signal(&monitor->cond); 
    //pthread_cond_broadcast(&monitor->cond);
    pthread_mutex_unlock(&monitor->mutex);
    return;
}

void monitor_reset(monitor_t* monitor) {
    if (!monitor) return;
    pthread_mutex_lock(&monitor->mutex);
    monitor->signaled = 0;
    pthread_mutex_unlock(&monitor->mutex);
    return;
}

int monitor_wait(monitor_t* monitor) {
    if (!monitor) return -1;
    pthread_mutex_lock(&monitor->mutex);
    while (!monitor->signaled) {
        pthread_cond_wait(&monitor->cond, &monitor->mutex);
    }
    monitor->signaled = 0;
    pthread_mutex_unlock(&monitor->mutex);
    return 0;
}