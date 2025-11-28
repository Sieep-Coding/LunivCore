#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

// --- 1. The Data Structure ---
// This is the actual data we want to manipulate.
#define MAX_ROWS 100

typedef struct {
    char title[50];
    char labels[MAX_ROWS][20];
    int values[MAX_ROWS];
    int count;
} DataFrame;

// --- 2. C Implementation of Methods ---

// Constructor: BI.new("Title")
static int l_dataset_new(lua_State *L) {
    const char *title = luaL_checkstring(L, 1);

    // Create 'Userdata' - Lua allocates memory for our struct
    DataFrame *df = (DataFrame *)lua_newuserdata(L, sizeof(DataFrame));
    
    // Initialize the struct
    strcpy(df->title, title);
    df->count = 0;

    // Attach the Metatable (Method lookup)
    // We will define "BI.Dataset" later. This tells Lua this userdata
    // belongs to that class.
    luaL_getmetatable(L, "BI.Dataset");
    lua_setmetatable(L, -2);

    return 1; // Return the userdata object
}

// Method: dataset:add("Label", Value)
static int l_dataset_add(lua_State *L) {
    // Check that the first argument is valid Userdata of type "BI.Dataset"
    DataFrame *df = (DataFrame *)luaL_checkudata(L, 1, "BI.Dataset");
    const char *label = luaL_checkstring(L, 2);
    int value = (int)luaL_checkinteger(L, 3);

    if (df->count < MAX_ROWS) {
        strcpy(df->labels[df->count], label);
        df->values[df->count] = value;
        df->count++;
    }

    return 0; // No return value
}

// Method: dataset:plot()
// Draws a simple ASCII chart
static int l_dataset_plot(lua_State *L) {
    DataFrame *df = (DataFrame *)luaL_checkudata(L, 1, "BI.Dataset");

    printf("\n--- %s ---\n", df->title);
    for (int i = 0; i < df->count; i++) {
        printf("%-10s | ", df->labels[i]);
        // Draw bars
        int bar_len = df->values[i] / 10; // Scale down
        for (int b = 0; b < bar_len; b++) printf("█");
        printf(" (%d)\n", df->values[i]);
    }
    printf("----------------\n");
    return 0;
}


static const struct luaL_Reg dataset_methods[] = {
    {"add", l_dataset_add},
    {"plot", l_dataset_plot},
    {NULL, NULL}
};

static const struct luaL_Reg bi_lib_funcs[] = {
    {"new", l_dataset_new},
    {NULL, NULL}
};

int main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    luaL_newmetatable(L, "BI.Dataset");
    
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luaL_setfuncs(L, dataset_methods, 0);

    luaL_newlib(L, bi_lib_funcs);
    lua_setglobal(L, "BI");

    if (luaL_dofile(L, "src/lua/config.lua") != LUA_OK) {
        printf("Error: %s\n", lua_tostring(L, -1));
    }

    lua_close(L);
    return 0;
}