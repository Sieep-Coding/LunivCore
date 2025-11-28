#include "dataset.h"
#include <stdio.h>
#include <string.h>

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
