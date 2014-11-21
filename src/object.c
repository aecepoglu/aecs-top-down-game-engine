#include "object.h"
#include "log.h"
#include "aiTable.h"
#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

struct object* createObject( enum objType type, unsigned int x, unsigned int y, unsigned int id) {
	struct object *obj = (struct object*)malloc(sizeof(struct object));
    obj->id = id;

	obj->pos.i = x;
	obj->pos.j = y;

	obj->type = type;
	obj->dir = dir_up;
	obj->ai = 0;
	obj->health = 3;
	obj->maxHealth = 3;
	obj->timerCounter= 0;
	obj->isDeleted=false;
	obj->visualState = 1;
    obj->callbacks.onInteract = LUA_NOREF;

	return obj;
}

void writeObject( FILE *fp, struct object *obj) {
	writeVector( fp, &obj->pos);
	fwrite( &obj->type, sizeof(enum objType),   1, fp);
	fwrite( &obj->dir,  sizeof(enum direction), 1, fp);
	enum aiType tmp = obj->ai == 0 ? ai_none : obj->ai->type;
	fwrite( &tmp, 		sizeof(enum aiType),     1, fp);
    fwrite( &obj->id, sizeof(int), 1, fp);
}

struct object* readObject( FILE *fp) {
	struct object *obj = (struct object*)malloc(sizeof(struct object));
	readToVector( fp, &obj->pos);
	fread( &obj->type, sizeof(enum objType),   1, fp);
	fread( &obj->dir,  sizeof(enum direction), 1, fp);
	enum aiType type;
	fread( &type,      sizeof(enum aiType),    1, fp);
	obj->ai = type != 0 ? AI_CREATE( type) : 0;

    fread( &obj->id, sizeof( int), 1, fp);

	obj->health = 3;
	obj->maxHealth = 3;
	obj->timerCounter= 0;
	obj->visualState = 1;
	obj->isDeleted=false;
    obj->callbacks.onInteract = LUA_NOREF;

	return obj;
}

/*
	Some functions objects use to interract with each other
*/


void objectInteract( struct object *obj, struct object *obj2, lua_State *lua) {
	if( obj2->callbacks.onInteract != LUA_NOREF) {
		lua_rawgeti( lua, LUA_REGISTRYINDEX, obj2->callbacks.onInteract);

		lua_pushboolean( lua, obj == NULL ? true : obj->dir == DIR_REVERSE(obj2->dir));

		if( lua_pcall( lua, 1, 0, 0) != 0) {
			fprintf( stderr, "Failed to call the callback\n%s\n", lua_tostring( lua, -1));
			return;
		}
	}
}

bool objectHit( struct object *obj1, struct object *obj2) {
	log1("Obj at (%d,%d) is hit\n", obj2->pos.i, obj2->pos.j);
	if( obj2->health > 0) {
		obj2->health -= 1;

		if( obj2->health ==0) {
			if( obj2->ai) {
				AI_DESTROY( obj2->ai);
				obj2->ai = NULL;
			}
			obj2->visualState = 0; //0 is always the state for dead
		}
			

		return true;
	}
	else {
		return false;
	}
}

void objectSwallow( struct object *obj1, struct object *obj2) {
	if( obj2->health == 0) {
		obj1->health += obj2->maxHealth;
		obj2->isDeleted = true;
	}
}

void objectFree( struct object *obj) {
	if( obj->ai) {
		AI_DESTROY( obj->ai);
	}
	free( obj);
}
