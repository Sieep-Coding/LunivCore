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

    const int TOTAL_WIDTH = 48;
    const int PADDING = 1;
    const int SEPARATOR = 3;

    // Use the title from AST node, fallback only if NULL/empty
    const char *title = (view->value && view->value[0] != '\0') ? view->value : "DASHBOARD";

    // Compute max lengths and max numeric value for rows
    int max_label_len = 0, max_value_len = 0, max_value = 0;
    for (size_t i = 0; i < view->child_count; i++) {
        ASTNode *child = view->children[i];
        if (!child) continue;
        if (child->type == NODE_ROW) {
            int label_len = strlen(child->name ? child->name : "");
            int value_len = snprintf(NULL, 0, "%d", child->numeric_value);
            if (label_len > max_label_len) max_label_len = label_len;
            if (value_len > max_value_len) max_value_len = value_len;
            if (child->numeric_value > max_value) max_value = child->numeric_value;
        }
    }

    int content_width = TOTAL_WIDTH - 2 * PADDING - 2;
    int bar_width = content_width - max_label_len - SEPARATOR - max_value_len - 2;
    if (bar_width < 8) bar_width = 8;

    // Top border
    printf("╔");
    for (int i = 0; i < TOTAL_WIDTH; i++) printf("═");
    printf("╗\n");

    // Centered title
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

    // Iterate children
    for (size_t i = 0; i < view->child_count; i++) {
        ASTNode *child = view->children[i];
        if (!child) continue;

        switch (child->type) {
            case NODE_ROW: {
                int bar_len = max_value > 0
                    ? (int)((double)child->numeric_value / max_value * bar_width + 0.5)
                    : 0;
                if (bar_len > bar_width) bar_len = bar_width;

                printf("║ %-*s : %*d │", max_label_len, child->name ? child->name : "", max_value_len, child->numeric_value);
                for (int j = 0; j < bar_len; j++) fputs("█", stdout);
                for (int j = bar_len; j < bar_width; j++) fputs("░", stdout);
                printf(" │ ║\n");
                break;
            }

            case NODE_TEXT: {
                // Render text centered with padding
                const char *text = child->value ? child->value : "";
                int text_len = strlen(text);
                int pad_left = (TOTAL_WIDTH - text_len) / 2;
                int pad_right = TOTAL_WIDTH - text_len - pad_left;
                printf("║");
                for (int j = 0; j < pad_left; j++) printf(" ");
                printf("%s", text);
                for (int j = 0; j < pad_right; j++) printf(" ");
                printf("║\n");
                break;
            }

            default:
                break;
        }
    }

    // Bottom border
    printf("╚");
    for (int i = 0; i < TOTAL_WIDTH; i++) printf("═");
    printf("╝\n");
}

void render_text(ASTNode *node) {
    if (!node || node->type != NODE_TEXT) return;

    const int TOTAL_WIDTH = 48;
    const char *text = node->value ? node->value : "";
    int text_len = strlen(text);

    int pad_left = (TOTAL_WIDTH - text_len) / 2;
    int pad_right = TOTAL_WIDTH - text_len - pad_left;

    // Top border
    printf("╔");
    for (int i = 0; i < TOTAL_WIDTH; i++) printf("═");
    printf("╗\n");

    // Centered text
    printf("║");
    for (int i = 0; i < pad_left; i++) printf(" ");
    printf("%s", text);
    for (int i = 0; i < pad_right; i++) printf(" ");
    printf("║\n");

    // Bottom border
    printf("╚");
    for (int i = 0; i < TOTAL_WIDTH; i++) printf("═");
    printf("╝\n");
}
