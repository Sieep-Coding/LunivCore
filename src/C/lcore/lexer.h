#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include <stdint.h>

/* Token types for LCore */
typedef enum {
    TOKEN_EOF,          // End of file
    TOKEN_HEADER,       // Comment header lines starting with #
    TOKEN_IDENTIFIER,   // Names (datasets, labels)
    TOKEN_STRING,       // Quoted strings
    TOKEN_NUMBER,       // Numeric values
    TOKEN_LBRACE,       // {
    TOKEN_RBRACE,       // }
    TOKEN_COLON,        // :
    TOKEN_AS,           // 'as' keyword
    TOKEN_NEWLINE,      // Line breaks
    TOKEN_UNKNOWN       // Unrecognized token
} TokenType;

/* Token structure */
typedef struct {
    TokenType type;      // Type of token
    const char *lexeme;  // Pointer to lexeme string
    int numeric_value;   // Numeric value if type is TOKEN_NUMBER
    int line;            // Line number for error reporting
    int column;          // Column number
} Token;

/* Lexer state */
typedef struct {
    const char *source;  // Source string
    size_t pos;          // Current position in source
    int line;            // Current line
    int column;          // Current column
} Lexer;

/* Initialize lexer with source string */
void lexer_init(Lexer *lexer, const char *source);

/* Return the next token from source */
Token lexer_next(Lexer *lexer);

/* Free dynamically allocated data in token if needed */
void token_free(Token *token);

#endif // LEXER_H
