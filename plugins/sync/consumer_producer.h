#ifndef CONSUMER_PRODUCER_H
#define CONSUMER_PRODUCER_H

#include "monitor.h"

typedef struct {
    char** items;
    int capacity;
    int size;
    int head;
    int tail;
    int finished;  // Flag to indicate queue is finished
    monitor_t not_full_monitor;
    monitor_t not_empty_monitor;
    monitor_t finished_monitor;
} consumer_producer_t;

/**
 * Initialize a consumer_producer_t queue
 * @param queue Pointer to the queue structure
 * @param capacity Maximum number of items that can be queued
 * @return NULL on success, error message on failure
 */
const char* consumer_producer_init(consumer_producer_t* queue, int capacity);

/**
 * Destroy a consumer-producer queue and free its resources
 * @param queue Pointer to the queue structure
 */
void consumer_producer_destroy(consumer_producer_t* queue);

/**
 * Add an item to the queue (producer)
 * Blocks if the queue is full
 * @param queue Pointer to the queue structure
 * @param item String to add (queue takes ownership)
 * @return NULL on success, error message on failure
 */
const char* consumer_producer_put(consumer_producer_t* queue, const char* item);

/**
 * Remove an item from the queue (consumer) and returns it
 * Blocks if the queue is empty
 * @param queue Pointer to the queue structure
 * @return String item or NULL if queue is empty
 */
const char* consumer_producer_get(consumer_producer_t* queue);

/**
 * Signal that processing is finished
 * @param queue Pointer to the queue structure
 */
void consumer_producer_signal_finished(consumer_producer_t* queue);

/**
 * Wait for processing to be finished
 * @param queue Pointer to the queue structure
 * @return 0 on success, -1 on timeout
 */
int consumer_producer_wait_finished(consumer_producer_t* queue);

#endif