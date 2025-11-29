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

/* ---------------------------- */
/* Lua Binding Forward Decls    */
/* ---------------------------- */

static int l_dataset_new(lua_State *L);
static int l_dataset_add(lua_State *L);
static int l_dataset_plot(lua_State *L);
static int l_bi_eval(lua_State *L);   /* <-- required */

/* BI.Dataset methods */
static const struct luaL_Reg dataset_methods[] = {
    {"add",  l_dataset_add},
    {"plot", l_dataset_plot},
    {NULL, NULL}
};

/* BI global namespace */
static const struct luaL_Reg bi_lib_funcs[] = {
    {"new",  l_dataset_new},
    {"eval", l_bi_eval},  /* <-- required */
    {NULL, NULL}
};

/* ---------------------------- */
/* Lua Binding Implementation   */
/* ---------------------------- */

static int l_dataset_new(lua_State *L) {
    const char *title = luaL_checkstring(L, 1);

    DataSet *ds = (DataSet *)lua_newuserdata(L, sizeof(DataSet));
    dataset_init(ds, title);

    luaL_getmetatable(L, "BI.Dataset");
    lua_setmetatable(L, -2);

    return 1;
}

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

/* ------------------------------------------------ */
/* BI.eval — run embedded LCore DSL from Lua string */
/* ------------------------------------------------ */

static int l_bi_eval(lua_State *L) {
    const char *src = luaL_checkstring(L, 1);

    Lexer lexer;
    lexer_init(&lexer, src);

    Parser parser;
    parser_init(&parser, &lexer);

    ASTNode *root = parser_parse(&parser);
    if (!root) {
        luaL_error(L, "LCore parse failed");
        return 0;
    }

    for (size_t i = 0; i < root->child_count; i++) {
        ASTNode *child = root->children[i];
        if (child->type == NODE_DATASET)
            render_dataset(child);
    }

    ast_free(root);
    return 0;
}

/* ---------------------------- */
/* File Helpers                 */
/* ---------------------------- */

static const char *get_extension(const char *path) {
    const char *dot = strrchr(path, '.');
    return dot ? dot + 1 : NULL;
}

static int is_lua(const char *ext) {
    return ext && strcmp(ext, "lua") == 0;
}

static int is_lcore(const char *ext) {
    return ext && strcmp(ext, "lcore") == 0;
}

/* ---------------------------- */
/* Lua Execution Path           */
/* ---------------------------- */

static void run_lua_file(const char *path) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    /* ------------------------------ */
    /* FIX 1: Proper BI.Dataset meta  */
    /* ------------------------------ */
    luaL_newmetatable(L, "BI.Dataset");

    /* metatable.__index = dataset_methods */
    lua_newtable(L);
    luaL_setfuncs(L, dataset_methods, 0);
    lua_setfield(L, -2, "__index");

    lua_pop(L, 1);

    /* Register BI */
    lua_newtable(L);
    luaL_setfuncs(L, bi_lib_funcs, 0);
    lua_setglobal(L, "BI");

    /* Execute */
    if (luaL_dofile(L, path) != LUA_OK) {
        fprintf(stderr, "Lua Error: %s\n", lua_tostring(L, -1));
    }

    lua_close(L);
}

/* ---------------------------- */
/* LCore DSL Execution Path     */
/* ---------------------------- */

static void run_lcore_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        perror("File open failed");
        return;
    }

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
    if (!root) {
        fprintf(stderr, "Parse error.\n");
        free(src);
        return;
    }

    for (size_t i = 0; i < root->child_count; i++) {
        ASTNode *child = root->children[i];
        if (child->type == NODE_DATASET)
            render_dataset(child);
    }

    ast_free(root);
    free(src);
}

/* ---------------------------- */
/* Main Entry                   */
/* ---------------------------- */

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <file.lua | file.lcore>\n", argv[0]);
        return 1;
    }

    const char *path = argv[1];
    const char *ext = get_extension(path);

    if (is_lua(ext)) {
        run_lua_file(path);
    }
    else if (is_lcore(ext)) {
        run_lcore_file(path);
    }
    else {
        fprintf(stderr, "Unsupported file type: %s\n",
                ext ? ext : "(none)");
        return 1;
    }

    return 0;
}
