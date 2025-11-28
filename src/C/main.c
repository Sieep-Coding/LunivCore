#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "core/dataset.h"

/*
 * Lua constructor: BI.new("Title")
 */
static int l_dataset_new(lua_State *L) {
    const char *title = luaL_checkstring(L, 1);

    DataSet *ds = (DataSet *)lua_newuserdata(L, sizeof(DataSet));
    dataset_init(ds, title);

    luaL_getmetatable(L, "BI.Dataset");
    lua_setmetatable(L, -2);

    return 1;
}

/*
 * Lua method: dataset:add("Label", value)
 */
static int l_dataset_add(lua_State *L) {
    DataSet *ds = luaL_checkudata(L, 1, "BI.Dataset");
    const char *label = luaL_checkstring(L, 2);
    int value = luaL_checkinteger(L, 3);

    dataset_add(ds, label, value);
    return 0;
}

/*
 * Lua method: dataset:plot()
 */
static int l_dataset_plot(lua_State *L) {
    DataSet *ds = luaL_checkudata(L, 1, "BI.Dataset");
    dataset_plot(ds);
    return 0;
}


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

int main(void) {
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

    if (luaL_dofile(L, "src/lua/config.lua") != LUA_OK) {
        fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
    }

    lua_close(L);
    return 0;
}
