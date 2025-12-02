#include "ast.h"
#include <string.h>
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
    if (!view) return;

    // Calculate dimensions
    const int TOTAL_WIDTH = 48;
    const int PADDING = 1;
    const int SEPARATOR = 3;  // for " : " and divider
    
    int max_label_len = 0;
    int max_value_len = 0;
    int max_value = 0;

    // First pass: compute layout metrics
    for (size_t i = 0; i < view->child_count; i++) {
        ASTNode *row = view->children[i];
        if (!row || row->type != NODE_ROW) continue;
        
        int label_len = strlen(row->name ? row->name : "");
        int value_len = snprintf(NULL, 0, "%d", row->numeric_value);
        
        if (label_len > max_label_len) max_label_len = label_len;
        if (value_len > max_value_len) max_value_len = value_len;
        if (row->numeric_value > max_value) max_value = row->numeric_value;
    }

    int content_width = TOTAL_WIDTH - 2 * PADDING - 2;  // Account for borders
    int bar_width = content_width - max_label_len - SEPARATOR - max_value_len - 2;
    if (bar_width < 8) bar_width = 8;

    // Top border
    printf("╔");
    for (int i = 0; i < TOTAL_WIDTH; i++) printf("═");
    printf("╗\n");

    // Title (centered)
    const char *title = view->value ? view->value : "DASHBOARD";
    int title_len = strlen(title);
    int pad_left = (TOTAL_WIDTH - title_len) / 2;
    int pad_right = TOTAL_WIDTH - title_len - pad_left;
    
    printf("║");
    for (int i = 0; i < pad_left; i++) printf(" ");
    printf("%s", title);
    for (int i = 0; i < pad_right; i++) printf(" ");
    printf("║\n");

    // Header separator
    printf("╠");
    for (int i = 0; i < TOTAL_WIDTH; i++) printf("═");
    printf("╣\n");

    // Data rows
    for (size_t i = 0; i < view->child_count; i++) {
        ASTNode *row = view->children[i];
        if (!row || row->type != NODE_ROW) continue;

        int bar_len = max_value > 0 
            ? (int)((double)row->numeric_value / max_value * bar_width + 0.5)
            : 0;
        if (bar_len > bar_width) bar_len = bar_width;

        // Build row: label : value │▓▓▓│
        printf("║ ");
        printf("%-*s : ", max_label_len, row->name ? row->name : "");
        printf("%*d ", max_value_len, row->numeric_value);
        printf("│");
        
        for (int j = 0; j < bar_len; j++) fputs("█", stdout);
        for (int j = bar_len; j < bar_width; j++) fputs("░", stdout);
        
        printf(" │ ║\n");
    }

    // Bottom border
    printf("╚");
    for (int i = 0; i < TOTAL_WIDTH; i++) printf("═");
    printf("╝\n");
}