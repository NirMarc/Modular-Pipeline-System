#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "../plugins/sync/consumer_producer.h"

#define CAPACITY 10
#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 3
#define NUM_ITEMS 50

void* producer_thread(void* arg) {
    consumer_producer_t* q = (consumer_producer_t*)arg;
    for (int i = 1; i <= NUM_ITEMS; i++) {
        char item[NUM_ITEMS];
        snprintf(item, sizeof(item), "Item %d", i);
        printf("[P] Put %s\n", item);
        if (consumer_producer_put(q, item) != NULL) {
            printf("[P] Failed to put %s\n", item);
            break;
        }
    }
    printf("[P] Signaling finished\n");
    consumer_producer_signal_finished(q);
    return NULL;
}

void* consumer_thread(void* arg) {
    consumer_producer_t* q = (consumer_producer_t*)arg;
    const char* out;
    while ((out = consumer_producer_get(q)) != NULL) {
        printf("[C] Get %s\n", out);
        usleep(100000); 
    }
    if (out == NULL) {
        printf("[C] Detected finished signal\n");
    } else {
        printf("[C] get error %s\n", out);
    }
    return NULL;
}


int main() {
    printf("=== consumer_producer Tests ===\n");

    consumer_producer_t q;
    if (consumer_producer_init(&q, CAPACITY) != NULL) {
        fprintf(stderr, "consumer_producer_init failed\n");
        return 1;
    }

    pthread_t prod, cons;
    pthread_create(&cons, NULL, consumer_thread, &q);
    pthread_create(&prod, NULL, producer_thread, &q);

    printf("[M] Waiting for finished signal\n");
    consumer_producer_wait_finished(&q);
    printf("[M] Finished signal received\n");

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    consumer_producer_destroy(&q);
    printf("All tests done\n");
    return 0;
}
