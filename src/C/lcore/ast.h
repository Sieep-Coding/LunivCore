#ifndef AST_H
#define AST_H

#include <stddef.h>

typedef enum {
    NODE_DOCUMENT,
    NODE_HEADER,
    NODE_DATASET,
    NODE_ROW,
    NODE_PLOT,
    NODE_EXPORT
} NodeType;

typedef struct ASTNode {
    NodeType type;
    const char *name;
    const char *value;
    int numeric_value;
    struct ASTNode **children;
    size_t child_count;
} ASTNode;

ASTNode *ast_new(NodeType type, const char *name, const char *value, int numeric_value);
void ast_add_child(ASTNode *parent, ASTNode *child);
void ast_free(ASTNode *node);

#endif
