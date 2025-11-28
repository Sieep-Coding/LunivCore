#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

/* Parser object */
typedef struct {
    Lexer *lexer;
    Token current_token;
} Parser;

/* Initialize parser */
void parser_init(Parser *parser, Lexer *lexer);

/* Parse the source into an AST */
ASTNode *parser_parse(Parser *parser);

#endif
