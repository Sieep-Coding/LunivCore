#include <stdio.h>
#include <string.h>

/* Refactored component headers */
#include "lua_bindings/lbind.h"
#include "data_processing/dp_dataset.h"
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

    DP_DataSet *csv_ds = dp_dataset_load_csv("src/C/data/sample.csv");
    DP_DataSet *json_ds = dp_dataset_load_json("src/C/data/sample.json");
    DP_DataSet *sqlite_ds = dp_dataset_load_sqlite("src/C/data/test.db", "test_table");

    if (csv_ds) {
        printf("CSV dataset count: %zu\n", dp_dataset_count(csv_ds));
        dp_dataset_free(csv_ds);
    } else {
        printf("Failed to load CSV dataset\n");
    }

    if (json_ds) {
        printf("JSON dataset count: %zu\n", dp_dataset_count(json_ds));
        dp_dataset_free(json_ds);
    } else {
        printf("Failed to load JSON dataset\n");
    }

    if (sqlite_ds) {
        printf("SQLite dataset count: %zu\n", dp_dataset_count(sqlite_ds));
        dp_dataset_free(sqlite_ds);
    } else {
        printf("Failed to load SQLite dataset\n");
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