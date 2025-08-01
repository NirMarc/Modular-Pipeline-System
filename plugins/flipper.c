
#include "plugin_common.h"
#include <stdio.h>
#include <string.h>
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
        output[i] = input[strlen(input) - i - 1];
    }
    output[strlen(input)] = '\0';
    
    return output;
}

const char* plugin_init(int queue_size) {
    return common_plugin_init(plugin_transform, "flipper", queue_size);
}

const char* get_plugin_name(void) {
    return "flipper";
}

