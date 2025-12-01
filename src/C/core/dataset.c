#include "dataset.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Global instance definition */
DataSetRegistry BI_Registry = { .count = 0 };

/* --------------------------------- */
/* Dataset Core Implementation       */
/* --------------------------------- */

void dataset_init(DataSet *ds, const char *title) {
    if (!ds) return;

    strncpy(ds->title, title, DATASET_MAX_TITLE - 1);
    ds->title[DATASET_MAX_TITLE - 1] = '\0';

    ds->count = 0;
}

int dataset_add(DataSet *ds, const char *label, int value) {
    if (!ds || !label) return -1;

    if (ds->count >= DATASET_MAX_ROWS) {
        return -1; /* Full */
    }

    strncpy(ds->labels[ds->count], label, DATASET_MAX_LABEL - 1);
    ds->labels[ds->count][DATASET_MAX_LABEL - 1] = '\0';

    ds->values[ds->count] = value;
    ds->count++;

    return 0;
}

void dataset_plot(const DataSet *ds) {
    if (!ds) return;

    printf("\n--- %s ---\n", ds->title);

    for (size_t i = 0; i < ds->count; i++) {
        printf("%-10s | ", ds->labels[i]);

        int bar_len = ds->values[i] / 10;
        for (int b = 0; b < bar_len; b++) {
            printf("█");
        }

        printf(" (%d)\n", ds->values[i]);
    }

    printf("----------------\n");
}


/* --------------------------------- */
/* Registry Implementation           */
/* --------------------------------- */

int dataset_registry_add(const char *name, DataSet *ds) {
    if (BI_Registry.count >= DATASET_MAX_DATASETS) {
        fprintf(stderr, "Registry error: Max dataset limit reached.\n");
        return -1;
    }

    // Check if name already exists
    for (size_t i = 0; i < BI_Registry.count; i++) {
        if (strcmp(BI_Registry.names[i], name) == 0) {
            fprintf(stderr, "Registry error: Dataset '%s' already exists.\n", name);
            return -1;
        }
    }

    strncpy(BI_Registry.names[BI_Registry.count], name, DATASET_MAX_NAME - 1);
    BI_Registry.names[BI_Registry.count][DATASET_MAX_NAME - 1] = '\0';
    
    BI_Registry.datasets[BI_Registry.count] = ds;
    BI_Registry.count++;

    return 0;
}

DataSet *dataset_registry_get(const char *name) {
    for (size_t i = 0; i < BI_Registry.count; i++) {
        if (strcmp(BI_Registry.names[i], name) == 0) {
            return BI_Registry.datasets[i];
        }
    }
    return NULL;
}

void dataset_registry_free() {
    for (size_t i = 0; i < BI_Registry.count; i++) {
        // Free the DataSet object that was dynamically allocated in main.c
        free(BI_Registry.datasets[i]);
        BI_Registry.datasets[i] = NULL;
    }
    BI_Registry.count = 0;
    fprintf(stderr, "BI_Registry: Cleaned up and freed all datasets.\n");
}