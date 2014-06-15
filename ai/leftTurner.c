#include "leftTurner.h"
#include <stdlib.h>

#define TURNSPERMOVE 4

void common_ai_destroy(struct AI* ai) {
	free(ai);
}

struct AI* leftTurner_create() {
	struct AI* ai = (struct AI*)malloc(sizeof(struct AI));
	ai->type = ai_leftTurner;
	ai->data = 0;
	ai->enabled = true;
	return ai;
}

void leftTurner_update( struct Map *map, struct object *obj, void *data) {
	if(obj->health == 0)
		return;

	if( ! moveForward( map, obj))
		turnLeft( map, obj);
	
	obj->timerCounter = TURNSPERMOVE;
}
