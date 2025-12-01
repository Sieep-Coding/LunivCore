#include "ast.h"
#include <stdio.h>

void render_dataset(ASTNode *dataset) {
    printf("--- %s ---\n", dataset->value);
    for (size_t i = 0; i < dataset->child_count; i++) {
        ASTNode *row = dataset->children[i];
        printf("%-10s | ", row->name);
        for (int j = 0; j < row->numeric_value / 10; j++)
            fputs("█", stdout);
        printf(" (%d)\n", row->numeric_value);
    }
    printf("----------------\n");
}

void render_view(ASTNode *view) {
    // For now, views are rendered identically to datasets
    printf("--- View: %s ---\n", view->value);
    for (size_t i = 0; i < view->child_count; i++) {
        ASTNode *row = view->children[i];
        printf("%-10s | ", row->name);
        for (int j = 0; j < row->numeric_value / 10; j++)
            fputs("█", stdout);
        printf(" (%d)\n", row->numeric_value);
    }
    printf("----------------\n");
}
