#include "object.h"
#include "../util/log.h"
#include "../ai/aiTable.h"

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
	obj->healthGiven = 1;
	obj->isMovable = true;
	obj->isPickable = true;
	obj->attack = 1;
	obj->defence = 1;

	obj->timerCounter= 0;
	obj->isDeleted=false;
	obj->visualState = 1;
    obj->callbacks.onInteract = LUA_NOREF;
	obj->callbacks.onSeen = LUA_NOREF;

	return obj;
}

void writeObject( FILE *fp, struct object *obj) {
	writeVector( fp, &obj->pos);
	fwrite( &obj->type, sizeof(enum objType),   1, fp);
	fwrite( &obj->dir,  sizeof(enum direction), 1, fp);
	enum AIType tmp = obj->ai == 0 ? ai_none : obj->ai->type;
	fwrite( &tmp, 		sizeof(enum AIType),     1, fp);
    fwrite( &obj->id, sizeof(int), 1, fp);

	fwrite( &obj->health,		sizeof(uint8_t), 	1, fp);
	fwrite( &obj->maxHealth,	sizeof(uint8_t), 	1, fp);
	fwrite( &obj->healthGiven,	sizeof(int8_t), 	1, fp);
	fwrite( &obj->isMovable, 	sizeof(bool),		1, fp);
	fwrite( &obj->isPickable, 	sizeof(bool),		1, fp);
	fwrite( &obj->attack, 		sizeof(uint8_t), 	1, fp);
	fwrite( &obj->defence, 		sizeof(uint8_t), 	1, fp);
}

struct object* readObject( FILE *fp) {
	struct object *obj = (struct object*)malloc(sizeof(struct object));
	readToVector( fp, &obj->pos);
	fread( &obj->type, sizeof(enum objType),   1, fp);
	fread( &obj->dir,  sizeof(enum direction), 1, fp);
	enum AIType type;
	fread( &type,      sizeof(enum AIType),    1, fp);
	obj->ai = type != 0 ? AI_CREATE( type) : 0;

    fread( &obj->id, sizeof( int), 1, fp);

	fread( &obj->health,		sizeof(uint8_t),	1, fp);
	fread( &obj->maxHealth,		sizeof(uint8_t),	1, fp);
	fread( &obj->healthGiven,	sizeof(int8_t),		1, fp);
	fread( &obj->isMovable,		sizeof(bool),		1, fp);
	fread( &obj->isPickable,	sizeof(bool),		1, fp);
	fread( &obj->attack,		sizeof(uint8_t),	1, fp);
	fread( &obj->defence,		sizeof(uint8_t),	1, fp);

	obj->timerCounter= 0;
	obj->visualState = 1;
	obj->isDeleted=false;
    obj->callbacks.onInteract = LUA_NOREF;
    obj->callbacks.onSeen = LUA_NOREF;

	return obj;
}

/*
	Some functions objects use to interract with each other
*/


bool objectInteract( struct object *obj, struct object *obj2, lua_State *lua) {
	if( obj2->callbacks.onInteract != LUA_NOREF) {
		lua_rawgeti( lua, LUA_REGISTRYINDEX, obj2->callbacks.onInteract);

		lua_pushboolean( lua, obj == NULL ? true : obj->dir == DIR_REVERSE(obj2->dir));

		if( lua_pcall( lua, 1, 0, 0) != 0) {
			fprintf( stderr, "Failed to call the callback\n%s\n", lua_tostring( lua, -1));
			return false;
		}
		return true;
	}
	else
		return false;
}

bool objectSee( struct object *obj, struct object *obj2, lua_State *lua) {
	if( obj2->callbacks.onSeen != LUA_NOREF) {
		if( obj2->ai)
			AI_SEEN( obj2->ai);

		lua_rawgeti( lua, LUA_REGISTRYINDEX, obj2->callbacks.onSeen);

		lua_pushinteger( lua, obj->id);

		if( lua_pcall( lua, 1, 0, 0) != 0) {
			fprintf( stderr, "Failed to call the callback\n%s\n", lua_tostring( lua, -1));
			return false;
		}
		return true;
	}
	else
		return false;
}

bool objectHit( struct object *obj1, struct object *obj2) {
	log1("Obj at (%d,%d) is hit\n", obj2->pos.i, obj2->pos.j);
	int dmg = obj1->attack - obj2->defence;
	if( obj2->health > 0 && dmg > 0) {
		obj2->health = dmg > obj2->health
			? 0
			: obj2->health - dmg;
		printf("obj health down to %d\n", obj2->health);

		if( obj2->health == 0) {
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

bool objectSwallow( struct object *obj1, struct object *obj2) {
	if( obj2->health == 0) {
		obj1->health += obj2->maxHealth;
		int value = obj1->health + obj2->healthGiven;
		if( value > obj1->maxHealth)
			value = obj1->maxHealth;
		else if (value <= 0) {
			value = 0;
			AI_DESTROY( obj1->ai);
			obj1->ai = NULL;
		}
		obj1->health = value;
		obj2->isDeleted = true;
		
		return true;
	}
	else
		return false;
};

void objectFree( struct object *obj) {
	if( obj->ai) {
		AI_DESTROY( obj->ai);
	}
	free( obj);
}
