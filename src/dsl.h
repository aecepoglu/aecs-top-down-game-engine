#ifndef DSL_H
#define DSL_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>


lua_State *lua;

lua_State* initLua();
void dsl_gameOver();

#endif /*DSL_H*/
