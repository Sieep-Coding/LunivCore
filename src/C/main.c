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
        fprintf(stderr, "Usage: %s <file.lua|file.lcore>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    if (strstr(filename, ".lua")) {
        /* Legacy Lua execution */
        lua_State *L = luaL_newstate();
        luaL_openlibs(L);

        /* metatable for userdatas */
        luaL_newmetatable(L, "BI.Dataset");
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
        luaL_setfuncs(L, dataset_methods, 0);

        /* global BI table */
        luaL_newlib(L, bi_lib_funcs);
        lua_setglobal(L, "BI");

        if (luaL_dofile(L, filename) != LUA_OK) {
            fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
        }

        lua_close(L);

    } else if (strstr(filename, ".lcore")) {
        /* LCore execution */
        FILE *fp = fopen(filename, "r");
        if (!fp) {
            perror("fopen");
            return 1;
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char *source = malloc(size + 1);
        fread(source, 1, size, fp);
        source[size] = '\0';
        fclose(fp);

        Lexer lexer;
        lexer_init(&lexer, source);

        Parser parser;
        parser_init(&parser, &lexer);

        ASTNode *ast = parser_parse(&parser);

        /* Executor: traverse AST and call Lua/C functions (not implemented yet) */
        printf("Parsed LCore AST. Root node type: %d\n", ast->type);

        ast_free(ast);
        free(source);

    } else {
        fprintf(stderr, "Unsupported file type: %s\n", filename);
        return 1;
    }

    return 0;
}
