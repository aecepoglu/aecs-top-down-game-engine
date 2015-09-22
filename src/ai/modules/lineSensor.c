#include "lineSensor.h"
#include "../../dsl.h"
#include <stdlib.h>

#define TURNS_PER_MOVE 1

struct LineSensorData {
	int posX, posY;
	bool isTriggered;
};

struct AI* lineSensor_create() {
	struct AI* ai = (struct AI*)malloc(sizeof(struct AI));
	ai->type = ai_lineSensor;

	struct LineSensorData *data = (struct LineSensorData*)malloc( sizeof( struct LineSensorData));
	data->isTriggered = false;

	ai->data = data;
	ai->enabled = true;
	return ai;
}

void lineSensor_destroy( struct AI *ai) {
	free( (struct LineSensorData*)(ai->data) );
	free( ai);
}

bool lineLook( struct Map *map, struct object *obj, struct LineSensorData *aiData) {
	struct BasePfNode *base;
	for( base = map->pfBase[ obj->pos.i][ obj->pos.j]->neighbours[ obj->dir]; base != NULL; base = base->neighbours[ obj->dir]) {
		if( map->objs[ base->pos.i][ base->pos.j] != NULL) {
			objectInteract( map->objs[ base->pos.i][ base->pos.j], obj, lua);
			aiData->posX = base->pos.i;
			aiData->posY = base->pos.j;
			return true;
		}
	}
	return false;
}

void lineSensor_update( struct Map *map, struct object *obj, void *data) {
	struct LineSensorData *aiData = (struct LineSensorData*)data;

	if( aiData->isTriggered) {
		if( map->objs[ aiData->posX][ aiData->posY] == NULL &&  lineLook( map, obj, aiData) != true) {
			objectInteract( NULL, obj, lua);
			aiData->isTriggered = false;
		}
	}
	else if( lineLook( map, obj, aiData)) {
		aiData->isTriggered = true;
	}

	obj->timerCounter = TURNS_PER_MOVE;
}
