/* render.c - Clean, minimal dashboard rendering with web-component aesthetic */
#include "render.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h> 

/* Responsive Constants */
#define CONSOLE_WIDTH 80
#define CHART_HEIGHT 12
#define BAR_MAX_LEN 40
#define TABLE_COL_WIDTH 25

/* Minimal color palette - accent only */
#define RESET       "\x1b[0m"
#define BOLD        "\x1b[1m"
#define DIM         "\x1b[2m"
#define ACCENT      "\x1b[36m"
#define TEXT        "\x1b[37m"
#define POSITIVE    "\x1b[32m"
#define NEGATIVE    "\x1b[31m"

/* Unicode blocks for precision */
#define BLOCK_FULL  "█"
#define BLOCK_HALF  "▌"
#define BLOCK_QTR   "▎"
#define BLOCK_EMPTY "░"

/* Symbols */
#define DOT         "⚪"
#define ARROW_UP    "⬆️"
#define ARROW_DOWN  "⬇️"

typedef enum {
    CHART_BAR,
    CHART_LINE,
    CHART_PIE,
    CHART_TABLE,
    CHART_HISTOGRAM,
    CHART_SCATTER,
    CHART_KPI
} ChartType;

static ChartType get_chart_type(const char *type_str) {
    if (!type_str) return CHART_BAR;
    if (strcmp(type_str, "line") == 0) return CHART_LINE;
    if (strcmp(type_str, "pie") == 0) return CHART_PIE;
    if (strcmp(type_str, "table") == 0) return CHART_TABLE;
    if (strcmp(type_str, "histogram") == 0) return CHART_HISTOGRAM;
    if (strcmp(type_str, "scatter") == 0) return CHART_SCATTER;
    if (strcmp(type_str, "kpi") == 0) return CHART_KPI;
    return CHART_BAR;
}

/* ========== UTILITY FUNCTIONS ========== */

static void print_frame_header(const char *title, const char *subtitle) {
    printf("\n" BOLD ACCENT "┌─ %s" DIM " %s" RESET "\n", title, subtitle ? subtitle : "");
}

static void print_frame_footer() {
    printf(ACCENT "└");
    for (int i = 0; i < CONSOLE_WIDTH - 2; i++) printf("─");
    printf("┘" RESET "\n");
}

/* Removed print_separator to resolve unused function warning */
/*
static void print_separator() {
    printf(ACCENT "├");
    for (int i = 0; i < CONSOLE_WIDTH - 2; i++) printf("─");
    printf("┤" RESET "\n");
}
*/

static int get_max_value(ASTNode *chart_data) {
    int max_value = 0;
    if (!chart_data) return 0;
    for (size_t i = 0; i < chart_data->child_count; i++) {
        if (chart_data->children[i]->numeric_value > max_value)
            max_value = chart_data->children[i]->numeric_value;
    }
    return max_value;
}

/* ========== BAR CHART (Horizontal) ========== */
static void render_bar_chart(ASTNode *chart_data) {
    if (!chart_data || chart_data->child_count == 0) {
        printf("  " DIM "(empty data set)" RESET "\n");
        return;
    }

    int max_value = get_max_value(chart_data);

    if (max_value == 0) {
        printf("  " DIM "(no data)" RESET "\n");
        return;
    }

    printf(DIM "  Value Distribution\n" RESET);

    for (size_t i = 0; i < chart_data->child_count; i++) {
        ASTNode *row = chart_data->children[i];
        int val = row->numeric_value;
        int bar_len = (int)((double)val / max_value * BAR_MAX_LEN);
        
        printf("  %-*.*s │", (int)TABLE_COL_WIDTH, (int)TABLE_COL_WIDTH, row->name);
        
        printf(ACCENT);
        for (int j = 0; j < bar_len; j++) {
            printf(BLOCK_FULL);
        }
        printf(RESET);
        
        printf(DIM " %8d\n" RESET, val);
    }
}

