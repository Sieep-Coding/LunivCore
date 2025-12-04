/* render.c - Clean, minimal dashboard rendering with web-component aesthetic */
#include "render.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

#define CHART_WIDTH 120
#define CHART_HEIGHT 14
#define BAR_WIDTH 42

/* Minimal color palette - accent only */
#define RESET       "\x1b[0m"
#define BOLD        "\x1b[1m"
#define DIM         "\x1b[2m"
#define ACCENT      "\x1b[36m"    /* Cyan accent */
#define TEXT        "\x1b[37m"    /* White text */

/* Unicode blocks for precision */
#define BLOCK_FULL  "█"
#define BLOCK_HALF  "▌"
#define BLOCK_QTR   "▎"

typedef enum {
    CHART_BAR,
    CHART_LINE,
    CHART_PIE,
    CHART_TABLE,
    CHART_HISTOGRAM,
    CHART_SCATTER
} ChartType;

static ChartType get_chart_type(const char *type_str) {
    if (!type_str) return CHART_BAR;
    if (strcmp(type_str, "line") == 0) return CHART_LINE;
    if (strcmp(type_str, "pie") == 0) return CHART_PIE;
    if (strcmp(type_str, "table") == 0) return CHART_TABLE;
    if (strcmp(type_str, "histogram") == 0) return CHART_HISTOGRAM;
    if (strcmp(type_str, "scatter") == 0) return CHART_SCATTER;
    return CHART_BAR;
}

/* ========== UTILITY FUNCTIONS ========== */

static void print_header(const char *title) {
    printf("\n" BOLD ACCENT "┌─ %s" RESET "\n", title);
}

static void print_footer() {
    printf(ACCENT "└─────────────────────────────────────────────────────────────────────\n" RESET);
}

/* Print a clean row separator */
static void print_separator() {
    printf(ACCENT "├─────────────────────────────────────────────────────────────────────\n" RESET);
}

/* ========== BAR CHART ========== */
static void render_bar_chart(ASTNode *chart_data) {
    if (!chart_data || chart_data->child_count == 0) {
        printf("  (empty)\n");
        return;
    }

    int max_value = 0;
    for (size_t i = 0; i < chart_data->child_count; i++) {
        if (chart_data->children[i]->numeric_value > max_value)
            max_value = chart_data->children[i]->numeric_value;
    }

    if (max_value == 0) {
        printf("  (no data)\n");
        return;
    }

    for (size_t i = 0; i < chart_data->child_count; i++) {
        ASTNode *row = chart_data->children[i];
        int val = row->numeric_value;
        int bar_len = (int)((double)val / max_value * BAR_WIDTH);
        
        printf("  %-16s │", row->name);
        
        for (int j = 0; j < bar_len; j++) {
            printf(ACCENT BLOCK_FULL RESET);
        }
        
        printf(" %8d\n", val);
    }
}

/* ========== LINE CHART ========== */
static void render_line_chart(ASTNode *chart_data) {
    if (!chart_data || chart_data->child_count == 0) {
        printf("  (empty)\n");
        return;
    }

    int max_value = 0;
    for (size_t i = 0; i < chart_data->child_count; i++) {
        if (chart_data->children[i]->numeric_value > max_value)
            max_value = chart_data->children[i]->numeric_value;
    }

    if (max_value == 0) return;

    /* Calculate scale for Y-axis */
    int scale = (max_value + 4) / 5;  /* Round to nearest 5 */
    if (scale < 1) scale = 1;

    /* Draw chart area */
    for (int y = CHART_HEIGHT - 1; y >= 0; y--) {
        int value_at_y = y * scale;
        
        /* Y-axis label */
        printf("%5d │", value_at_y);

        /* Plot area */
        for (size_t x = 0; x < chart_data->child_count && x < CHART_WIDTH - 8; x++) {
            int val = chart_data->children[x]->numeric_value;
            int val_y = (val + scale / 2) / scale;  /* Round to nearest scale */
            
            if (val_y == y) {
                printf(ACCENT "●" RESET);
            } else if (val_y > y) {
                /* Draw vertical line from point down */
                if (x == 0 || (int)((chart_data->children[x-1]->numeric_value + scale/2) / scale) < y) {
                    printf("│");
                } else {
                    printf("·");
                }
            } else {
                printf("·");
            }
        }
        printf("\n");
    }

    /* X-axis */
    printf("      └");
    for (size_t i = 0; i < chart_data->child_count && i < CHART_WIDTH - 8; i++) {
        printf("─");
    }
    printf("\n");

    /* X-axis labels */
    printf("       ");
    for (size_t i = 0; i < chart_data->child_count && i < 6; i++) {
        printf(" %.*s", 2, chart_data->children[i]->name);
    }
    printf("\n");
}

