#pragma once
#include <stddef.h>

typedef struct {
    char *name;
    double *values;
    size_t count;
    size_t capacity;
} DP_DataSet;

// Core functions
DP_DataSet *dp_dataset_new(const char *name);
void dp_dataset_add(DP_DataSet *ds, double value);
size_t dp_dataset_count(DP_DataSet *ds);
void dp_dataset_free(DP_DataSet *ds);

// Data source loaders
DP_DataSet *dp_dataset_load_csv(const char *filename);
DP_DataSet *dp_dataset_load_json(const char *filename);
DP_DataSet *dp_dataset_load_sqlite(const char *db_path, const char *table_name);
