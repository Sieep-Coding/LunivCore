#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "core/dataset.h"
#include "lcore/parser.h"
#include "lcore/ast.h"
#include "lcore/lexer.h"
#include "lcore/render.h"

/* Forward declarations of Lua functions */
static int l_dataset_new(lua_State *L);
static int l_dataset_add(lua_State *L);
static int l_dataset_plot(lua_State *L);

/* BI.Dataset methods */
static const struct luaL_Reg dataset_methods[] = {
    {"add",  l_dataset_add},
    {"plot", l_dataset_plot},
    {NULL, NULL}
};

/* BI library (global namespace) */
static const struct luaL_Reg bi_lib_funcs[] = {
    {"new", l_dataset_new},
    {NULL, NULL}
};

/* Lua userdata constructor */
static int l_dataset_new(lua_State *L) {
    const char *title = luaL_checkstring(L, 1);

    DataSet *ds = (DataSet *)lua_newuserdata(L, sizeof(DataSet));
    dataset_init(ds, title);

    luaL_getmetatable(L, "BI.Dataset");
    lua_setmetatable(L, -2);

    return 1;
}

/* Lua methods */
static int l_dataset_add(lua_State *L) {
    DataSet *ds = luaL_checkudata(L, 1, "BI.Dataset");
    const char *label = luaL_checkstring(L, 2);
    int value = luaL_checkinteger(L, 3);

    dataset_add(ds, label, value);
    return 0;
}

static int l_dataset_plot(lua_State *L) {
    DataSet *ds = luaL_checkudata(L, 1, "BI.Dataset");
    dataset_plot(ds);
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <file.lcore>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) { perror("File open failed"); return 1; }

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *src = malloc(len + 1);
    fread(src, 1, len, f);
    src[len] = '\0';
    fclose(f);

    Lexer lexer;
    lexer_init(&lexer, src);

    Parser parser;
    parser_init(&parser, &lexer);

    ASTNode *root = parser_parse(&parser);

    for (size_t i = 0; i < root->child_count; i++) {
        ASTNode *child = root->children[i];
        if (child->type == NODE_DATASET) render_dataset(child);
    }

    ast_free(root);
    free(src);
    return 0;
}
