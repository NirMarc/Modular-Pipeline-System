#ifndef PLUGIN_COMMON_H
#define PLUGIN_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <pthread.h>
#include "sync/consumer_producer.h"

typedef struct {
    const char* name; // plugin name
    consumer_producer_t* queue; // Input queue
    pthread_t consumer_thread; // Consumer thread
    const char* (*next_place_work)(const char*); // Next plugin's place_work function
    const char* (*process_function)(const char*); // Plugin-specific process function
    int initialized; // Initialized flag
    int finished; // Finished processing flag
} plugin_context_t;

/**
 * Generic consumer thread function
 * @param arg Pointer to plugin_context_t
 * @return NULL
 */
void* plugin_consumer_thread(void* arg);

/**
 * Print error message in format [ERROR][Plugin Name] - message
 * @param context Plugin context
 * @param message Error message
 */
void log_error(plugin_context_t* context, const char* message);

/**
 * Print info message in format [INFO][Plugin Name] - message
 * @param context Plugin context
 * @param message Info message
 */
void log_info(plugin_context_t* context, const char* message);

/**
 * Initialize the common plugin infrastructure with the specified queue size
 * @param process_function Plugin-specific process function
 * @param name Plugin name
 * @param queue_size Maximum number of items that can be queued
 * @return NULL on success, error message on failure
 */
const char* common_plugin_init(const char* (*process_function)(const char*), const char* name, int queue_size);

/**
 * Get the plugin's name
 * @return The plugin's name (should be modified or freed)
 */
__attribute__((visibility("default"))) const char* get_plugin_name(void);

/**
 * Initialize the plugin with the speficied queue size - calls common_plugin_init
 * This function should be implemented by each plugin
 * @param queue_size Maximum number of items that can be queued
 * @return NULL on success, error message on failure
 */
__attribute__((visibility("default"))) const char* plugin_init(int queue_size);

/**
 * Finalize the plugin - drain queue and terminate thread gracfully (i.e pthread_join)
 * @return NULL on success, error message on failure
 */
__attribute__((visibility("default"))) const char* plugin_fini(void);

/**
 * Place work (a string) into the plugin's queue
 * @param str The string to process (plugin takes ownership if it allocates new memory)
 * @return NULL on success, error message on failure
 */
__attribute__((visibility("default"))) const char* plugin_place_work(const char* str);

/**
 * Attach this plugin to the next plugin in the chain
 * @param next_place_work Function pointer to the next plugin's place_work_function
 */
__attribute__((visibility("default"))) void plugin_attach(const char* (*next_place_work)(const char*));

/**
 * Wait untill the plugin has finished processing all work and is ready to shutdown
 * This is a blocking function used for graceful shutdown coordination
 * @return NULL on success, error message on failure
 */
__attribute__((visibility("default"))) const char* plugin_wait_finished(void);


#endif