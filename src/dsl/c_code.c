#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <stdio.h>
#include <string.h>

#define LINEBUFSIZE 256

int counter =  0;
int triggerInt = LUA_NOREF;

static int tripler( lua_State *l) {
	printf("C: tripler()\n");
	int x = lua_tointeger( l, 1);
	lua_pushinteger( l, x*3);
	return 1;
}

static int doubler( lua_State *l) {
	printf("C: doubler()\n");
	int x = lua_tointeger( l, 1);
	lua_pushinteger( l, x*2);
	return 1;
}

static int count( lua_State *l) {
	counter ++;
	if( counter == 3) {
		if( triggerInt != 0) {
			lua_rawgeti( l, LUA_REGISTRYINDEX, triggerInt);

			if ( 0 != lua_pcall( l, 0, 0, 0 ) ) {
				printf("Failed to call the callback!\n %s\n", lua_tostring( l, -1 ) );
				return 0;
			}
		}
		else
			printf("\t\ttrigger not set.\n");
	
		printf("resetting counter to 0\n");
		counter = 0;
	}
	return 0;
}

static int setTrigger( lua_State *l) {
	luaL_checktype( l, 1, LUA_TFUNCTION);
	printf("trigger set. call inc() to increment counter. trigger will be called when counter reaches 3.\n");
	
	triggerInt = luaL_ref( l, LUA_REGISTRYINDEX);

	return 0;
}

static int consoleWrite( lua_State *l) {
	luaL_checkstring( l, 1);
	printf("CONSOLE: %s\n", lua_tostring( l, 1));
	return 0;
}

static const struct luaL_Reg myLuaLib[] = {
	{"doubler", doubler},
	{"tripler", tripler},
	{"inc", count},
	{"setTrigger", setTrigger},
	{"write", consoleWrite},
	{NULL, NULL}
};



int main ( void )
{
    char buff[LINEBUFSIZE];
    lua_State * L;
    int error;

    printf( "Test starts.\n\n" );

    L = luaL_newstate();
    luaL_openlibs( L ); 

	//sending a variable to lua code
	lua_pushnumber( L, 1);
	lua_setglobal( L, "PLAYER");

	//TODO
	//sending an array for use in lua

	//sharing function
	lua_pushcfunction( L, tripler);
	lua_setglobal( L, "timesThree");

	//simple package definition
	lua_newtable( L);
	luaL_setfuncs( L, myLuaLib, 0);
	lua_setglobal( L, "lib");

    while ( fgets( buff, LINEBUFSIZE, stdin ) != NULL)
    {
      //if ( strcmp( buff, "q\n" ) == 0 )
      //{
      //    break;
      //}
      error = luaL_loadbuffer( L, buff, strlen(buff), "line" ) ||
              lua_pcall( L, 0, 0, 0 );
      if (error)
      {
        printf( "Test error: %s\n", lua_tostring( L, -1 ) );
        lua_pop( L, 1 );
      }
    }
    lua_close( L );

    printf( "\n\nTest ended.\n" );
    return 0;
 }