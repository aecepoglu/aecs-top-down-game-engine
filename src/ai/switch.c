#include "switch.h"
#include <stdlib.h>

#define DEAD	0
#define OFF		1
#define ON		2

struct AI* switch_create() {
	struct AI* ai = (struct AI*)malloc(sizeof(struct AI));
	ai->type = ai_switch;
	ai->enabled = false;
	ai->data = NULL;
	return ai;
}

void switch_use( struct Map *map, struct object *obj, void *data) {
	switch( obj->visualState) {
		case ON: 
			obj->visualState = OFF;
			break;
		case OFF: 
			obj->visualState = ON;
			break;
	};
}
