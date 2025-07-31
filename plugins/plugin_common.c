#include <stdio.h>
#include "plugin_common.h"

void* plugin_consumer_thread(void* arg) {
    return NULL;
}

void log_error(plugin_context_t* context, const char* message) {
    return;
}

void log_info(plugin_context_t* context, const char* message) {
    return;
}

const char* common_plugin_init(const char* (*process_function)(const char*), const char* name, int queue_size) {
    return NULL;
}

__attribute__((visibility("default"))) const char* plugin_fini(void) {
    return NULL;
}

__attribute__((visibility("default"))) const char* plugin_place_work(const char* str) {
    return NULL;
}

__attribute__((visibility("default"))) void plugin_attach(const char* (*next_place_work)(const char*)) {
    return;
}

__attribute__((visibility("default"))) const char* plugin_wait_finished(void) {
    return NULL;
}