#include "parser.h"

void parser_init(Parser *parser, Lexer *lexer) {
    parser->lexer = lexer;
    parser->current_token = lexer_next(lexer);
}

ASTNode *parser_parse(Parser *parser) {
    ASTNode *doc = ast_new(NODE_DOCUMENT, NULL, NULL, 0);
    return doc;
}
