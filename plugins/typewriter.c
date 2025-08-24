
#include "plugin_common.h"
#include <stdio.h>
#include <unistd.h>

const char* plugin_transform(const char* input) {
    if (!input) {
        return NULL;
    }
    
    printf("[typewriter] ");
    for (int i = 0; input[i] != '\0'; i++) {
        printf("%c", input[i]);
        fflush(stdout);
        usleep(100000);
    }
    printf("\n");
    fflush(stdout);
    return input;
}

const char* plugin_init(int queue_size) {
    return common_plugin_init(plugin_transform, "typewriter", queue_size);
}

const char* get_plugin_name(void) {
    return "typewriter";
}
