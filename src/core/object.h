/*
	game-object related stuff
*/

#ifndef OBJECT_H
#define OBJECT_H

#include <stdio.h>
#include <lua.h>

#include "../ai/ai.h"
#include "../basic.h"
#include "../vector.h"

#define PLAYER_TEXTURE_ID 0

enum objType {
	go_player,
	go_npc
};

/*struct AI defined in ai.h*/
struct object {
	unsigned int id;
	struct Vector pos;
	enum objType type;
	int textureId;
	enum direction dir;
	struct AI *ai;
	unsigned int timerCounter;
	bool isDeleted;
	unsigned int visualState;

	uint8_t health;
	uint8_t maxHealth;
	bool isMovable;
	bool isPickable;
	uint8_t attack;
	uint8_t defence;

	struct {
		int onInteract;
		int onEaten;
	} callbacks;
};




/* Creates and returns an Object of given type */
struct object* createObject( enum objType type, unsigned int x, unsigned int y, unsigned int id, int textureId);
struct object* readObject( FILE *fp);
void writeObject( FILE *fp, struct object *obj);

bool objectInteract( struct object *obj1, struct object *obj2, lua_State *lua);
bool objectSwallow( struct object *obj1, struct object *obj2, lua_State *lua);
bool objectHit( struct object *obj1, struct object *obj2);

void objectFree( struct object *obj);

#endif //OBJECT_H
