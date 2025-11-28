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