#include "lcore_exec.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

/* Project headers */
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
    }

    ast_free(root);
    free(src);
}