#include <stdio.h>
#include <string.h>

/* Refactored component headers */
#include "lua_bindings/lbind.h"
#include "lcore_exec.h"

/* ---------------------------- */
/* File Helpers                 */
/* ---------------------------- */

static const char *get_extension(const char *path) {
    const char *dot = strrchr(path, '.');
    return dot ? dot + 1 : NULL;
}

static int is_lua(const char *ext) {
    return ext && strcmp(ext, "lua") == 0;
}

static int is_lcore(const char *ext) {
    return ext && strcmp(ext, "lcore") == 0;
}

/* ---------------------------- */
/* Main Entry                   */
/* ---------------------------- */

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <file.lua | file.lcore>\n", argv[0]);
        return 1;
    }

    const char *path = argv[1];
    const char *ext = get_extension(path);

    if (is_lua(ext)) {
        lbind_run_file(path);
    }
    else if (is_lcore(ext)) {
        lcore_exec_file(path);
    }
    else {
        fprintf(stderr, "Unsupported file type: %s\n",
                ext ? ext : "(none)");
        return 1;
    }

    return 0;
}