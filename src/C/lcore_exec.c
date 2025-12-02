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

/* --- Implementation --- */

void lcore_exec_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        perror("File open failed");
        return;
    }

    /* Read the entire file content into a buffer */
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

    /* Parse and Render */
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

    for (size_t i = 0; i < root->child_count; i++) {
        ASTNode *child = root->children[i];
        if (child->type == NODE_DATASET)
            render_dataset(child);
        else if (child->type == NODE_VIEW)
            render_view(child);
        else if (child->type == NODE_FUNCTION_CALL) {
            DataSet *ds = dataset_registry_get(child->value);
            if (!ds) {
                fprintf(stderr, "Unknown dataset: %s\n", child->value);
                continue;
            }

            if (strcmp(child->name, "sum") == 0)
                printf("Sum(%s) = %ld\n", child->value, dataset_sum(ds));
            else if (strcmp(child->name, "avg") == 0)
                printf("Avg(%s) = %.2f\n", child->value, dataset_avg(ds));
            else if (strcmp(child->name, "min") == 0)
                printf("Min(%s) = %d\n", child->value, dataset_min(ds));
            else if (strcmp(child->name, "max") == 0)
                printf("Max(%s) = %d\n", child->value, dataset_max(ds));
            else if (strcmp(child->name, "count") == 0)
                printf("Count(%s) = %zu\n", child->value, dataset_count(ds));
        }
    }

    ast_free(root);
    free(src);
}