/* ========== LINE CHART (Sparkline/Time-series) ========== */
static void render_line_chart(ASTNode *chart_data) {
    if (!chart_data || chart_data->child_count < 2) {
        printf("  " DIM "(needs 2+ data points for a line chart)" RESET "\n");
        return;
    }

    int max_value = get_max_value(chart_data);
    if (max_value == 0) return;

    const int plot_width = CONSOLE_WIDTH - 8;
    
    // Scale factor to map data to CHART_HEIGHT (y-axis)
    double scale_y = (double)CHART_HEIGHT / max_value;
    
    // 2D Array to store plot characters
    char plot_grid[CHART_HEIGHT][CONSOLE_WIDTH + 1];

    // Initialize grid with spaces
    for (int y = 0; y < CHART_HEIGHT; y++) {
        memset(plot_grid[y], ' ', CONSOLE_WIDTH);
        plot_grid[y][CONSOLE_WIDTH] = '\0';
    }

    // Pass 1: Plot the points and draw vertical lines
    for (size_t x = 0; x < chart_data->child_count; x++) {
        int val = chart_data->children[x]->numeric_value;
        int current_y = (int)(val * scale_y); // 0 to CHART_HEIGHT - 1

        // Position on the X-axis
        int plot_x = 7 + (int)((double)x / (chart_data->child_count - 1) * (plot_width - 1));
        
        if (plot_x >= CONSOLE_WIDTH) continue;
        
        if (current_y < CHART_HEIGHT) {
             // Draw point (must be drawn last to overwrite vertical lines)
             plot_grid[CHART_HEIGHT - 1 - current_y][plot_x] = DOT[0];
        }

        // Draw vertical lines downwards from the point's position to y=0
        if (current_y > 0) {
            for (int y_line = 0; y_line < current_y; y_line++) {
                if (plot_grid[CHART_HEIGHT - 1 - y_line][plot_x] != DOT[0]) {
                    // Change from '│' to '|'
                    plot_grid[CHART_HEIGHT - 1 - y_line][plot_x] = '|'; 
                }
            }
        }
    }
    
    // Pass 2: Print the grid with Y-axis labels and colors
    for (int y = 0; y < CHART_HEIGHT; y++) {
        int value_at_y = (int)((double)(CHART_HEIGHT - y) / CHART_HEIGHT * max_value);
        
        // Y-axis label
        if (y == 0 || y == CHART_HEIGHT - 1 || y == CHART_HEIGHT / 2) {
            printf("%5d │", value_at_y);
        } else {
            printf("      │");
        }
        
        // Print the plot area
        for (int x = 7; x < CONSOLE_WIDTH; x++) {
            char c = plot_grid[y][x];
            if (c == DOT[0]) {
                printf(ACCENT DOT RESET);
            } else if (c == '|') { 
                printf(DIM "|" RESET);
            } else {
                printf("%c", c);
            }
        }
        printf("\n");
    }

    // X-axis and labels
    printf("      └");
    for (int i = 0; i < plot_width; i++) printf("─");
    printf("\n");

    printf("       ");
    int step = chart_data->child_count / 5; 
    if (step == 0) step = 1;
    for (size_t i = 0; i < chart_data->child_count; i += step) {
        // Corrected format specifiers to cast size_t result to int
        int width = (int)((double)plot_width / (chart_data->child_count / step));
        printf(" %-*.*s", width, width, chart_data->children[i]->name);
    }
    printf("\n");
}

/* ========== PIE CHART (Donut/Ring ASCII Representation) ========== */
static void render_pie_chart(ASTNode *chart_data) {
    if (!chart_data || chart_data->child_count == 0) {
        printf("  " DIM "(empty data set)" RESET "\n");
        return;
    }

    int total = 0;
    for (size_t i = 0; i < chart_data->child_count; i++) {
        total += chart_data->children[i]->numeric_value;
    }

    if (total == 0) return;

    printf(DIM "  Share of Total (n=%d)\n" RESET, total);

    for (size_t i = 0; i < chart_data->child_count; i++) {
        ASTNode *row = chart_data->children[i];
        double percent = (double)row->numeric_value / total * 100.0;
        int bar_width = (int)(percent / 4);
        if (bar_width > 25) bar_width = 25;

        printf("  ");
        
        printf(ACCENT);
        for (int j = 0; j < bar_width; j++) printf(BLOCK_FULL);
        printf(RESET);
        
        printf(" %5.1f%% │ %-*.*s\n", percent, 
            (int)(CONSOLE_WIDTH - 15 - bar_width), 
            (int)(CONSOLE_WIDTH - 15 - bar_width),
            row->name);
    }
}

