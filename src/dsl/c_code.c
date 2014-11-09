#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <stdio.h>
#include <string.h>

#define LINEBUFSIZE 256

static int test_fun_1( lua_State * L )
{
    printf( "t1 function fired\n" );
    return 0;
}

int counter =  0;
int triggerSet = 0;
lua_CFunction trigger;
int triggerInt;

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
	printf("count: %d\n", counter);
	if( counter == 3) {
	
		printf("\tpull the trigger\n");
		if( triggerSet) {
			lua_rawgeti( l, LUA_REGISTRYINDEX, triggerInt);

			if ( 0 != lua_pcall( l, 0, 0, 0 ) ) {
				printf("Failed to call the callback!\n %s\n", lua_tostring( l, -1 ) );
				return;
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
	
	triggerSet = 1;
	triggerInt = luaL_ref( l, LUA_REGISTRYINDEX);

	return 0;
}

static const struct luaL_Reg myLuaLib[] = {
	{"doubler", doubler},
	{"tripler", tripler},
	{"inc", count},
	{"setTrigger", setTrigger},
	{NULL, NULL}
};

int luaopen_aoeu( lua_State *l) {
	luaL_setfuncs( l, myLuaLib, 0);
	return 1;
}



int main ( void )
{
    char buff[LINEBUFSIZE];
    lua_State * L;
    int error;

    printf( "Test starts.\n\n" );

    L = luaL_newstate();
    luaL_openlibs( L ); 

    lua_register( L, "t1", test_fun_1 );

	//sending a variable to lua code
	lua_pushnumber( L, 1);
	lua_setglobal( L, "PLAYER");

	//TODO
	//sending an array for use in lua

	//sharing function
	lua_pushcfunction( L, tripler);
	lua_setglobal( L, "timesThree");

	//sharing functions as lib
	//libImporter( L);

	//simple package definition
	lua_newtable( L);
	luaL_setfuncs( L, myLuaLib, 0);
	lua_setglobal( L, "tab");

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
