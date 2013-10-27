/*
	game-engine basic definitions and functions
*/

#ifndef OBJECT_H
#define OBJECT_H

#include <SDL.h>
#include <stdio.h>
#include "ai.h"
#include "basic.h"

enum objType { go_player, go_monster, };
enum direction { dir_right, dir_up, dir_left, dir_down, };

/*struct AI defined in ai.h*/
struct object {
	unsigned int x,y;
	enum objType type;
	enum direction dir;
	struct AI *ai;
};


#define MIN( x, y) ((x) < (y) ? (x) : (y))
#define MAX( x, y) ((x) > (y) ? (x) : (y))

/* Creates and returns an Object of given type */
struct object* createObject( enum objType type, unsigned int x, unsigned int y);
struct object* readObject( FILE *fp);
void writeObject( FILE *fp, struct object *obj);

#endif //OBJECT_H
