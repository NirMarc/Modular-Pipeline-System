#include "plugin_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* plugin_transform(const char* input) {
    if (!input) {
        return NULL;
    }
    
    char* output = malloc(strlen(input) * 2 + 1);
    if (!output) {
        return "Could not allocate memory for output";
    }
    for (int i = 0; i < strlen(input); i++) {
        output[i * 2] = input[i];
        output[i * 2 + 1] = ' ';
    }
    output[strlen(input) * 2 - 1] = '\0';
    return output;
}

const char* plugin_init(int queue_size) {
    return common_plugin_init(plugin_transform, "expander", queue_size);
}

const char* get_plugin_name(void) {
    return "expander";
}

