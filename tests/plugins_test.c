#include <stdio.h>
#include "../plugins/plugin_common.h"
#include "../plugins/sync/consumer_producer.h"
#include "../plugins/sync/monitor.h"
#include <dlfcn.h>

int test_plugin(const char* plugin_name) {
    char* plugin_path = malloc(strlen("./output/") + strlen(plugin_name) + 1);
    strcpy(plugin_path, "./output/");
    strcat(plugin_path, plugin_name);
    strcat(plugin_path, ".so");
    void* handle = dlopen(plugin_path, RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        return 1;
    }
    const char* (*plugin_init)(int) = dlsym(handle, "plugin_init");
    if (!plugin_init) {
        fprintf(stderr, "dlsym failed in %s: %s\n", plugin_name, dlerror());
        return 1;
    }
    plugin_init(10);
    const char* (*get_plugin_name)(void) = dlsym(handle, "get_plugin_name");
    if (!get_plugin_name) {
        fprintf(stderr, "dlsym failed in %s: %s\n", plugin_name, dlerror());
        return 1;
    }
    const char* (*transform)(const char*) = dlsym(handle, "plugin_transform");
    if (!transform) {
        fprintf(stderr, "dlsym failed in %s: %s\n", plugin_name, dlerror());
        return 1;
    }
    puts(get_plugin_name());
    puts("Plugin loaded OK");

    char* inputs[5] = {"hello world!", "Hello World!", "HeLLo WoRld!", "1a2b3c", ""};
    for (int i = 0; i < 5; i++) {
        const char* result = transform(inputs[i]);
        printf("Input: %s\n", inputs[i]);
        printf("Result: %s\n", result);
    }
    dlclose(handle);
    return 0;
}

int main() {
    // test_plugin("logger");
    // test_plugin("uppercaser");
    // test_plugin("rotator");
    // test_plugin("flipper");
    test_plugin("typewriter");
    return 0;
}