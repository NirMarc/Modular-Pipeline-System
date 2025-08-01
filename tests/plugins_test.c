#include <stdio.h>
#include "../plugins/plugin_common.h"
#include "../plugins/sync/consumer_producer.h"
#include "../plugins/sync/monitor.h"
#include <dlfcn.h>

void test_plugin(const char* str) {
    const char* plugins[] = {"logger", "typewriter", "rotator", "flipper", "uppercaser"};
    for (int i = 0; i < 5; i++) {
        char* plugin_path = malloc(strlen("./output/") + strlen(plugins[i]) + 1);
        strcpy(plugin_path, "./output/");
        strcat(plugin_path, plugins[i]);
        strcat(plugin_path, ".so");
        void* handle = dlopen(plugin_path, RTLD_NOW | RTLD_LOCAL);
    
        if (!handle) {
            fprintf(stderr, "dlopen failed: %s\n", dlerror());
            return;
        }
        const char* (*plugin_init)(int) = dlsym(handle, "plugin_init");
        if (!plugin_init) {
            fprintf(stderr, "dlsym failed in %s: %s\n", plugins[i], dlerror());
            return;
        }
        plugin_init(10);
        const char* (*get_plugin_name)(void) = dlsym(handle, "get_plugin_name");
        if (!get_plugin_name) {
            fprintf(stderr, "dlsym failed in %s: %s\n", plugins[i], dlerror());
            return;
        }
        const char* (*transform)(const char*) = dlsym(handle, "plugin_transform");
        if (!transform) {
            fprintf(stderr, "dlsym failed in %s: %s\n", plugins[i], dlerror());
            return;
        }
        printf("Plugin: %s: ", get_plugin_name());
        transform(str);
        if (i > 1) printf("%s\n", transform(str));
        free(plugin_path);
        dlclose(handle);
    }
    return;
}

int main() {
    const char* strs[] = {"Hello World!", "1a2b3c", "1234567890", "abCde", ""};
    for (int i = 0; i < 5; i++) {
        printf("Input: %s\n", strs[i]);
        test_plugin(strs[i]);
    }
    return 0;
}