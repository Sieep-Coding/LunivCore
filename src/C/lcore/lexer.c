#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int match_sum(const char *str, size_t len)   { return len == 3 && strcmp(str, "sum") == 0; }
static int match_avg(const char *str, size_t len)   { return len == 3 && strcmp(str, "avg") == 0; }
static int match_min(const char *str, size_t len)   { return len == 3 && strcmp(str, "min") == 0; }
static int match_max(const char *str, size_t len)   { return len == 3 && strcmp(str, "max") == 0; }
static int match_count(const char *str, size_t len) { return len == 5 && strcmp(str, "count") == 0; }


static int is_identifier_char(char c) {
    return isalnum(c) || c == '_';
}

static int match_as(const char *str, size_t len) {
    return len == 2 && str[0] == 'a' && str[1] == 's';
}

static int match_view(const char *str, size_t len) {
    return len == 4 && strcmp(str, "view") == 0;
}

void lexer_init(Lexer *lexer, const char *source) {
    lexer->source = source;
    lexer->pos = 0;
    lexer->line = 1;
    lexer->column = 1;
}

static char advance(Lexer *lexer) {
    char c = lexer->source[lexer->pos];
    lexer->pos++;
    if (c == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    return c;
}

static char peek(Lexer *lexer) {
    return lexer->source[lexer->pos];
}

Token lexer_next(Lexer *lexer) {
    Token token;
    token.lexeme = NULL;
    token.numeric_value = 0;
    token.line = lexer->line;
    token.column = lexer->column;
    token.type = TOKEN_EOF;

    const char *src = lexer->source;

    while (isspace(peek(lexer))) advance(lexer);

    char c = peek(lexer);
    if (c == '\0') return token;

    if (c == '#') {
        advance(lexer);
        size_t start = lexer->pos;
        while (peek(lexer) != '\n' && peek(lexer) != '\0') advance(lexer);
        size_t len = lexer->pos - start;
        token.type = TOKEN_HEADER;
        token.lexeme = malloc(len + 1);
        strncpy((char *)token.lexeme, src + start, len);
        ((char *)token.lexeme)[len] = '\0';
        return token;
    }

    switch (c) {
        case '{': advance(lexer); token.type = TOKEN_LBRACE; return token;
        case '}': advance(lexer); token.type = TOKEN_RBRACE; return token;
        case ':': advance(lexer); token.type = TOKEN_COLON; return token;
        case '\n': advance(lexer); token.type = TOKEN_NEWLINE; return token;
    }

    if (isdigit(c)) {
        size_t start = lexer->pos;
        while (isdigit(peek(lexer))) advance(lexer);
        size_t len = lexer->pos - start;
        char *buf = malloc(len + 1);
        strncpy(buf, src + start, len);
        buf[len] = '\0';
        token.type = TOKEN_NUMBER;
        token.lexeme = buf;
        token.numeric_value = atoi(buf);
        return token;
    }

    if (c == '"') {
        advance(lexer);
        size_t start = lexer->pos;
        while (peek(lexer) != '"' && peek(lexer) != '\0') advance(lexer);
        size_t len = lexer->pos - start;
        char *buf = malloc(len + 1);
        strncpy(buf, src + start, len);
        buf[len] = '\0';
        token.type = TOKEN_STRING;
        token.lexeme = buf;
        if (peek(lexer) == '"') advance(lexer);
        return token;
    }

    if (isalpha(c) || c == '_') {
        size_t start = lexer->pos;
        while (is_identifier_char(peek(lexer))) advance(lexer);
        size_t len = lexer->pos - start;
        char *buf = malloc(len + 1);
        strncpy(buf, src + start, len);
        buf[len] = '\0';

        /* Check for keywords */
        if (match_as(buf, len)) {
            token.type = TOKEN_AS; free(buf); token.lexeme = NULL;
        } else if (match_view(buf, len)) {
            token.type = TOKEN_VIEW; free(buf); token.lexeme = NULL;
        } else if (match_sum(buf, len)) {
            token.type = TOKEN_SUM; free(buf); token.lexeme = NULL;
        } else if (match_avg(buf, len)) {
            token.type = TOKEN_AVG; free(buf); token.lexeme = NULL;
        } else if (match_min(buf, len)) {
            token.type = TOKEN_MIN; free(buf); token.lexeme = NULL;
        } else if (match_max(buf, len)) {
            token.type = TOKEN_MAX; free(buf); token.lexeme = NULL;
        } else if (match_count(buf, len)) {
            token.type = TOKEN_COUNT; free(buf); token.lexeme = NULL;
        } else {
            token.type = TOKEN_IDENTIFIER; token.lexeme = buf;
        }
        return token;
    }


    advance(lexer);
    token.type = TOKEN_UNKNOWN;
    return token;
}

void token_free(Token *token) {
    if (token->lexeme) free((void *)token->lexeme);
}