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
/* Aggregation forward declarations */
static int l_dataset_sum(lua_State *L);
static int l_dataset_avg(lua_State *L);
static int l_dataset_min(lua_State *L);
static int l_dataset_max(lua_State *L);
static int l_dataset_count(lua_State *L);
static int l_render_view_table(lua_State *L);
static int l_bi_text(lua_State *L);


/* BI.Dataset methods */
static const struct luaL_Reg dataset_methods[] = {
    {"add",   l_dataset_add},
    {"plot",  l_dataset_plot},
    {"sum",   l_dataset_sum},
    {"avg",   l_dataset_avg},
    {"min",   l_dataset_min},
    {"max",   l_dataset_max},
    {"count", l_dataset_count},
    {NULL, NULL}
};


/* BI global namespace */
static const struct luaL_Reg bi_lib_funcs[] = {
    {"new",  l_dataset_new},
    {"eval", l_bi_eval},
    {"text", l_bi_text},
    {"render_view", l_render_view_table},
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

static int l_bi_text(lua_State *L) {
    const char *text = luaL_checkstring(L, 1);

    // Check for current view context
    lua_getfield(L, LUA_REGISTRYINDEX, "BI.current_view");
    ASTNode *current_view = lua_touserdata(L, -1);
    lua_pop(L, 1);

    ASTNode *node = ast_new(NODE_TEXT, NULL, strdup(text), 0);

    if (current_view) {
        // Attach to current view
        ast_add_child(current_view, node);
    } else {
        // Standalone note: render a single box
        render_text(node);
        ast_free(node);
    }

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

static int l_dataset_sum(lua_State *L) {
    DataSet *ds = luaL_checkudata(L, 1, "BI.Dataset");
    long s = dataset_sum(ds);
    lua_pushinteger(L, (lua_Integer)s);
    return 1;
}

static int l_dataset_avg(lua_State *L) {
    DataSet *ds = luaL_checkudata(L, 1, "BI.Dataset");
    double a = dataset_avg(ds);
    lua_pushnumber(L, (lua_Number)a);
    return 1;
}

static int l_dataset_min(lua_State *L) {
    DataSet *ds = luaL_checkudata(L, 1, "BI.Dataset");
    int m = dataset_min(ds);
    lua_pushinteger(L, m);
    return 1;
}

static int l_dataset_max(lua_State *L) {
    DataSet *ds = luaL_checkudata(L, 1, "BI.Dataset");
    int m = dataset_max(ds);
    lua_pushinteger(L, m);
    return 1;
}

static int l_dataset_count(lua_State *L) {
    DataSet *ds = luaL_checkudata(L, 1, "BI.Dataset");
    size_t c = dataset_count(ds);
    lua_pushinteger(L, (lua_Integer)c);
    return 1;
}

static int l_render_view_table(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);

    // Extract title
    lua_getfield(L, 1, "title");
    const char *title = lua_tostring(L, -1);
    lua_pop(L, 1);
    if (!title || title[0] == '\0') title = "VIEW";

    ASTNode *view_node = ast_new(NODE_VIEW, NULL, strdup(title), 0);

    // Set as current view context in registry
    lua_pushlightuserdata(L, view_node);
    lua_setfield(L, LUA_REGISTRYINDEX, "BI.current_view");

    // Add rows
    lua_getfield(L, 1, "rows");
    if (lua_istable(L, -1)) {
        size_t n = lua_rawlen(L, -1);
        for (size_t i = 1; i <= n; i++) {
            lua_rawgeti(L, -1, i);
            if (lua_istable(L, -1)) {
                lua_getfield(L, -1, "name");
                const char *name = lua_tostring(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "value");
                int value = (int)lua_tointeger(L, -1);
                lua_pop(L, 1);

                ASTNode *row = ast_new(NODE_ROW, strdup(name), NULL, value);
                ast_add_child(view_node, row);
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    // Add children (supports text nodes)
    lua_getfield(L, 1, "children");
    if (lua_istable(L, -1)) {
        size_t n = lua_rawlen(L, -1);
        for (size_t i = 1; i <= n; i++) {
            lua_rawgeti(L, -1, i);
            if (lua_istable(L, -1)) {
                lua_getfield(L, -1, "type");
                const char *type = lua_tostring(L, -1);
                lua_pop(L, 1);

                if (type && strcmp(type, "text") == 0) {
                    lua_getfield(L, -1, "value");
                    const char *val = lua_tostring(L, -1);
                    lua_pop(L, 1);

                    ASTNode *text_node = ast_new(NODE_TEXT, NULL, strdup(val), 0);
                    ast_add_child(view_node, text_node);
                }
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    // Clear current view context
    lua_pushnil(L);
    lua_setfield(L, LUA_REGISTRYINDEX, "BI.current_view");

    render_view(view_node);
    ast_free(view_node);

    return 0;
}

