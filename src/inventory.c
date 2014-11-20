#include "inventory.h"
#include <lauxlib.h>

void inventory_reset( bool deallocate) {
	int i;
	for( i=0; i<INVENTORY_SIZE; i++) {
		if( inventory[ i] != NULL) {
			if( deallocate)
				objectFree( inventory[i]);

			inventory[ i] = NULL;
		}
	}

	inventoryCallbacks.onAdd = LUA_NOREF;
	inventoryCallbacks.onRemove = LUA_NOREF;
}

bool inventory_add( struct object *obj, lua_State *lua) {
	int i;
	for( i=0; i < INVENTORY_SIZE; i++) {
		if( inventory[ i] == NULL) {
			inventory[ i] = obj;

			if( inventoryCallbacks.onAdd != LUA_NOREF) {
				lua_rawgeti( lua, LUA_REGISTRYINDEX, inventoryCallbacks.onAdd);
				lua_pushinteger( lua, obj->id);
				lua_pushinteger( lua, i);
				if( lua_pcall( lua, 2, 0, 0) != 0) {
					fprintf( stderr, "Failed to call the callback\n%s\n", lua_tostring( lua, -1));
				}
			}

			return true;
		}
	}
	return false;
}

struct object* inventory_remove( int index, lua_State *lua) {
	struct object *result = NULL;
	if( index >= 0 && index < INVENTORY_SIZE) {
		result =inventory[ index];
		inventory[ index] = NULL;
	}

	if( result != NULL && inventoryCallbacks.onRemove != LUA_NOREF) {
		lua_rawgeti( lua, LUA_REGISTRYINDEX, inventoryCallbacks.onRemove);
		lua_pushinteger( lua, result->id);
		lua_pushinteger( lua, index);
		if( lua_pcall( lua, 2, 0, 0) != 0) {
			fprintf( stderr, "Failed to call the callback\n%s\n", lua_tostring( lua, -1));
		}
	}

	return result;
}