/* ========== TABLE CHART (Clean Grid) ========== */
static void render_table_chart(ASTNode *chart_data) {
    if (!chart_data || chart_data->child_count == 0) {
        printf("  " DIM "(empty data set)" RESET "\n");
        return;
    }

    const int value_col_width = 15;
    // Removed unused 'total_cols' variable
    const int label_col_width = CONSOLE_WIDTH - 6 - value_col_width;

    printf(ACCENT "  ┌");
    for (int i = 0; i < label_col_width; i++) printf("─");
    printf("┬");
    for (int i = 0; i < value_col_width; i++) printf("─");
    printf("┐\n");
    
    printf("  │ " BOLD "Metric" RESET ACCENT " %*s│ " BOLD "Value" RESET ACCENT "%*s│\n", 
           label_col_width - 8, "", value_col_width - 7, "");
    
    printf("  ├");
    for (int i = 0; i < label_col_width; i++) printf("─");
    printf("┼");
    for (int i = 0; i < value_col_width; i++) printf("─");
    printf("┤\n" RESET);

    for (size_t i = 0; i < chart_data->child_count; i++) {
        ASTNode *row = chart_data->children[i];
        printf("  │ %-*.*s │ %10d │\n", 
               label_col_width - 2, 
               label_col_width - 2, 
               row->name, 
               row->numeric_value);
    }

    printf(ACCENT "  └");
    for (int i = 0; i < label_col_width; i++) printf("─");
    printf("┴");
    for (int i = 0; i < value_col_width; i++) printf("─");
    printf("┘\n" RESET);
}

