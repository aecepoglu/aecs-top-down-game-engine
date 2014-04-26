#include "leftTurner.h"
#include <stdlib.h>

#define TURNSPERMOVE 4

struct AI* leftTurner_create() {
	struct AI* ai = (struct AI*)malloc(sizeof(struct AI));
	ai->type = ai_leftTurner;
	ai->data = 0;
	return ai;
}

void leftTurner_update( struct Map *map, struct object *obj, void *data) {
	//try to move forward
	if( obj->health > 0) {
		if( ! moveForward( map, obj))
			//if can't move forward; turn-left
			turnLeft( map, obj);
	}
	
	obj->timerCounter = TURNSPERMOVE;
}
