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
    const char *name;       // dataset name, plot type, export format, etc.
    const char *value;      // header text, row label, filename, etc.
    int numeric_value;      // for numbers in dataset rows
    struct ASTNode **children; // dynamic array of child nodes
    size_t child_count;
} ASTNode;

/* Create a new AST node */
ASTNode *ast_new(NodeType type, const char *name, const char *value, int numeric_value);

/* Add a child node */
void ast_add_child(ASTNode *parent, ASTNode *child);

/* Free the AST */
void ast_free(ASTNode *node);

#endif
