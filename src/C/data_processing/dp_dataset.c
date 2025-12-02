#include "dp_dataset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <ctype.h>
#include <json-c/json.h>

#define INITIAL_CAPACITY 16

DP_DataSet *dp_dataset_new(const char *name) {
    DP_DataSet *ds = malloc(sizeof(DP_DataSet));
    ds->name = strdup(name);
    ds->count = 0;
    ds->capacity = INITIAL_CAPACITY;
    ds->values = malloc(sizeof(double) * ds->capacity);
    return ds;
}

void dp_dataset_add(DP_DataSet *ds, double value) {
    if (ds->count >= ds->capacity) {
        ds->capacity *= 2;
        ds->values = realloc(ds->values, sizeof(double) * ds->capacity);
    }
    ds->values[ds->count++] = value;
}

size_t dp_dataset_count(DP_DataSet *ds) {
    return ds->count;
}

void dp_dataset_free(DP_DataSet *ds) {
    free(ds->values);
    free(ds->name);
    free(ds);
}

DP_DataSet *dp_dataset_load_csv(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return NULL;

    DP_DataSet *ds = dp_dataset_new(filename);
    char line[256];

    // Skip header
    fgets(line, sizeof(line), f);

    while (fgets(line, sizeof(line), f)) {
        double value = atof(line);
        dp_dataset_add(ds, value);
    }

    fclose(f);
    return ds;
}

DP_DataSet *dp_dataset_load_json(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *data = malloc(len + 1);
    fread(data, 1, len, f);
    data[len] = '\0';
    fclose(f);

    struct json_object *root = json_tokener_parse(data);
    free(data);

    struct json_object *values;
    if (!json_object_object_get_ex(root, "values", &values)) {
        json_object_put(root);
        return NULL;
    }

    DP_DataSet *ds = dp_dataset_new(filename);
    for (size_t i = 0; i < json_object_array_length(values); i++) {
        double val = json_object_get_double(json_object_array_get_idx(values, i));
        dp_dataset_add(ds, val);
    }

    json_object_put(root);
    return ds;
}

DP_DataSet *dp_dataset_load_sqlite(const char *db_path, const char *table_name) {
    sqlite3 *db;
    if (sqlite3_open(db_path, &db)) return NULL;

    char query[256];
    snprintf(query, sizeof(query), "SELECT value FROM %s;", table_name);

    DP_DataSet *ds = dp_dataset_new(table_name);

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        sqlite3_close(db);
        dp_dataset_free(ds);
        return NULL;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        double val = sqlite3_column_double(stmt, 0);
        dp_dataset_add(ds, val);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return ds;
}
