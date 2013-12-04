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
enum direction { dir_right, dir_up, dir_left, dir_down, };

/*struct AI defined in ai.h*/
struct object {
	struct Vector pos;
	enum objType type;
	struct Vector *dir;
	struct AI *ai;
};


#define MIN( x, y) ((x) < (y) ? (x) : (y))
#define MAX( x, y) ((x) > (y) ? (x) : (y))

//static struct Vector vectorLeft = { -1, 0};
//static struct Vector vectorRight = { 1, 0};
//static struct Vector vectorDown = { 0, 1};
//static struct Vector vectorUp = { 0, -1};
static struct Vector dirVectors[] = {
	[dir_right] = { 1, 0},
	[dir_left] = { -1, 0},
	[dir_up] = { 0, -1},
	[dir_down] = {0, 1},
};

/* Creates and returns an Object of given type */
struct object* createObject( enum objType type, unsigned int x, unsigned int y);
struct object* readObject( FILE *fp);
void writeObject( FILE *fp, struct object *obj);

void use( struct object *obj1, struct object *obj2);
void swallow( struct object *obj1, struct object *obj2);
void hit( struct object *obj1, struct object *obj2);

#endif //OBJECT_H
