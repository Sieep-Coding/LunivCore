#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "core/dataset.h" 

// Forward declarations
static ASTNode *parse_header(Parser *parser);
static ASTNode *parse_row(Parser *parser);
static ASTNode *parse_dataset(Parser *parser);
static ASTNode *parse_plot(Parser *parser);
static ASTNode *parse_export(Parser *parser);
static ASTNode *parse_view(Parser *parser);

static void parser_advance(Parser *parser) {
    token_free(&parser->current_token);
    parser->current_token = lexer_next(parser->lexer);
}

static void parser_expect(Parser *parser, TokenType type) {
    if (parser->current_token.type != type) {
        fprintf(stderr, "Parse error: expected token %d, got %d at line %d, column %d\n",
                type, parser->current_token.type, parser->current_token.line, parser->current_token.column);
        fprintf(stderr, "  Current lexeme: %s\n", parser->current_token.lexeme ? parser->current_token.lexeme : "NULL");
        exit(1);
    }
}

static ASTNode *parse_header(Parser *parser) {
    parser_expect(parser, TOKEN_HEADER);
    char *text = strdup(parser->current_token.lexeme);
    parser_advance(parser);
    return ast_new(NODE_HEADER, NULL, text, 0);
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

    parser_expect(parser, TOKEN_IDENTIFIER); // dataset name
    char *name = strdup(parser->current_token.lexeme);
    parser_advance(parser);

    parser_expect(parser, TOKEN_STRING); // title
    char *title = strdup(parser->current_token.lexeme);
    parser_advance(parser);

    parser_expect(parser, TOKEN_LBRACE);
    parser_advance(parser);

    ASTNode *dataset_node = ast_new(NODE_DATASET, name, title, 0);

    // Parse rows
    while (parser->current_token.type != TOKEN_RBRACE &&
           parser->current_token.type != TOKEN_EOF) {
        ASTNode *row = parse_row(parser);
        ast_add_child(dataset_node, row);
    }

    parser_expect(parser, TOKEN_RBRACE);
    parser_advance(parser);

    // --- REGISTER DATASET ---
    DataSet *ds = malloc(sizeof(DataSet));
    dataset_init(ds, title);
    for (size_t i = 0; i < dataset_node->child_count; i++) {
        ASTNode *row = dataset_node->children[i];
        dataset_add(ds, row->name, row->numeric_value);
    }
    if (dataset_registry_add(name, ds) != 0) {
        fprintf(stderr, "Failed to register dataset '%s'\n", name);
    }

    return dataset_node;
}

static ASTNode *parse_view(Parser *parser) {
    parser_expect(parser, TOKEN_VIEW); // "view"
    parser_advance(parser);

    parser_expect(parser, TOKEN_IDENTIFIER); // name
    char *name = strdup(parser->current_token.lexeme);
    parser_advance(parser);

    parser_expect(parser, TOKEN_STRING); // title
    char *title = strdup(parser->current_token.lexeme);
    parser_advance(parser);

    parser_expect(parser, TOKEN_LBRACE);
    parser_advance(parser);

    ASTNode *view_node = ast_new(NODE_VIEW, name, title, 0);

    while (parser->current_token.type != TOKEN_RBRACE &&
           parser->current_token.type != TOKEN_EOF) {
        ASTNode *row = parse_row(parser);
        ast_add_child(view_node, row);
    }

    parser_expect(parser, TOKEN_RBRACE);
    parser_advance(parser);

    return view_node;
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
    fprintf(stderr, "DEBUG: parse_export()\n");
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

static ASTNode *parse_function_call(Parser *parser);

ASTNode *parser_parse(Parser *parser) {
    ASTNode *root = ast_new(NODE_DOCUMENT, NULL, NULL, 0);

    while (parser->current_token.type != TOKEN_EOF) {
        if (parser->current_token.type == TOKEN_HEADER) {
            ASTNode *header = parse_header(parser);
            ast_add_child(root, header);
        } 
        else if (parser->current_token.type == TOKEN_IDENTIFIER && 
                 parser->current_token.lexeme &&
                 strcmp(parser->current_token.lexeme, "dataset") == 0) {
            ASTNode *dataset = parse_dataset(parser);
            ast_add_child(root, dataset);
        }
        else if (parser->current_token.type == TOKEN_VIEW) {
            ASTNode *view = parse_view(parser);
            ast_add_child(root, view);
        }
        else if (parser->current_token.type == TOKEN_IDENTIFIER && 
                 parser->current_token.lexeme &&
                 strcmp(parser->current_token.lexeme, "plot") == 0) {
            ASTNode *plot = parse_plot(parser);
            ast_add_child(root, plot);
        } 
        else if (parser->current_token.type == TOKEN_IDENTIFIER && 
                 parser->current_token.lexeme &&
                 strcmp(parser->current_token.lexeme, "export") == 0) {
            ASTNode *export_node = parse_export(parser);
            ast_add_child(root, export_node);
        }
        else if (parser->current_token.type == TOKEN_SUM ||
         parser->current_token.type == TOKEN_AVG ||
         parser->current_token.type == TOKEN_MIN ||
         parser->current_token.type == TOKEN_MAX ||
         parser->current_token.type == TOKEN_COUNT) {
            ASTNode *func_call = parse_function_call(parser);
            ast_add_child(root, func_call);
        }

        else {
            fprintf(stderr, "Unknown statement at line %d, column %d (token type: %d)\n",
                    parser->current_token.line, parser->current_token.column,
                    parser->current_token.type);
            exit(1);
        }
    }

    return root;
}

static ASTNode *parse_function_call(Parser *parser) {
    // Current token is one of TOKEN_SUM, TOKEN_AVG, TOKEN_MIN, TOKEN_MAX, TOKEN_COUNT
    TokenType func_type = parser->current_token.type;
    const char *func_name = NULL;

    switch (func_type) {
        case TOKEN_SUM:   func_name = "sum"; break;
        case TOKEN_AVG:   func_name = "avg"; break;
        case TOKEN_MIN:   func_name = "min"; break;
        case TOKEN_MAX:   func_name = "max"; break;
        case TOKEN_COUNT: func_name = "count"; break;
        default:
            fprintf(stderr, "Unknown aggregation function at line %d, column %d\n",
                    parser->current_token.line, parser->current_token.column);
            exit(1);
    }

    parser_advance(parser);

    parser_expect(parser, TOKEN_IDENTIFIER); // dataset name
    char *dataset_name = strdup(parser->current_token.lexeme);
    parser_advance(parser);

    return ast_new(NODE_FUNCTION_CALL, func_name, dataset_name, 0);
}

ASTNode *ast_new_function_call(const char *func_name, const char *dataset_name) {
    return ast_new(NODE_FUNCTION_CALL, func_name, dataset_name, 0);
}
