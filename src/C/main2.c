#include "data_processing/dp_dataset.h"
#include <stdio.h>

int main() {
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

    return 0;
}
