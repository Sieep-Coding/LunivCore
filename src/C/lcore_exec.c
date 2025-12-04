/* lcore_exec.c - Enhanced LCore execution with chart rendering */

#include "lcore_exec.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/* Project headers */
#include "core/dataset.h"
#include "lcore/lexer.h"
#include "lcore/parser.h"
#include "lcore/ast.h"
#include "lcore/render.h"

/* Forward declaration - defined in render.h */
void render_chart(ASTNode *chart_node, ASTNode *data_node);

/* ========== Helper: Convert DataSet to AST ========== */
/**
 * Convert a DataSet from registry into an AST NODE_DATASET
 * This creates temporary AST nodes for chart rendering
 */
static ASTNode *dataset_to_ast(const DataSet *ds, const char *name) {
    if (!ds) return NULL;

    ASTNode *data = ast_new(NODE_DATASET, name, ds->title, 0);

    /* Iterate through dataset rows using the known structure */
    for (size_t i = 0; i < ds->count; i++) {
        ASTNode *row = ast_new(NODE_ROW, 
                              ds->labels[i],    /* label from labels array */
                              NULL,
                              ds->values[i]);   /* value from values array */
        ast_add_child(data, row);
    }

    return data;
}

/* ========== Execution Engine ========== */

void lcore_exec_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        perror("File open failed");
        return;
    }

    /* Read entire file into buffer */
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *src = malloc(len + 1);
    if (!src) {
        fclose(f);
        perror("Memory allocation failed");
        return;
    }

    fread(src, 1, len, f);
    src[len] = '\0';
    fclose(f);

    /* Parse source */
    Lexer lexer;
    lexer_init(&lexer, src);

    Parser parser;
    parser_init(&parser, &lexer);

    ASTNode *root = parser_parse(&parser);
    if (!root) {
        fprintf(stderr, "Parse error.\n");
        free(src);
        return;
    }

    /* Execute and render all nodes */
    for (size_t i = 0; i < root->child_count; i++) {
        ASTNode *child = root->children[i];

        switch (child->type) {
            /* ========== Datasets ========== */
            case NODE_DATASET:
                render_dataset(child);
                break;

            /* ========== Views ========== */
            case NODE_VIEW:
                render_view(child);
                break;

            /* ========== Text Nodes ========== */
            case NODE_TEXT:
                render_text(child);
                break;

            /* ========== Charts ========== */
            case NODE_PLOT: {
                /* Plot nodes have:
                   - name: dataset identifier (e.g., "sales")
                   - value: chart type (e.g., "bar", "pie", "line")
                */
                
                if (!child->name || !child->value) {
                    fprintf(stderr, "Warning: Plot node missing name or type\n");
                    break;
                }

                /* Look up dataset in registry by name */
                DataSet *ds = dataset_registry_get(child->name);
                if (!ds) {
                    fprintf(stderr, "Warning: Dataset '%s' not found for chart\n",
                           child->name);
                    break;
                }

                /* Convert DataSet to AST nodes */
                ASTNode *data = dataset_to_ast(ds, child->name);
                if (data) {
                    /* Render the chart - chart type is in child->value */
                    render_chart(child, data);
                    ast_free(data);
                } else {
                    fprintf(stderr, "Warning: Failed to create AST from dataset '%s'\n",
                           child->name);
                }
                break;
            }

            /* ========== Aggregation Functions ========== */
            case NODE_FUNCTION_CALL: {
                /* Function calls have:
                   - name: function name (e.g., "sum", "avg", "min", "max", "count")
                   - value: dataset name
                */
                
                DataSet *ds = dataset_registry_get(child->value);
                if (!ds) {
                    fprintf(stderr, "Error: Unknown dataset '%s'\n", child->value);
                    break;
                }

                if (strcmp(child->name, "sum") == 0) {
                    printf("Sum(%s) = %ld\n", child->value, dataset_sum(ds));
                } 
                else if (strcmp(child->name, "avg") == 0) {
                    printf("Avg(%s) = %.2f\n", child->value, dataset_avg(ds));
                } 
                else if (strcmp(child->name, "min") == 0) {
                    printf("Min(%s) = %d\n", child->value, dataset_min(ds));
                } 
                else if (strcmp(child->name, "max") == 0) {
                    printf("Max(%s) = %d\n", child->value, dataset_max(ds));
                } 
                else if (strcmp(child->name, "count") == 0) {
                    printf("Count(%s) = %zu\n", child->value, dataset_count(ds));
                } 
                else {
                    fprintf(stderr, "Unknown function: %s\n", child->name);
                }
                break;
            }

            /* ========== Other Node Types ========== */
            case NODE_HEADER:
                /* Headers from comment lines */
                if (child->value) {
                    printf("# %s\n", child->value);
                }
                break;

            /* These are typically child nodes, not top-level */
            case NODE_ROW:
            case NODE_FILTER:
            case NODE_AGGREGATE:
            case NODE_SORT:
            case NODE_JOIN:
            case NODE_COMPUTED_COL:
            case NODE_DOCUMENT:
                break;

            default:
                fprintf(stderr, "Warning: Unhandled node type %d\n", child->type);
                break;
        }
    }

    /* Cleanup */
    ast_free(root);
    free(src);
}