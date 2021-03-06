#include "door.h"
#include <stdlib.h>

#define INVISIBLE 0
#define VISIBLE 1

struct doorData {
	uint8_t state;
};

void door_destroy(struct AI* ai) {
	struct doorData *data = (struct doorData*)ai->data;
	free( data);
	free(ai);
}

struct AI* door_create() {
	struct AI* ai = (struct AI*)malloc(sizeof(struct AI));
	ai->type = ai_door;
	ai->enabled = false;

	struct doorData *data = (struct doorData*)malloc( sizeof( struct doorData));
	data->state = VISIBLE;

	ai->data = data;
	return ai;
}

void door_use( struct Map *map, struct object *obj, void *data) {
	struct doorData *aiData = (struct doorData*)data;

	switch( aiData->state) {
		case INVISIBLE: 
            if( map->objs[ obj->pos.i][ obj->pos.j] == NULL) {
				map->objs[ obj->pos.i][ obj->pos.j] = obj;
            }
			aiData->state = VISIBLE;
			break;

		case VISIBLE: 
			map->objs[ obj->pos.i][ obj->pos.j] = NULL;
			aiData->state = INVISIBLE;
			break;
	};
}
