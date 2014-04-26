/*
	game-engine basic definitions and functions
*/

#ifndef OBJECT_H
#define OBJECT_H

#include <SDL.h>
#include <stdio.h>
#include "ai.h"
#include "basic.h"
#include "vector.h"

enum objType { go_player, go_monster, go_misc};
enum direction { dir_up, dir_right, dir_down, dir_left};

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
};


#define MIN( x, y) ((x) < (y) ? (x) : (y))
#define MAX( x, y) ((x) > (y) ? (x) : (y))

//static struct Vector vectorLeft = { -1, 0};
//static struct Vector vectorRight = { 1, 0};
//static struct Vector vectorDown = { 0, 1};
//static struct Vector vectorUp = { 0, -1};
static struct Vector dirVectors[] = {
	[dir_up] = { 0, -1},
	[dir_right] = { 1, 0},
	[dir_down] = {0, 1},
	[dir_left] = { -1, 0},
};

/* Creates and returns an Object of given type */
struct object* createObject( enum objType type, unsigned int x, unsigned int y);
struct object* readObject( FILE *fp);
void writeObject( FILE *fp, struct object *obj);

void objectUse( struct object *obj1, struct object *obj2);
void objectSwallow( struct object *obj1, struct object *obj2);
bool objectHit( struct object *obj1, struct object *obj2);

#endif //OBJECT_H
