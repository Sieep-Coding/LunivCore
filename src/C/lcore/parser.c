#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void parser_advance(Parser *parser) {
    token_free(&parser->current_token);
    parser->current_token = lexer_next(parser->lexer);
}

static void parser_expect(Parser *parser, TokenType type) {
    if (parser->current_token.type != type) {
        fprintf(stderr, "Parse error: expected token %d at line %d, column %d\n",
                type, parser->current_token.line, parser->current_token.column);
        exit(1);
    }
}

static ASTNode *parse_row(Parser *parser) {
    parser_expect(parser, TOKEN_IDENTIFIER);
    char *label = strdup(parser->current_token.lexeme);
    parser_advance(parser);

    parser_expect(parser, TOKEN_COLON);
    parser_advance(parser);

    parser_expect(parser, TOKEN_NUMBER);
    int value = parser->current_token.numeric_value;
    parser_advance(parser);

    return ast_new(NODE_ROW, label, NULL, value);
}

static ASTNode *parse_dataset(Parser *parser) {
    parser_expect(parser, TOKEN_IDENTIFIER); // "dataset"
    parser_advance(parser);

    parser_expect(parser, TOKEN_IDENTIFIER); // name
    char *name = strdup(parser->current_token.lexeme);
    parser_advance(parser);

    parser_expect(parser, TOKEN_STRING); // title
    char *title = strdup(parser->current_token.lexeme);
    parser_advance(parser);

    parser_expect(parser, TOKEN_LBRACE);
    parser_advance(parser);

    ASTNode *dataset_node = ast_new(NODE_DATASET, name, title, 0);

    while (parser->current_token.type != TOKEN_RBRACE &&
           parser->current_token.type != TOKEN_EOF) {
        ASTNode *row = parse_row(parser);
        ast_add_child(dataset_node, row);
    }

    parser_expect(parser, TOKEN_RBRACE);
    parser_advance(parser);

    return dataset_node;
}

static ASTNode *parse_plot(Parser *parser) {
    parser_expect(parser, TOKEN_IDENTIFIER); // "plot"
    parser_advance(parser);

    parser_expect(parser, TOKEN_IDENTIFIER); // dataset name
    char *dataset_name = strdup(parser->current_token.lexeme);
    parser_advance(parser);

    parser_expect(parser, TOKEN_AS);
    parser_advance(parser);

    parser_expect(parser, TOKEN_IDENTIFIER); // plot type
    char *plot_type = strdup(parser->current_token.lexeme);
    parser_advance(parser);

    return ast_new(NODE_PLOT, dataset_name, plot_type, 0);
}

static ASTNode *parse_export(Parser *parser) {
    parser_expect(parser, TOKEN_IDENTIFIER); // "export"
    parser_advance(parser);

    parser_expect(parser, TOKEN_IDENTIFIER); // format
    char *format = strdup(parser->current_token.lexeme);
    parser_advance(parser);

    parser_expect(parser, TOKEN_STRING); // filename
    char *filename = strdup(parser->current_token.lexeme);
    parser_advance(parser);

    return ast_new(NODE_EXPORT, format, filename, 0);
}

void parser_init(Parser *parser, Lexer *lexer) {
    parser->lexer = lexer;
    parser->current_token = lexer_next(lexer);
}

ASTNode *parser_parse(Parser *parser) {
    ASTNode *root = ast_new(NODE_DOCUMENT, NULL, NULL, 0);

    while (parser->current_token.type != TOKEN_EOF) {
        if (parser->current_token.type == TOKEN_HEADER) {
            char *header_text = strdup(parser->current_token.lexeme);
            ASTNode *header = ast_new(NODE_HEADER, NULL, header_text, 0);
            ast_add_child(root, header);
            parser_advance(parser);
        } else if (strcmp(parser->current_token.lexeme, "dataset") == 0) {
            ASTNode *dataset = parse_dataset(parser);
            ast_add_child(root, dataset);
        } else if (strcmp(parser->current_token.lexeme, "plot") == 0) {
            ASTNode *plot = parse_plot(parser);
            ast_add_child(root, plot);
        } else if (strcmp(parser->current_token.lexeme, "export") == 0) {
            ASTNode *export_node = parse_export(parser);
            ast_add_child(root, export_node);
        } else {
            fprintf(stderr, "Unknown statement at line %d, column %d\n",
                    parser->current_token.line, parser->current_token.column);
            exit(1);
        }
    }

    return root;
}
