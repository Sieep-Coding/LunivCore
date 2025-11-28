#ifndef DATASET_H
#define DATASET_H

#include <stddef.h>

#define DATASET_MAX_ROWS 100
#define DATASET_MAX_TITLE 64
#define DATASET_MAX_LABEL 32

typedef struct {
    char title[DATASET_MAX_TITLE];
    char labels[DATASET_MAX_ROWS][DATASET_MAX_LABEL];
    int values[DATASET_MAX_ROWS];
    size_t count;
} DataSet;


/* Create a new dataset with a given title */
void dataset_init(DataSet *ds, const char *title);

/* Add a single row (label, value) */
int dataset_add(DataSet *ds, const char *label, int value);

/* ASCII visualization */
void dataset_plot(const DataSet *ds);

#endif
