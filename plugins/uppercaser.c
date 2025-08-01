#include "plugin_sdk.h"
#include "plugin_common.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

const char* plugin_transform(const char* input) {
    if (!input) {
        return NULL;
    }
    
    // Allocate memory for the output string
    char* output = malloc(strlen(input) + 1);
    if (!output) {
        return NULL;
    }
    
    // Convert each character to uppercase
    for (int i = 0; input[i] != '\0'; i++) {
        output[i] = toupper(input[i]);
    }
    output[strlen(input)] = '\0';
    
    return output;
}

const char* plugin_init(int queue_size) {
    return common_plugin_init(plugin_transform, "uppercaser", queue_size);
}

const char* get_plugin_name(void) {
    return "uppercaser";
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