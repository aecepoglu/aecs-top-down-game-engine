#include "object.h"
#include "../util/log.h"
#include "../ai/aiTable.h"

#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>

struct object* createObject( enum objType type, unsigned int x, unsigned int y, unsigned int id, int textureId) {
	struct object *obj = (struct object*)malloc(sizeof(struct object));
    obj->id = id;

	obj->pos.i = x;
	obj->pos.j = y;

	obj->type = type;
	obj->textureId = textureId;
	obj->dir = dir_up;
	obj->ai = 0;
	obj->health = 3;
	obj->maxHealth = 3;
	obj->isMovable = true;
	obj->isPickable = true;
	obj->attack = 1;
	obj->defence = 1;

	obj->width = 1;
	obj->height = 1;

	obj->timerCounter= 0;
	obj->isDeleted=false;
	obj->visualState = 1;
	obj->callbacks.onInteract = LUA_NOREF;
	obj->callbacks.onEaten = LUA_NOREF;

	return obj;
}

void writeObject( FILE *fp, struct object *obj) {
	writeVector( fp, &obj->pos);
	fwrite( &obj->textureId, sizeof(int),   1, fp);
	fwrite( &obj->dir,  sizeof(enum direction), 1, fp);
	enum AIType tmp = obj->ai == 0 ? ai_none : obj->ai->type;
	fwrite( &tmp, 		sizeof(enum AIType),     1, fp);
	fwrite( &obj->id, sizeof(int), 1, fp);

	fwrite( &obj->health,		sizeof(uint8_t), 	1, fp);
	fwrite( &obj->maxHealth,	sizeof(uint8_t), 	1, fp);
	fwrite( &obj->isMovable, 	sizeof(bool),		1, fp);
	fwrite( &obj->isPickable, 	sizeof(bool),		1, fp);
	fwrite( &obj->attack, 		sizeof(uint8_t), 	1, fp);
	fwrite( &obj->defence, 		sizeof(uint8_t), 	1, fp);

	fwrite (&obj->width,            sizeof(int),            1, fp);
	fwrite (&obj->height,           sizeof(int),            1, fp);
}

struct object* readObject( FILE *fp) {
	struct object *obj = (struct object*)malloc(sizeof(struct object));
	readToVector( fp, &obj->pos);
	fread( &obj->textureId,         sizeof(int),            1, fp);
	fread( &obj->dir,               sizeof(enum direction), 1, fp);
	enum AIType type;
	fread( &type,                   sizeof(enum AIType),    1, fp);

	fread( &obj->id,                sizeof( int),           1, fp);

	fread( &obj->health,		sizeof(uint8_t),	1, fp);
	fread( &obj->maxHealth,		sizeof(uint8_t),	1, fp);
	fread( &obj->isMovable,		sizeof(bool),		1, fp);
	fread( &obj->isPickable,	sizeof(bool),		1, fp);
	fread( &obj->attack,		sizeof(uint8_t),	1, fp);
	fread( &obj->defence,		sizeof(uint8_t),	1, fp);

	fread(&obj->width,              sizeof(int),            1, fp);
	fread(&obj->height,             sizeof(int),            1, fp);

	if (obj->width > 1 || obj->height > 1) {
		obj->isMovable = false;
		obj->isPickable = false;
		obj->ai = NULL;
	}
	else {
		obj->ai = type != 0 ? AI_CREATE( type) : 0;
	}


	obj->type = go_npc;
	obj->timerCounter= 0;
	obj->visualState = 1;
	obj->isDeleted=false;
	obj->callbacks.onInteract = LUA_NOREF;
	obj->callbacks.onEaten = LUA_NOREF;

	return obj;
}

/*
	Some functions objects use to interract with each other
*/

bool triggerObjectCallback( lua_State *lua, int luaRef, const struct object *triggeredObj, const struct object *triggeringObj ) {
	lua_rawgeti(lua, LUA_REGISTRYINDEX, luaRef);

	lua_pushinteger(lua, triggeredObj->id);
	lua_pushinteger(lua, triggeringObj ? triggeringObj->id : -1);

	if( lua_pcall( lua, 2, 0, 0) != 0) {
		fprintf( stderr, "Failed to call the callback\n%s\n", lua_tostring( lua, -1));
		return false;
	}
	else
		return true;
}

bool objectInteract( struct object *obj, struct object *obj2, lua_State *lua) {
	if( obj2->callbacks.onInteract != LUA_NOREF) {
		return triggerObjectCallback(lua, obj2->callbacks.onInteract, obj2, obj);
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
		log1("obj health down to %d\n", obj2->health);

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

bool objectSwallow( struct object *obj1, struct object *obj2, lua_State *lua) {
	if( obj2->health == 0) {
		if (obj2->callbacks.onEaten != LUA_NOREF) {
			triggerObjectCallback(lua, obj2->callbacks.onEaten, obj2, obj1);
		}

		if (obj1->health < 0) {
			AI_DESTROY( obj1->ai);
			obj1->ai = NULL;
		}

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
