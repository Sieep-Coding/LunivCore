#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include <stdint.h>

/* Extended token types for enterprise features */
typedef enum {
    TOKEN_EOF,              /* End of file */
    TOKEN_HEADER,           /* Comment header lines starting with # */
    TOKEN_IDENTIFIER,       /* Names (datasets, labels) */
    TOKEN_STRING,           /* Quoted strings */
    TOKEN_NUMBER,           /* Numeric values */
    TOKEN_FLOAT,            /* Floating-point numbers */
    TOKEN_DATE,             /* ISO 8601 dates */
    TOKEN_LBRACE,           /* { */
    TOKEN_RBRACE,           /* } */
    TOKEN_LPAREN,           /* ( */
    TOKEN_RPAREN,           /* ) */
    TOKEN_LBRACKET,         /* [ */
    TOKEN_RBRACKET,         /* ] */
    TOKEN_COLON,            /* : */
    TOKEN_COMMA,            /* , */
    TOKEN_SEMICOLON,        /* ; */
    TOKEN_DOT,              /* . */
    TOKEN_PIPE,             /* | */
    
    /* Keywords */
    TOKEN_AS,               /* 'as' keyword */
    TOKEN_VIEW,             /* 'view' keyword */
    TOKEN_DATASET,          /* 'dataset' keyword */
    TOKEN_PLOT,             /* 'plot' keyword */
    TOKEN_FILTER,           /* 'filter' keyword */
    TOKEN_WHERE,            /* 'where' keyword */
    TOKEN_AGGREGATE,        /* 'aggregate' keyword */
    TOKEN_GROUP_BY,         /* 'group_by' keyword */
    TOKEN_ORDER_BY,         /* 'order_by' keyword */
    TOKEN_JOIN,             /* 'join' keyword */
    TOKEN_EXPORT,           /* 'export' keyword */
    TOKEN_SORT,             /* 'sort' keyword */
    TOKEN_COMPUTED,         /* 'computed' keyword for derived columns */
    TOKEN_SELECT,           /* 'select' keyword */
    TOKEN_FROM,             /* 'from' keyword */
    TOKEN_INTO,             /* 'into' keyword */
    
    /* Operators */
    TOKEN_EQ,               /* == */
    TOKEN_NE,               /* != */
    TOKEN_LT,               /* < */
    TOKEN_LE,               /* <= */
    TOKEN_GT,               /* > */
    TOKEN_GE,               /* >= */
    TOKEN_PLUS,             /* + */
    TOKEN_MINUS,            /* - */
    TOKEN_STAR,             /* * */
    TOKEN_SLASH,            /* / */
    TOKEN_PERCENT,          /* % */
    TOKEN_AND,              /* && or AND */
    TOKEN_OR,               /* || or OR */
    TOKEN_NOT,              /* ! or NOT */
    TOKEN_ASSIGN,           /* = */
    TOKEN_ARROW,            /* -> */
    
    /* Aggregation functions */
    TOKEN_SUM,              /* 'sum' */
    TOKEN_AVG,              /* 'avg' or 'average' */
    TOKEN_MIN,              /* 'min' */
    TOKEN_MAX,              /* 'max' */
    TOKEN_COUNT,            /* 'count' */
    TOKEN_STDDEV,           /* 'stddev' */
    TOKEN_MEDIAN,           /* 'median' */
    /* Data types */
    TOKEN_INT_TYPE,         /* 'int' */
    TOKEN_FLOAT_TYPE,       /* 'float' or 'double' */
    TOKEN_STRING_TYPE,      /* 'string' or 'text' */
    TOKEN_DATE_TYPE,        /* 'date' */
    TOKEN_BOOL_TYPE,        /* 'bool' or 'boolean' */
    
    TOKEN_TEXT,
    /* Visualization types */
    TOKEN_BAR,              /* 'bar' */
    TOKEN_LINE,             /* 'line' */
    TOKEN_TABLE,            /* 'table' */
    TOKEN_HISTOGRAM,        /* 'histogram' */
    TOKEN_SCATTER,          /* 'scatter' */
    TOKEN_PIE,              /* 'pie' */
    TOKEN_HEATMAP,          /* 'heatmap' */
    
    /* Other */
    TOKEN_NEWLINE,          /* Line breaks */
    TOKEN_UNKNOWN           /* Unrecognized token */
} TokenType;

/* Token structure with extended metadata */
typedef struct {
    TokenType type;
    const char *lexeme;
    int numeric_value;
    double float_value;
    int line;
    int column;
} Token;

/* Lexer state */
typedef struct {
    const char *source;
    size_t pos;
    int line;
    int column;
} Lexer;

/* Function prototypes */
void lexer_init(Lexer *lexer, const char *source);
Token lexer_next(Lexer *lexer);
void token_free(Token *token);
const char *token_type_to_string(TokenType type);

#endif