/* ========== PIE CHART - ASCII Ring Chart ========== */
static void render_pie_chart(ASTNode *chart_data) {
    if (!chart_data || chart_data->child_count == 0) {
        printf("  (empty)\n");
        return;
    }

    int total = 0;
    for (size_t i = 0; i < chart_data->child_count; i++) {
        total += chart_data->children[i]->numeric_value;
    }

    if (total == 0) return;

    /* Render as a clean list with proportional bar indicators */
    printf("\n");
    
    int cumulative = 0;
    for (size_t i = 0; i < chart_data->child_count; i++) {
        ASTNode *row = chart_data->children[i];
        double percent = (double)row->numeric_value / total * 100.0;
        int bar_width = (int)(percent / 2);  /* 0-50 width for 0-100% */
        if (bar_width > 25) bar_width = 25;

        printf("  ");
        
        /* Percentage indicator bar */
        printf(ACCENT);
        for (int j = 0; j < bar_width; j++) printf("▯");
        printf(RESET);
        
        /* Right-aligned label and percentage */
        printf(" %5.1f%% │ %s\n", percent, row->name);
    }
}

/* ========== TABLE CHART ========== */
static void render_table_chart(ASTNode *chart_data) {
    if (!chart_data || chart_data->child_count == 0) {
        printf("  (empty)\n");
        return;
    }

    printf("\n");
    printf(ACCENT "  ┌────────────────────────┬────────────┐\n");
    printf("  │ " BOLD "Label" RESET ACCENT "                │ " BOLD "Value" RESET ACCENT "      │\n");
    printf("  ├────────────────────────┼────────────┤\n" RESET);

    for (size_t i = 0; i < chart_data->child_count; i++) {
        ASTNode *row = chart_data->children[i];
        printf("  │ %-22s │ %10d │\n", row->name, row->numeric_value);
    }

    printf(ACCENT "  └────────────────────────┴────────────┘\n" RESET);
}

