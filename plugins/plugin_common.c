#include <stdio.h>
#include "plugin_common.h"
#include "plugin_sdk.h"
#include <unistd.h>

static plugin_context_t* g_context = NULL;

void* plugin_consumer_thread(void* arg) {
    plugin_context_t* context = (plugin_context_t*)arg;
    char* input;
    while ((input = consumer_producer_get(context->queue)) != NULL) {
        char* output = (char*)context->process_function(input);
        if (context->next_place_work != NULL) {
            context->next_place_work(output);
        }
        if (output != input) free(output);
    }
    free(input);
    context->finished = 1;
    return NULL;
}

void log_error(plugin_context_t* context, const char* message) {
    printf("[ERROR][%s] - %s\n", context->name, message);
}

void log_info(plugin_context_t* context, const char* message) {
    printf("[INFO][%s] - %s\n", context->name, message);
}

const char* common_plugin_init(const char* (*process_function)(const char*), const char* name, int queue_size) {
    plugin_context_t* context = malloc(sizeof(plugin_context_t));
    if (!context) {
        return "Could not allocate memory for plugin context";
    }
    context->name = name;
    context->process_function = process_function;
    context->initialized = 0;
    context->finished = 0;
    context->consumer_thread = 0;
    context->next_place_work = NULL; 
    context->queue = malloc(sizeof(consumer_producer_t));
    if (!context->queue) {
        free(context);
        return "Could not allocate memory for plugin queue";
    }
    if (consumer_producer_init(context->queue, queue_size) != NULL) {
        free(context->queue);
        free(context);
        return "Could not initialize plugin queue";
    }
    if (pthread_create(&context->consumer_thread, NULL, plugin_consumer_thread, context) != 0) {
        free(context->queue);
        free(context);
        return "Could not create consumer thread";
    }
    context->initialized = 1;
    g_context = context;
    return NULL;
}

__attribute__((visibility("default"))) const char* plugin_fini(void) {
    if (!g_context) return "Plugin context not initialized";
    
    const char* err = plugin_wait_finished();
    if (err != NULL) return err;
  
    consumer_producer_destroy(g_context->queue);
    free(g_context->queue);
    g_context->queue = NULL;
    free(g_context);
    g_context = NULL;
    return NULL;
}


__attribute__((visibility("default"))) const char* plugin_place_work(const char* str) {
    if (!g_context) return "Plugin context not initialized";
    //char* work = malloc(strlen(str) + 1);
    //if (!work) return "Could not allocate memory for work";
    //strcpy(work, str);
    //if (consumer_producer_put(g_context->queue, str) != NULL) {
        //free(work);
    //     return "Could not place work in plugin queue";
    // }
    //return NULL;
    const char* err = consumer_producer_put(g_context->queue, str);
    return err;
}

__attribute__((visibility("default"))) void plugin_attach(const char* (*next_place_work)(const char*)) {
    g_context->next_place_work = next_place_work;
}

__attribute__((visibility("default"))) const char* plugin_wait_finished(void) {
    if (!g_context) return  "Plugin context not initialized";
    consumer_producer_signal_finished(g_context->queue);
    if (g_context->consumer_thread) {
        int err = pthread_join(g_context->consumer_thread, NULL);
        if (err != 0) {
            return "Could not join consumer thread";
        }
        g_context->consumer_thread = 0;
    }
    return NULL;
}