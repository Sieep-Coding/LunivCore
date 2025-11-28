#include "lexer.h"
#include <stdlib.h>
#include <string.h>

void lexer_init(Lexer *lexer, const char *source) {
    lexer->source = source;
    lexer->pos = 0;
    lexer->line = 1;
    lexer->column = 1;
}

Token lexer_next(Lexer *lexer) {
    Token t;
    t.type = TOKEN_EOF;
    t.lexeme = NULL;
    t.numeric_value = 0;
    t.line = lexer->line;
    t.column = lexer->column;
    return t;
}

void token_free(Token *token) {
    if (token->lexeme) free((void *)token->lexeme);
}
