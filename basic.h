/*
	game-engine basic definitions and functions
*/

#ifndef BASIC_H
#define BASIC_H

#include <SDL.h>

typedef int bool;
enum objType { go_player, go_monster};
enum direction { dir_right, dir_top, dir_left, dir_bottom};

struct ai {
	//TODO add state variable
	void (*update)(void); //the update method of AI
	//TODO fill in here
};

struct object {
	unsigned int x,y;
	enum objType type;
	struct ai *ai;
};




#define MIN( x, y) ((x) < (y) ? (x) : (y))
#define MAX( x, y) ((x) > (y) ? (x) : (y))

struct object* createObject( enum objType type, unsigned int x, unsigned int y);

#endif //BASIC_H
