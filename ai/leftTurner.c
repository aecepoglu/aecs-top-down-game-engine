#include "leftTurner.h"

struct AI* leftTurner_create() {
	struct AI* ai = (struct AI*)malloc(sizeof(struct AI));
	ai->type = ai_leftTurner;
	ai->data = 0;
	//ai->update = leftTurner_update;
	return ai;
}

void leftTurner_update( struct Map *map, struct object *obj, void *data) {
	//try to move forward
	if( ! moveForward( map, obj))
		//if can't move forward; turn-left
		turnLeft( map, obj);
}
