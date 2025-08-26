#define _GNU_SOURCE
#include "plugins/plugin_common.h"
#include "plugins/sync/consumer_producer.h"
#include "plugins/sync/monitor.h"
#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <link.h>

#define MAX_LINE 1024

int g_queue_size = 0;
int g_num_plugins = 0;

typedef const char* (*init_fn)(int);
typedef const char* (*place_work_fn)(const char*);
typedef void (*attach_fn)(const char* (*next_place_work)(const char*));
typedef const char* (*wait_finished_fn)(void);
typedef const char* (*fini_fn)(void);

typedef struct {
    char* name;
    void* handle;
    init_fn init;
    place_work_fn place_work;
    attach_fn attach;
    wait_finished_fn wait_finished;
    fini_fn fini;
} plugin_handle_t;

plugin_handle_t* g_plugin_handles = NULL;

static void build_plugin_path(char* path, size_t path_size, const char* plugin_name) {
    snprintf(path, path_size, "./output/%s.so", plugin_name);
}

void print_help() {
    printf("Usage: ./analyzer <queue_size> <plugin1> <plugin2> ... <pluginN>\n");
    printf("\n");
    printf("Arguments:\n");
    printf("  queue_size   Maximum number of items in each plugin's queue\n");
    printf("  plugin1..N   Name of plugins to load (without .so extension)\n");
    printf("\n");
    printf("Available plugins:\n");
    printf("  logger        - Logs all strings that pass through\n");
    printf("  typewriter    - Simulates typewriter effect with delays\n");
    printf("  uppercaser    - Converts strings to uppercase\n");
    printf("  rotator       - Move every character to the right.  Last character moves to the beginning.\n");
    printf("  flipper       - Reverses the order of the characters\n");
    printf("  expander      - Expands each character with spaces\n");
    printf("\n");
    printf("Example:\n");
    printf("  ./analyzer 20 uppercaser rotator logger\n");
    printf("  echo 'hello' | ./analyzer 20 uppercaser rotator logger\n");
    printf("  echo <END> | ./analyzer 20 uppercaser rotator logger\n");
    fflush(stdout);
}

static void init_plugins(char** argv) {
    for (int i = 0; i < g_num_plugins; i++) {
        g_plugin_handles[i].name = argv[i + 2];
        
        char path[256];
        build_plugin_path(path, sizeof(path), g_plugin_handles[i].name);
        g_plugin_handles[i].handle = dlmopen(LM_ID_NEWLM, path, RTLD_NOW | RTLD_LOCAL);

        if (!g_plugin_handles[i].handle) {
            fprintf(stderr, "%s\n", dlerror());
            for (int j = 0; j < i; j++) {
                dlclose(g_plugin_handles[j].handle);
            }
            free(g_plugin_handles);
            print_help();
            exit(1);
        }
        dlerror();
        g_plugin_handles[i].init = (init_fn)dlsym(g_plugin_handles[i].handle, "plugin_init");
        g_plugin_handles[i].place_work = (place_work_fn)dlsym(g_plugin_handles[i].handle, "plugin_place_work");
        g_plugin_handles[i].attach = (attach_fn)dlsym(g_plugin_handles[i].handle, "plugin_attach");
        g_plugin_handles[i].wait_finished = (wait_finished_fn)dlsym(g_plugin_handles[i].handle, "plugin_wait_finished");
        g_plugin_handles[i].fini = (fini_fn)dlsym(g_plugin_handles[i].handle, "plugin_fini");

        const char *sym_error = dlerror();
        if (sym_error || !g_plugin_handles[i].init || !g_plugin_handles[i].place_work ||
            !g_plugin_handles[i].attach || !g_plugin_handles[i].wait_finished || !g_plugin_handles[i].fini) {
            fprintf(stderr, "dlsym error in %s: %s\n", path, sym_error ? sym_error : "missing symbol(s)");
            for (int j = 0; j <= i; j++) dlclose(g_plugin_handles[j].handle);
            free(g_plugin_handles);
            print_help();
            exit(1);
        }

        const char* init_error = g_plugin_handles[i].init(g_queue_size);
        if (init_error) {
            fprintf(stderr, "Failed to initialize plugin %s: %s\n", g_plugin_handles[i].name, init_error);
            for (int j = 0; j <= i; j++) dlclose(g_plugin_handles[j].handle);
            free(g_plugin_handles);
            print_help();
            exit(1);
        }
    }
}

static void attach_plugins(void) {
    for (int i = 0; i < g_num_plugins - 1; i++) {
        g_plugin_handles[i].attach(g_plugin_handles[i + 1].place_work);
    }
}

static int read_input(void) {
    char line[MAX_LINE];
    while (1) {
        fgets(line, sizeof(line), stdin);
        if (feof(stdin)) {
            continue;
        }
        if (strcmp(line, "<END>\n") == 0) break;
        line[strcspn(line, "\n")] = '\0';
        const char* err = g_plugin_handles[0].place_work(line);
        if (err) {
            fprintf(stderr, "Failed to place work in plugin %s: %s\n", g_plugin_handles[0].name, err);
            for (int j = 0; j < g_num_plugins; j++) dlclose(g_plugin_handles[j].handle);
            free(g_plugin_handles);
            print_help();
            return 1;
        }
    }
    return 0;
}

static void shutdown_pipeline(void) {
    for (int i = 0; i < g_num_plugins; i++) {
        if (g_plugin_handles[i].wait_finished) {
            const char* wait_finished_err = g_plugin_handles[i].wait_finished();
            if (wait_finished_err) {
                fprintf(stderr, "Failed to wait for plugin %s to finish: %s\n", g_plugin_handles[i].name, wait_finished_err);
            }
        }
        if (g_plugin_handles[i].fini) {
            const char* fini_err = g_plugin_handles[i].fini();
            if (fini_err) {
                fprintf(stderr, "Failed to finalize plugin %s: %s\n", g_plugin_handles[i].name, fini_err);
            }
        }
    }
    for (int i = g_num_plugins - 1; i >= 0; i--) {
        dlclose(g_plugin_handles[i].handle);
    }
    free(g_plugin_handles);
    printf("Pipeline shutdown complete\n");
}
int main(int argc, char** argv) {
    if (argc < 3) {
        print_help();
        return 1;
    }
    g_queue_size = atoi(argv[1]);
    if (g_queue_size <= 0) {
        fprintf(stderr, "Queue size must be greater than 0\n");
        print_help();
        return 1;
    }
    g_num_plugins = argc - 2;
    if (g_num_plugins <= 0) {
        fprintf(stderr, "At least one plugin is required\n");
        print_help();
        return 1;
    }
    g_plugin_handles = (plugin_handle_t*)malloc(g_num_plugins * sizeof(plugin_handle_t));
    if (!g_plugin_handles) {
        fprintf(stderr, "Failed to allocate memory for plugin handles\n");
        print_help();
        return 1;
    }

    init_plugins(argv);
    attach_plugins();
    if (read_input() != 0) {
        shutdown_pipeline();
        return 1;
    }
    shutdown_pipeline();
    return 0;
}