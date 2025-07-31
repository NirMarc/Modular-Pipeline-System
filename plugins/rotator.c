#include "plugin_sdk.h"
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
    int len = strlen(input);
    // Rotate the string by 1 character to the right
    for (int i = 0; i < len; i++) {
        output[(i + 1) % len] = input[i];
    }
    output[len] = '\0';
    
    return output;
}

const char* plugin_init(int queue_size) {
    return common_plugin_init(plugin_transform, "rotator", queue_size);
}

const char* get_plugin_name(void) {
    return "rotator";
}