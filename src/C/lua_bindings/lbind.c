#include "lbind.h"

#include <stdio.h>
#include <string.h>

/* System headers already included in main.c */
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/* Project headers */
#include "core/dataset.h"
#include "lcore/lexer.h"
#include "lcore/parser.h"
#include "lcore/ast.h"
#include "lcore/render.h"

/* --- Forward Decls & Registry Tables --- */

static int l_dataset_new(lua_State *L);
static int l_dataset_add(lua_State *L);
static int l_dataset_plot(lua_State *L);
static int l_bi_eval(lua_State *L);

/* BI.Dataset methods */
static const struct luaL_Reg dataset_methods[] = {
    {"add",  l_dataset_add},
    {"plot", l_dataset_plot},
    {NULL, NULL}
};

/* BI global namespace */
static const struct luaL_Reg bi_lib_funcs[] = {
    {"new",  l_dataset_new},
    {"eval", l_bi_eval},
    {NULL, NULL}
};

/* --- Implementation --- */

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
        else if (child->type == NODE_VIEW)
            render_view(child);
    }

    ast_free(root);
    return 0;
}

static void lbind_register(lua_State *L) {
    /* Create and set BI.Dataset metatable */
    luaL_newmetatable(L, "BI.Dataset");

    /* metatable.__index = dataset_methods */
    lua_newtable(L);
    luaL_setfuncs(L, dataset_methods, 0);
    lua_setfield(L, -2, "__index");

    lua_pop(L, 1); /* Pop metatable */

    /* Register BI global namespace */
    lua_newtable(L);
    luaL_setfuncs(L, bi_lib_funcs, 0);
    lua_setglobal(L, "BI");
}

void lbind_run_file(const char *path) {
    lua_State *L = luaL_newstate();
    if (!L) {
        fprintf(stderr, "Fatal: Could not create Lua state.\n");
        return;
    }

    luaL_openlibs(L);
    lbind_register(L);

    if (luaL_dofile(L, path) != LUA_OK) {
        fprintf(stderr, "Lua Error: %s\n", lua_tostring(L, -1));
    }

    lua_close(L);
}