/* ========== HISTOGRAM - Stacked Block Distribution ========== */
static void render_histogram(ASTNode *chart_data) {
    if (!chart_data || chart_data->child_count == 0) {
        printf("  (empty)\n");
        return;
    }

    int max_value = 0;
    for (size_t i = 0; i < chart_data->child_count; i++) {
        if (chart_data->children[i]->numeric_value > max_value)
            max_value = chart_data->children[i]->numeric_value;
    }

    if (max_value == 0) return;

    /* Render as vertical bars - use full width */
    int max_height = 12;
    
    for (int h = max_height - 1; h >= 0; h--) {
        printf("  %4d │ ", (int)((double)h / max_height * max_value));
        
        for (size_t i = 0; i < chart_data->child_count; i++) {
            ASTNode *row = chart_data->children[i];
            int bar_height = (int)((double)row->numeric_value / max_value * max_height);
            
            if (bar_height > h) {
                printf(ACCENT BLOCK_FULL RESET);
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }

    /* X-axis */
    printf("       └─");
    for (size_t i = 0; i < chart_data->child_count; i++) {
        printf("─");
    }
    printf("\n");

    /* Labels - one char per column */
    printf("         ");
    for (size_t i = 0; i < chart_data->child_count; i++) {
        ASTNode *row = chart_data->children[i];
        printf("%.*s", 1, row->name);
    }
    printf("\n");

    /* Values below labels */
    printf("         ");
    size_t col = 0;
    for (size_t i = 0; i < chart_data->child_count; i++) {
        ASTNode *row = chart_data->children[i];
        char val_str[8];
        snprintf(val_str, sizeof(val_str), "%d", row->numeric_value);
        
        /* Print value, one char at a time */
        if (col < strlen(val_str)) {
            printf("%c", val_str[col]);
            col++;
        } else {
            printf(" ");
        }
    }
    printf("\n");
}

/* ========== SCATTER PLOT - Grid-based Points ========== */
static void render_scatter_plot(ASTNode *chart_data) {
    if (!chart_data || chart_data->child_count < 2) {
        printf("  (needs 2+ points)\n");
        return;
    }

    int max_value = 0;
    for (size_t i = 0; i < chart_data->child_count; i++) {
        if (chart_data->children[i]->numeric_value > max_value)
            max_value = chart_data->children[i]->numeric_value;
    }

    if (max_value == 0) return;

    int height = 12;

    /* Draw grid with full width */
    for (int y = height - 1; y >= 0; y--) {
        int value_at_y = (int)((double)y / height * max_value);
        printf("%5d │", value_at_y);

        for (size_t x = 0; x < chart_data->child_count; x++) {
            int val = chart_data->children[x]->numeric_value;
            int point_y = (int)((double)val / max_value * height);
            
            if (point_y == y) {
                printf(ACCENT "●" RESET);
            } else if (y == 0) {
                printf("─");
            } else {
                printf(" ");
            }
        }
        printf("│\n");
    }

    /* Axes */
    printf("      └");
    for (size_t i = 0; i < chart_data->child_count; i++) printf("─");
    printf("┘\n");

    /* X-axis labels (one char per point) */
    printf("       ");
    for (size_t i = 0; i < chart_data->child_count; i++) {
        printf("%c", chart_data->children[i]->name[0]);
    }
    printf("\n");

    /* Values row */
    printf("       ");
    for (size_t i = 0; i < chart_data->child_count; i++) {
        char val_str[16];
        snprintf(val_str, sizeof(val_str), "%d", chart_data->children[i]->numeric_value);
        printf("%c", val_str[0]);  /* First digit of value */
    }
    printf("\n");
}

/* ========== MAIN CHART RENDERER ========== */
void render_chart(ASTNode *chart_node, ASTNode *data_node) {
    if (!chart_node || !data_node) {
        printf("  (invalid chart)\n");
        return;
    }

    ChartType chart_type = get_chart_type(chart_node->value);
    const char *title = chart_node->name ? chart_node->name : "Chart";
    
    print_header(title);

    switch (chart_type) {
        case CHART_BAR:
            render_bar_chart(data_node);
            break;
        case CHART_LINE:
            render_line_chart(data_node);
            break;
        case CHART_PIE:
            render_pie_chart(data_node);
            break;
        case CHART_TABLE:
            render_table_chart(data_node);
            break;
        case CHART_HISTOGRAM:
            render_histogram(data_node);
            break;
        case CHART_SCATTER:
            render_scatter_plot(data_node);
            break;
        default:
            printf("  (unknown type)\n");
    }
    
    print_footer();
}

/* ========== LEGACY FUNCTIONS ========== */

void render_dataset(ASTNode *dataset) {
    if (!dataset) return;
    
    print_header(dataset->value ? dataset->value : "Dataset");
    
    int max_value = 0;
    for (size_t i = 0; i < dataset->child_count; i++) {
        if (dataset->children[i]->numeric_value > max_value)
            max_value = dataset->children[i]->numeric_value;
    }

    for (size_t i = 0; i < dataset->child_count; i++) {
        ASTNode *row = dataset->children[i];
        int val = row->numeric_value;
        int bar_len = max_value > 0 ? (int)((double)val / max_value * 35) : 0;
        
        printf("  %-14s │", row->name);
        for (int j = 0; j < bar_len; j++) printf(ACCENT BLOCK_FULL RESET);
        printf(" %8d\n", val);
    }
    
    print_footer();
}

void render_view(ASTNode *view) {
    if (!view) return;

    const int WIDTH = 74;
    const char *title = (view->value && view->value[0] != '\0') ? view->value : "Dashboard";

    int max_label_len = 0, max_value = 0;
    for (size_t i = 0; i < view->child_count; i++) {
        ASTNode *child = view->children[i];
        if (!child || child->type != NODE_ROW) continue;
        int label_len = strlen(child->name ? child->name : "");
        if (label_len > max_label_len) max_label_len = label_len;
        if (child->numeric_value > max_value) max_value = child->numeric_value;
    }

    if (max_label_len < 12) max_label_len = 12;

    /* Header */
    printf("\n" BOLD ACCENT "┌─ %s\n" RESET, title);
    
    printf(ACCENT "├");
    for (int i = 0; i < WIDTH - 2; i++) printf("─");
    printf("┤\n" RESET);

    /* Rows */
    for (size_t i = 0; i < view->child_count; i++) {
        ASTNode *child = view->children[i];
        if (!child) continue;

        if (child->type == NODE_ROW) {
            int bar_len = max_value > 0
                ? (int)((double)child->numeric_value / max_value * 35)
                : 0;

            printf("  %-*s │ ", max_label_len, child->name ? child->name : "");
            
            for (int j = 0; j < bar_len; j++) {
                printf(ACCENT BLOCK_FULL RESET);
            }
            
            printf(" %8d\n", child->numeric_value);
        } 
        else if (child->type == NODE_TEXT) {
            const char *text = child->value ? child->value : "";
            int text_len = strlen(text);
            int pad = (WIDTH - 4 - text_len) / 2;
            
            printf("  ");
            for (int j = 0; j < pad; j++) printf(" ");
            printf(BOLD "%s" RESET "\n", text);
        }
    }

    /* Footer */
    printf(ACCENT "└");
    for (int i = 0; i < WIDTH - 2; i++) printf("─");
    printf("┘\n" RESET "\n");
}

void render_text(ASTNode *node) {
    if (!node || node->type != NODE_TEXT) return;

    const int WIDTH = 74;
    const char *text = node->value ? node->value : "";
    int text_len = strlen(text);

    printf("\n" BOLD ACCENT "┌─ %s\n" RESET, text);
    printf(ACCENT "└");
    for (int i = 0; i < WIDTH - 2; i++) printf("─");
    printf("┘\n" RESET "\n");
}