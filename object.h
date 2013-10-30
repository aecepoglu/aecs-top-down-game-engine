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
	enum direction dir;
	struct AI *ai;
};


#define MIN( x, y) ((x) < (y) ? (x) : (y))
#define MAX( x, y) ((x) > (y) ? (x) : (y))


static struct Vector dirVectors[] = {
	{1, 0}, {0, -1}, {-1, 0}, {0, 1}
};

/* Creates and returns an Object of given type */
struct object* createObject( enum objType type, unsigned int x, unsigned int y);
struct object* readObject( FILE *fp);
void writeObject( FILE *fp, struct object *obj);

#endif //OBJECT_H
