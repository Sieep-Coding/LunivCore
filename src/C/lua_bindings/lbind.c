/* lua_bindings/lbind.c - Enhanced with chart rendering support */

#include "lbind.h"
#include <stdio.h>
#include <string.h>

/* System headers */
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/* Project headers */
#include "core/dataset.h"
#include "lcore/lexer.h"
#include "lcore/parser.h"
#include "lcore/ast.h"
#include "lcore/render.h"

/* Forward declaration - defined in render.h */
void render_chart(ASTNode *chart_node, ASTNode *data_node);

/* --- Forward Declarations --- */
static int l_dataset_new(lua_State *L);
static int l_dataset_add(lua_State *L);
static int l_dataset_plot(lua_State *L);
static int l_dataset_chart(lua_State *L);
static int l_bi_eval(lua_State *L);

/* Aggregation functions */
static int l_dataset_sum(lua_State *L);
static int l_dataset_avg(lua_State *L);
static int l_dataset_min(lua_State *L);
static int l_dataset_max(lua_State *L);
static int l_dataset_count(lua_State *L);

/* Rendering functions */
static int l_render_view_table(lua_State *L);
static int l_render_chart_lua(lua_State *L);
static int l_bi_text(lua_State *L);

/* BI.Dataset methods */
static const struct luaL_Reg dataset_methods[] = {
    {"add",         l_dataset_add},
    {"plot",        l_dataset_plot},
    {"chart",       l_dataset_chart},      /* NEW: generic chart */
    {"sum",         l_dataset_sum},
    {"avg",         l_dataset_avg},
    {"min",         l_dataset_min},
    {"max",         l_dataset_max},
    {"count",       l_dataset_count},
    {NULL, NULL}
};

/* BI global namespace */
static const struct luaL_Reg bi_lib_funcs[] = {
    {"new",         l_dataset_new},
    {"eval",        l_bi_eval},
    {"text",        l_bi_text},
    {"render_view", l_render_view_table},
    {"chart",       l_render_chart_lua},   /* NEW: render chart from Lua */
    {NULL, NULL}
};

/* ========== Dataset Operations ========== */

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

/* NEW: Generic chart function for datasets
   Usage: ds:chart("bar"), ds:chart("line"), etc.
   
   This creates AST nodes from the dataset and renders them as a chart.
*/
static int l_dataset_chart(lua_State *L) {
    DataSet *ds = luaL_checkudata(L, 1, "BI.Dataset");
    const char *chart_type = luaL_checkstring(L, 2);

    if (!ds || !chart_type) {
        luaL_error(L, "Invalid dataset or chart type");
        return 0;
    }

    if (ds->count == 0) {
        fprintf(stderr, "Warning: Dataset is empty, skipping chart\n");
        return 0;
    }

    /* Create AST dataset node from the DataSet
       Using the actual structure: labels[], values[], count */
    ASTNode *data = ast_new(NODE_DATASET, "lua_data", ds->title, 0);

    /* Iterate through dataset rows */
    for (size_t i = 0; i < ds->count; i++) {
        ASTNode *row = ast_new(NODE_ROW, 
                              ds->labels[i],    /* label from labels array */
                              NULL,
                              ds->values[i]);   /* value from values array */
        ast_add_child(data, row);
    }

    /* Create chart node with specified type */
    ASTNode *chart = ast_new(NODE_PLOT, "chart", chart_type, 0);

    /* Render chart */
    render_chart(chart, data);

    /* Cleanup */
    ast_free(chart);
    ast_free(data);

    return 0;
}

/* ========== Aggregation Functions ========== */

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

/* ========== Rendering Functions ========== */

static int l_bi_text(lua_State *L) {
    const char *text = luaL_checkstring(L, 1);

    /* Check for current view context */
    lua_getfield(L, LUA_REGISTRYINDEX, "BI.current_view");
    ASTNode *current_view = lua_touserdata(L, -1);
    lua_pop(L, 1);

    ASTNode *node = ast_new(NODE_TEXT, NULL, strdup(text), 0);

    if (current_view) {
        /* Attach to current view */
        ast_add_child(current_view, node);
    } else {
        /* Standalone: render single box */
        render_text(node);
        ast_free(node);
    }

    return 0;
}

static int l_render_view_table(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);

    /* Extract title */
    lua_getfield(L, 1, "title");
    const char *title = lua_tostring(L, -1);
    lua_pop(L, 1);
    if (!title || title[0] == '\0') title = "VIEW";

    ASTNode *view_node = ast_new(NODE_VIEW, NULL, strdup(title), 0);

    /* Set as current view context in registry */
    lua_pushlightuserdata(L, view_node);
    lua_setfield(L, LUA_REGISTRYINDEX, "BI.current_view");

    /* Add rows */
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

    /* Add children (supports text nodes) */
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

    /* Clear current view context */
    lua_pushnil(L);
    lua_setfield(L, LUA_REGISTRYINDEX, "BI.current_view");

    render_view(view_node);
    ast_free(view_node);

    return 0;
}

/**
 * NEW: Generic chart rendering from Lua
 * Usage: BI.chart("bar", data_table)
 * 
 * data_table format:
 *   {
 *       {name = "Label1", value = 100},
 *       {name = "Label2", value = 200},
 *       ...
 *   }
 */
static int l_render_chart_lua(lua_State *L) {
    const char *chart_type = luaL_checkstring(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);

    ASTNode *data = ast_new(NODE_DATASET, "lua_data", "Data", 0);

    /* Iterate over Lua table and build AST rows */
    lua_pushnil(L);
    while (lua_next(L, 2) != 0) {
        if (lua_istable(L, -1)) {
            lua_getfield(L, -1, "name");
            const char *name = lua_tostring(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "value");
            int value = (int)lua_tointeger(L, -1);
            lua_pop(L, 1);

            if (name) {
                ASTNode *row = ast_new(NODE_ROW, strdup(name), NULL, value);
                ast_add_child(data, row);
            }
        }
        lua_pop(L, 1);
    }

    /* Create chart node and render */
    ASTNode *chart = ast_new(NODE_PLOT, "chart", chart_type, 0);
    render_chart(chart, data);

    /* Cleanup */
    ast_free(chart);
    ast_free(data);

    return 0;
}

/* ========== DSL Evaluation ========== */

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
        else if (child->type == NODE_TEXT)
            render_text(child);
        else if (child->type == NODE_FUNCTION_CALL) {
            DataSet *ds = dataset_registry_get(child->value);
            if (!ds) {
                fprintf(stderr, "Unknown dataset: %s\n", child->value);
                continue;
            }
            if (strcmp(child->name, "sum") == 0)
                printf("Sum(%s) = %ld\n", child->value, dataset_sum(ds));
            else if (strcmp(child->name, "avg") == 0)
                printf("Avg(%s) = %.2f\n", child->value, dataset_avg(ds));
            else if (strcmp(child->name, "min") == 0)
                printf("Min(%s) = %d\n", child->value, dataset_min(ds));
            else if (strcmp(child->name, "max") == 0)
                printf("Max(%s) = %d\n", child->value, dataset_max(ds));
            else if (strcmp(child->name, "count") == 0)
                printf("Count(%s) = %zu\n", child->value, dataset_count(ds));
        }
    }

    ast_free(root);
    return 0;
}

/* ========== Module Registration ========== */

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