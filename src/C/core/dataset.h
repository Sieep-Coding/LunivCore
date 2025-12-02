#ifndef DATASET_H
#define DATASET_H

#include <stddef.h>

#define DATASET_MAX_ROWS 100
#define DATASET_MAX_TITLE 64
#define DATASET_MAX_LABEL 32

// Constants for the global registry
#define DATASET_MAX_DATASETS 50
#define DATASET_MAX_NAME 32 // The identifier name used in LCore, e.g., 'Sales'

// Full definition of the DataSet structure
typedef struct {
    char title[DATASET_MAX_TITLE];
    char labels[DATASET_MAX_ROWS][DATASET_MAX_LABEL];
    int values[DATASET_MAX_ROWS];
    size_t count;
} DataSet;


// Global Registry Structure
typedef struct {
    DataSet *datasets[DATASET_MAX_DATASETS];
    char names[DATASET_MAX_DATASETS][DATASET_MAX_NAME]; // LCore identifier names
    size_t count;
} DataSetRegistry;

extern DataSetRegistry BI_Registry; // Global instance declaration

/* --------------------------------- */
/* Dataset Core Functions            */
/* --------------------------------- */

/* Create a new dataset with a given title */
void dataset_init(DataSet *ds, const char *title);

/* Add a single row (label, value) */
int dataset_add(DataSet *ds, const char *label, int value);

/* ASCII visualization */
void dataset_plot(const DataSet *ds);


/* --------------------------------- */
/* Registry Functions                */
/* --------------------------------- */

/* Add a dataset to the global registry */
int dataset_registry_add(const char *name, DataSet *ds);

/* Retrieve a dataset from the global registry */
DataSet *dataset_registry_get(const char *name);

/* Free all dynamically allocated datasets in the registry */
void dataset_registry_free();

/* Aggregation helpers */
long dataset_sum(const DataSet *ds);
double dataset_avg(const DataSet *ds);
int dataset_min(const DataSet *ds);
int dataset_max(const DataSet *ds);
size_t dataset_count(const DataSet *ds);


#endif