#ifndef GAME_H
#define GAME_H
/* not to be included everywhere. Only used by the very core files */

#include "basic.h"
#include "fov/fov.h"
#include "move.h"

/* when a level is read, player position needs to be set from within the level script.
	until then, this variable stays false */
bool isPlayerPosSet;
struct object *player;

char *dirPath;
fovFun *currentFov;

#define KEY_LOOKUP_SIZE 283
moveFun *keyLookup[KEY_LOOKUP_SIZE];

void setViewRange(int value);

void level_startTimer();
void level_endTimer();

void gameOver( int levelEndValue);
int run();


#endif /*GAME_H*/
