#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <dlfcn.h>
#include "../plugins/plugin_common.h"
#include <assert.h>

#define MAX_PIPELINE_LENGTH 10

char* use_functions(void** functions);
char* test_pipeline(const char* pipeline);

char* test_pipeline(const char* pipeline) {
    char* input = strdup(pipeline);
    char* token = strtok(input, " ");

    void* handles[MAX_PIPELINE_LENGTH] = {0};
    void* functions[MAX_PIPELINE_LENGTH] = {0};
    int i = 0;

    while (token != NULL && i < MAX_PIPELINE_LENGTH) {
        char path[256];
        snprintf(path, sizeof(path), "output/%s.so", token);
        handles[i] = dlopen(path, RTLD_NOW | RTLD_LAZY);
        if (!handles[i]) {
            printf("Failed to load plugin: %s\n", dlerror());
            exit(1);
        }
        const char* (*transform)(const char*) = dlsym(handles[i], "plugin_transform");
        functions[i] = (void*)transform;
        if (!functions[i]) {
            printf("Failed to load function: %s\n", dlerror());
        }
        token = strtok(NULL, " ");
        i++;
    }

    char* result = use_functions(functions);
    printf("Final string: %s\n", result);
    for (int i = 0; i < MAX_PIPELINE_LENGTH && handles[i] != 0; i++) {
        dlclose(handles[i]);
    }
    return result;
}

char* use_functions(void** functions) {
    int i = 0;
    const char* str = "A sentence to be Tested 123!!!";
    while (functions[i] != 0) {
        str = ((const char* (*)(const char*))functions[i])(str);
        i++;
    }
    return strdup(str);
}

int main() {
    const char* output = test_pipeline("logger uppercaser rotator");
    assert(strcmp(output, "!A SENTENCE TO BE TESTED 123!!") == 0);
    output = test_pipeline("flipper typewriter");
    assert(strcmp(output, "!!!321 detseT eb ot ecnetnes A") == 0);
    output = test_pipeline("flipper rotator logger uppercaser rotator typewriter flipper rotator uppercaser");
    assert(strcmp(output, " SENTENCE TO BE TESTED 123!!!A") == 0);
    printf("All tests passed\n");
    return 0;
}