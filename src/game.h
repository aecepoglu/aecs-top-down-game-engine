#ifndef GAME_H
#define GAME_H
/* not to be included everywhere. Only used by the very core files */

#include "basic.h"

/* when a level is read, player position needs to be set from within the level script.
	until then, this variable stays false */
bool isPlayerPosSet;
struct object *player;

char *dirPath;


void level_startTimer();
void level_endTimer();

void gameOver( int levelEndValue);
int run();


#endif /*GAME_H*/