/* ========== HISTOGRAM (Vertical Bar Distribution) ========== */
static void render_histogram(ASTNode *chart_data) {
    if (!chart_data || chart_data->child_count == 0) {
        printf("  " DIM "(empty data set)" RESET "\n");
        return;
    }

    int max_value = get_max_value(chart_data);
    if (max_value == 0) return;

    int max_height = CHART_HEIGHT;
    
    printf(DIM "  Distribution by Bucket\n" RESET);

    for (int h = max_height - 1; h >= 0; h--) {
        printf("  %4d │ ", (int)((double)(h + 1) / max_height * max_value));
        
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

    // X-axis
    printf("       └");
    for (size_t i = 0; i < chart_data->child_count; i++) {
        printf("─");
    }
    printf("\n");

    // Labels - one char per column
    printf("         ");
    for (size_t i = 0; i < chart_data->child_count; i++) {
        ASTNode *row = chart_data->children[i];
        printf("%.*s", 1, row->name);
    }
    printf("\n");
}

/* ========== SCATTER PLOT (Simple Point Grid) ========== */
static void render_scatter_plot(ASTNode *chart_data) {
    if (!chart_data || chart_data->child_count < 2) {
        printf("  " DIM "(needs 2+ points)" RESET "\n");
        return;
    }

    int max_value = get_max_value(chart_data);
    if (max_value == 0) return;

    int height = CHART_HEIGHT;
    const int plot_width = CONSOLE_WIDTH - 8;

    printf(DIM "  Point Distribution\n" RESET);

    for (int y = height - 1; y >= 0; y--) {
        int value_at_y = (int)((double)(y + 1) / height * max_value);
        printf("%5d │", value_at_y);

        for (size_t x = 0; x < chart_data->child_count; x++) {
            int val = chart_data->children[x]->numeric_value;
            int point_y = (int)((double)val / max_value * height);
            
            // Removed unused 'plot_x' variable
            
            if (point_y == y) {
                printf(ACCENT DOT RESET);
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }

    // Axes
    printf("      └");
    // Corrected sign comparison warning by casting to int
    for (int i = 0; i < (int)chart_data->child_count; i++) printf("─");
    printf("\n");

    // X-axis labels (one char per point)
    printf("       ");
    for (size_t i = 0; i < chart_data->child_count; i++) {
        printf("%c", chart_data->children[i]->name[0]);
    }
    printf("\n");

    // Values row
    printf("       ");
    for (size_t i = 0; i < chart_data->child_count; i++) {
        char val_str[16];
        snprintf(val_str, sizeof(val_str), "%d", chart_data->children[i]->numeric_value);
        printf("%c", val_str[0]);
    }
    printf("\n");
}

/* ========== KPI Component (Key Performance Indicator) ========== */
static void render_kpi(ASTNode *chart_data) {
    if (!chart_data || chart_data->child_count == 0) {
        printf("  " DIM "(empty data set)" RESET "\n");
        return;
    }

    ASTNode *main_kpi = chart_data->children[0];
    int main_value = main_kpi->numeric_value;
    const char *label = main_kpi->name ? main_kpi->name : "Metric";

    printf("  " DIM "Value of %s\n" RESET, label);
    printf("  " BOLD ACCENT "%.*s" RESET, (int)TABLE_COL_WIDTH, label);
    printf(BOLD " %d\n" RESET, main_value);

    if (chart_data->child_count > 1) {
        ASTNode *comp_kpi = chart_data->children[1];
        int comp_value = comp_kpi->numeric_value;
        int diff = main_value - comp_value;
        double percent_change = (comp_value != 0) ? ((double)diff / comp_value * 100.0) : 0.0;

        const char *color = (diff >= 0) ? POSITIVE : NEGATIVE;
        const char *arrow = (diff >= 0) ? ARROW_UP : ARROW_DOWN;

        printf("  " DIM "vs. %s: " RESET, comp_kpi->name ? comp_kpi->name : "Previous");
        printf("%s%s %d (%+.1f%%)" RESET "\n", color, arrow, abs(diff), percent_change);
    }
}

/* ========== MAIN CHART RENDERER ========== */
void render_chart(ASTNode *chart_node, ASTNode *data_node) {
    if (!chart_node || !data_node) {
        printf("  " DIM "(invalid chart definition)" RESET "\n");
        return;
    }

    ChartType chart_type = get_chart_type(chart_node->value);
    const char *title = chart_node->name ? chart_node->name : "Component";
    
    print_frame_header(title, chart_node->value);

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
        case CHART_KPI:
            render_kpi(data_node);
            break;
        default:
            printf("  " DIM "(unknown chart type: %s)" RESET "\n", chart_node->value);
    }
    
    print_frame_footer();
}

/* ========== LEGACY FUNCTIONS - Simplified into Frame + Data List ========== */

void render_dataset(ASTNode *dataset) {
    if (!dataset) return;
    
    print_frame_header(dataset->value ? dataset->value : "Dataset", "Raw Data");
    
    int max_value = get_max_value(dataset);

    for (size_t i = 0; i < dataset->child_count; i++) {
        ASTNode *row = dataset->children[i];
        int val = row->numeric_value;
        int bar_len = max_value > 0 ? (int)((double)val / max_value * 35) : 0;
        
        printf("  %-16s │", row->name);
        for (int j = 0; j < bar_len; j++) printf(ACCENT BLOCK_FULL RESET);
        printf(DIM " %8d\n" RESET, val);
    }
    
    print_frame_footer();
}

void render_view(ASTNode *view) {
    if (!view) return;

    const char *title = (view->value && view->value[0] != '\0') ? view->value : "Dashboard View";
    
    print_frame_header(title, "Container");

    for (size_t i = 0; i < view->child_count; i++) {
        ASTNode *child = view->children[i];
        if (!child) continue;

        if (child->type == NODE_ROW) {
            printf("  " DIM "%-25s: " RESET BOLD "%d\n" RESET, 
                   child->name ? child->name : "(anon)", 
                   child->numeric_value);
        } 
        else if (child->type == NODE_TEXT) {
            const char *text = child->value ? child->value : "";
            int text_len = strlen(text);
            int pad = (CONSOLE_WIDTH - text_len) / 2;
            
            printf("  ");
            for (int j = 0; j < pad - 2; j++) printf(" ");
            printf(BOLD "%s" RESET "\n", text);
        }
    }

    print_frame_footer();
}

void render_text(ASTNode *node) {
    if (!node || node->type != NODE_TEXT) return;

    const char *text = node->value ? node->value : "Note";

    print_frame_header("Note", "");
    printf("  " DIM "%s" RESET "\n", text);
    print_frame_footer();
}