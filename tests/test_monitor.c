#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../plugins/sync/monitor.h"
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

// Global variables for testing
int test_passed = 1;
int thread_received_signal = 0;
int multiple_threads_received = 0;
pthread_mutex_t test_mutex = PTHREAD_MUTEX_INITIALIZER;

void* single_thread_func(void* arg) {
    monitor_t* monitor = (monitor_t*)arg;
    printf("Thread waiting for monitor signal\n");
    int result = monitor_wait(monitor);
    if (result == 0) {
        printf("Thread received signal successfully\n");
        thread_received_signal = 1;
    } else {
        printf("Thread wait failed\n");
        test_passed = 0;
    }
    return NULL;
}

void* multiple_thread_func(void* arg) {
    monitor_t* monitor = (monitor_t*)arg;
    printf("Multiple thread waiting for monitor signal\n");
    int result = monitor_wait(monitor);
    if (result == 0) {
        pthread_mutex_lock(&test_mutex);
        multiple_threads_received++;
        printf("Multiple thread %d received signal\n", multiple_threads_received);
        pthread_mutex_unlock(&test_mutex);
    } else {
        printf("Multiple thread wait failed\n");
        test_passed = 0;
    }
    return NULL;
}

void* reset_test_func(void* arg) {
    monitor_t* monitor = (monitor_t*)arg;
    printf("Reset test thread waiting\n");
    int result = monitor_wait(monitor);
    if (result == 0) {
        printf("Reset test thread received signal\n");
    } else {
        printf("Reset test thread wait failed\n");
        test_passed = 0;
    }
    return NULL;
}

void test_basic_functionality() {
    printf("\n=== Testing Basic Functionality ===\n");
    
    monitor_t monitor;
    int init_result = monitor_init(&monitor);
    if (init_result != 0) {
        printf("FAILED: monitor_init returned %d\n", init_result);
        test_passed = 0;
        return;
    }
    printf("PASSED: monitor_init\n");
    
    // Test signal and wait
    pthread_t thread;
    thread_received_signal = 0;
    pthread_create(&thread, NULL, single_thread_func, &monitor);
    
    sleep(1); // Give thread time to start waiting
    printf("Signaling monitor\n");
    monitor_signal(&monitor);
    
    pthread_join(thread, NULL);
    
    if (thread_received_signal) {
        printf("PASSED: Signal and wait functionality\n");
    } else {
        printf("FAILED: Thread did not receive signal\n");
        test_passed = 0;
    }
    
    monitor_destroy(&monitor);
    printf("PASSED: monitor_destroy\n");
}

void test_multiple_threads() {
    printf("\n=== Testing Multiple Threads ===\n");
    
    monitor_t monitor;
    monitor_init(&monitor);
    
    pthread_t threads[3];
    multiple_threads_received = 0;
    
    // Create multiple threads
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, multiple_thread_func, &monitor);
    }
    
    sleep(1); // Give threads time to start waiting
    printf("Signaling monitor for multiple threads\n");
    monitor_signal(&monitor);
    
    // Wait for the first thread to complete
    pthread_join(threads[0], NULL);
    
    // Cancel the other threads since they will wait indefinitely
    for (int i = 1; i < 3; i++) {
        pthread_cancel(threads[i]);
        pthread_join(threads[i], NULL);
    }
    
    if (multiple_threads_received == 1) {
        printf("PASSED: Only one thread received signal (as expected)\n");
    } else {
        printf("FAILED: %d threads received signal (expected 1)\n", multiple_threads_received);
        test_passed = 0;
    }
    
    monitor_destroy(&monitor);
}

void test_reset_functionality() {
    printf("\n=== Testing Reset Functionality ===\n");
    
    monitor_t monitor;
    monitor_init(&monitor);
    
    // Signal the monitor
    monitor_signal(&monitor);
    printf("Monitor signaled\n");
    
    // Reset the monitor
    monitor_reset(&monitor);
    printf("Monitor reset\n");
    
    // Create a thread that should wait
    pthread_t thread;
    pthread_create(&thread, NULL, reset_test_func, &monitor);
    
    sleep(1); // Give thread time to start waiting
    printf("Signaling monitor after reset\n");
    monitor_signal(&monitor);
    
    pthread_join(thread, NULL);
    
    monitor_destroy(&monitor);
    printf("PASSED: Reset functionality\n");
}

void test_immediate_signal() {
    printf("\n=== Testing Immediate Signal ===\n");
    
    monitor_t monitor;
    monitor_init(&monitor);
    
    // Signal before creating thread
    monitor_signal(&monitor);
    printf("Monitor signaled before thread creation\n");
    
    // Create thread that should receive signal immediately
    pthread_t thread;
    thread_received_signal = 0;
    pthread_create(&thread, NULL, single_thread_func, &monitor);
    
    pthread_join(thread, NULL);
    
    if (thread_received_signal) {
        printf("PASSED: Thread received immediate signal\n");
    } else {
        printf("FAILED: Thread did not receive immediate signal\n");
        test_passed = 0;
    }
    
    monitor_destroy(&monitor);
}

void test_multiple_signals() {
    printf("\n=== Testing Multiple Signals ===\n");
    
    monitor_t monitor;
    monitor_init(&monitor);
    
    pthread_t thread;
    thread_received_signal = 0;
    pthread_create(&thread, NULL, single_thread_func, &monitor);
    
    sleep(1);
    
    // Send multiple signals
    for (int i = 0; i < 3; i++) {
        printf("Sending signal %d\n", i + 1);
        monitor_signal(&monitor);
        usleep(100000); // 100ms delay between signals
    }
    
    pthread_join(thread, NULL);
    
    if (thread_received_signal) {
        printf("PASSED: Thread handled multiple signals correctly\n");
    } else {
        printf("FAILED: Thread did not handle multiple signals\n");
        test_passed = 0;
    }
    
    monitor_destroy(&monitor);
}

void test_edge_cases() {
    printf("\n=== Testing Edge Cases ===\n");
    
    // Test with NULL monitor 
    printf("Testing NULL monitor (should not crash)\n");
    monitor_signal(NULL);
    monitor_reset(NULL);
    monitor_wait(NULL);
    monitor_destroy(NULL);
    printf("PASSED: NULL monitor handling\n");
    
    // Test uninitialized monitor 
    monitor_t uninit_monitor;
    printf("Testing uninitialized monitor (should not crash)\n");
    monitor_signal(&uninit_monitor);
    monitor_reset(&uninit_monitor);
    // monitor_wait(&uninit_monitor);
    monitor_destroy(&uninit_monitor);
    printf("PASSED: Uninitialized monitor handling\n");
}

int main() {
    printf("Starting Monitor Tests\n");
    
    test_basic_functionality();
    test_reset_functionality();
    test_immediate_signal();
    test_multiple_signals();
    test_edge_cases();
    
    printf("\nTest Summary\n");
    if (test_passed) {
        printf("ALL TESTS PASSED\n");
        return 0;
    } else {
        printf("SOME TESTS FAILED\n");
        return 1;
    }
}