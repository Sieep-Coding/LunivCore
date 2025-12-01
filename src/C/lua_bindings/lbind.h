#ifndef LBIND_H
#define LBIND_H

#include <lua.h>

/*
 * Initializes the Lua state, registers the BI library and the
 * BI.Dataset metatable, and executes the specified Lua file.
 */
void lbind_run_file(const char *path);

#endif /* LBIND_H */