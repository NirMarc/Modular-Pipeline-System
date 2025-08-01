#include "plugin_sdk.h"
#include "plugin_common.h"
#include <stdio.h>

const char* plugin_transform(const char* input) {
    if (!input) {
        return NULL;
    }
    
    printf("Logger: %s\n", input);
    return input;
}

const char* plugin_init(int queue_size) {
    return common_plugin_init(plugin_transform, "logger", queue_size);
}

const char* get_plugin_name(void) {
    return "logger";
}

const char* plugin_fini(void) {
    return plugin_fini();
}

const char* plugin_place_work(const char* str) {
    return plugin_place_work(str);
}

void plugin_attach(const char* (*next_place_work)(const char*)) {
    return plugin_attach(next_place_work);
}

const char* plugin_wait_finished(void) {
    return plugin_wait_finished();
}