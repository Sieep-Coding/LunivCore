#include "ast.h"
#include <stdlib.h>
#include <string.h>

ASTNode *ast_new(NodeType type, const char *name, const char *value, int numeric_value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = type;
    node->name = name ? strdup(name) : NULL;
    node->value = value ? strdup(value) : NULL;
    node->numeric_value = numeric_value;
    node->children = NULL;
    node->child_count = 0;
    return node;
}

void ast_add_child(ASTNode *parent, ASTNode *child) {
    parent->children = (ASTNode **)realloc(parent->children, sizeof(ASTNode *) * (parent->child_count + 1));
    parent->children[parent->child_count] = child;
    parent->child_count++;
}

void ast_free(ASTNode *node) {
    if (!node) return;

    if (node->name) free((void *)node->name);
    if (node->value) free((void *)node->value);

    for (size_t i = 0; i < node->child_count; i++) {
        ast_free(node->children[i]);
    }

    free(node->children);
    free(node);
}
