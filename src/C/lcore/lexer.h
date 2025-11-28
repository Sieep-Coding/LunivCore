#ifndef LEXER_H
#define LEXER_H
#include <stddef.h>  
#include <stdint.h>
typedef enum {
    TOKEN_EOF,
    TOKEN_HEADER,
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_COLON,
    TOKEN_AS,
    TOKEN_NEWLINE,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    const char *lexeme; // string value of token
    int numeric_value;  // if token is a number
    int line;
    int column;
} Token;

/* Lexer interface */
typedef struct {
    const char *source;
    size_t pos;
    int line;
    int column;
} Lexer;

/* Initialize lexer */
void lexer_init(Lexer *lexer, const char *source);

/* Get next token */
Token lexer_next(Lexer *lexer);

/* Free token if needed */
void token_free(Token *token);

#endif
