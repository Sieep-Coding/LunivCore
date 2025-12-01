#ifndef AST_H
#define AST_H

#include <stddef.h>
#include <time.h>

/* Data type for type-safe operations (duplicated to avoid circular dependency) */
typedef enum {
    DATA_TYPE_INT,
    DATA_TYPE_FLOAT,
    DATA_TYPE_STRING,
    DATA_TYPE_DATE,
    DATA_TYPE_BOOL,
} DataType;

/* Extended node types for enterprise features */
typedef enum {
    NODE_DOCUMENT,
    NODE_HEADER,
    NODE_DATASET,
    NODE_ROW,
    NODE_PLOT,
    NODE_EXPORT,
    NODE_VIEW,
    NODE_FILTER,        /* New: filter operations */
    NODE_AGGREGATE,     /* New: aggregation (SUM, AVG, etc) */
    NODE_SORT,          /* New: sorting operations */
    NODE_JOIN,          /* New: dataset joins */
    NODE_COMPUTED_COL,  /* New: computed/derived columns */
    NODE_FUNCTION_CALL, /* New: function calls */
} NodeType;

/* Aggregation function types */
typedef enum {
    AGG_SUM,
    AGG_AVG,
    AGG_MIN,
    AGG_MAX,
    AGG_COUNT,
    AGG_STDDEV,
    AGG_MEDIAN,
} AggregationType;

/* Comparison operators for filtering */
typedef enum {
    OP_EQ,      /* == */
    OP_NE,      /* != */
    OP_LT,      /* < */
    OP_LE,      /* <= */
    OP_GT,      /* > */
    OP_GE,      /* >= */
    OP_IN,      /* IN */
    OP_NOT_IN,  /* NOT IN */
} ComparisonOp;

/* Sort direction */
typedef enum {
    SORT_ASC,
    SORT_DESC,
} SortDirection;

/* AST Node structure with extended metadata */
typedef struct ASTNode {
    NodeType type;
    const char *name;           /* Identifier name */
    const char *value;          /* String value */
    int numeric_value;          /* Integer value */
    double float_value;         /* Floating-point value */
    
    /* Extended attributes for advanced features */
    DataType data_type;         /* Data type of this node */
    AggregationType agg_type;   /* Aggregation type (if applicable) */
    ComparisonOp comparison_op; /* Comparison operator (for filters) */
    SortDirection sort_dir;     /* Sort direction */
    
    /* Child nodes for tree structure */
    struct ASTNode **children;
    size_t child_count;
    
    /* Metadata */
    int line;
    int column;
} ASTNode;

/* Function prototypes */
ASTNode *ast_new(NodeType type, const char *name, const char *value, int numeric_value);
ASTNode *ast_new_float(NodeType type, const char *name, double float_value);
void ast_add_child(ASTNode *parent, ASTNode *child);
void ast_set_data_type(ASTNode *node, DataType dtype);
void ast_set_comparison_op(ASTNode *node, ComparisonOp op);
void ast_set_aggregation(ASTNode *node, AggregationType agg);
void ast_set_sort_direction(ASTNode *node, SortDirection dir);
void ast_free(ASTNode *node);

#endif