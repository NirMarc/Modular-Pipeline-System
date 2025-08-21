#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "consumer_producer.h"

const char* consumer_producer_init(consumer_producer_t* queue, int capacity){
    if (!queue) return "Queue is NULL";
    if (capacity <= 0) return "Capacity must be greater than 0";
    queue->items = malloc(capacity * sizeof(char*));
    if (!queue->items) return "Failed to allocate memory";
    queue->capacity = capacity;
    queue->size = 0;
    queue->head = 0;
    queue->tail = 0;
    queue->finished = 0;  
    pthread_mutex_init(&queue->mutex, NULL);
    monitor_init(&queue->not_full_monitor);
    monitor_init(&queue->not_empty_monitor);
    monitor_init(&queue->finished_monitor);
    return NULL;
}

void consumer_producer_destroy(consumer_producer_t* queue){
    if (!queue) return;
    free(queue->items);
    pthread_mutex_destroy(&queue->mutex);
    monitor_destroy(&queue->not_full_monitor);
    monitor_destroy(&queue->not_empty_monitor);
    monitor_destroy(&queue->finished_monitor);
    return;
}

const char* consumer_producer_put(consumer_producer_t* queue, const char* item){
    if (!queue) return "Queue is NULL";
    if (!item) return "Item is NULL";
    pthread_mutex_lock(&queue->mutex);
    if (queue->finished) {
        pthread_mutex_unlock(&queue->mutex);
        return "Queue is finished";
    }
    while (queue->size == queue->capacity) {
        pthread_mutex_unlock(&queue->mutex);
        monitor_wait(&queue->not_full_monitor);
        pthread_mutex_lock(&queue->mutex);
        if (queue->finished) {
            pthread_mutex_unlock(&queue->mutex);
            return "Queue is finished";
        }
    }
    queue->items[queue->tail] = strdup(item);
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->size++;
    monitor_signal(&queue->not_empty_monitor);
    pthread_mutex_unlock(&queue->mutex);
    return NULL;
}

char* consumer_producer_get(consumer_producer_t* queue){
    if (!queue) return NULL;
    pthread_mutex_lock(&queue->mutex);
    while (queue->size == 0) {
        if (queue->finished) {
            pthread_mutex_unlock(&queue->mutex);
            return NULL; 
        }
        pthread_mutex_unlock(&queue->mutex);
        monitor_wait(&queue->not_empty_monitor);
        pthread_mutex_lock(&queue->mutex);
        if (queue->finished && queue->size == 0) {
            pthread_mutex_unlock(&queue->mutex);
            return NULL; 
        }
    }
    char* item = queue->items[queue->head];
    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;
    monitor_signal(&queue->not_full_monitor);
    pthread_mutex_unlock(&queue->mutex);
    return item;
}

void consumer_producer_signal_finished(consumer_producer_t* queue){
    if (!queue) return;
    pthread_mutex_lock(&queue->mutex);
    queue->finished = 1;  
    monitor_signal(&queue->finished_monitor);
    monitor_signal(&queue->not_empty_monitor);
    monitor_signal(&queue->not_full_monitor);
    pthread_mutex_unlock(&queue->mutex);
    return;
}

int consumer_producer_wait_finished(consumer_producer_t* queue){
    if (!queue) return -1;
    monitor_wait(&queue->finished_monitor);
    return 0;
}