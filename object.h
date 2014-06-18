/*
	game-object related stuff
*/

#ifndef OBJECT_H
#define OBJECT_H

#include <SDL.h>
#include <stdio.h>
#include "ai.h"
#include "basic.h"
#include "vector.h"

enum objType { go_player, go_leftTurner, go_apple, go_flower, go_creeperPlant, go_peekaboo, go_NUM_ITEMS};

/*struct AI defined in ai.h*/
struct object {
	struct Vector pos;
	enum objType type;
	enum direction dir;
	struct AI *ai;
	uint8_t health;
	uint8_t maxHealth;
	unsigned int timerCounter;
	bool isDeleted;
	unsigned int visualState;
};




/* Creates and returns an Object of given type */
struct object* createObject( enum objType type, unsigned int x, unsigned int y);
struct object* readObject( FILE *fp);
void writeObject( FILE *fp, struct object *obj);

void objectUse( struct object *obj1, struct object *obj2);
void objectSwallow( struct object *obj1, struct object *obj2);
bool objectHit( struct object *obj1, struct object *obj2);

#endif //OBJECT_H
