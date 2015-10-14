#include "simpleFlower.h"
#include <stdlib.h>


struct simpleFlowerData {
	uint8_t state;
};


#define HEALTH_GROWN 10
#define TURNS_PER_MOVE_YOUNG 10
#define TURNS_PER_MOVE_GROWN 100


struct AI* simpleFlower_create() {
	struct AI* ai = (struct AI*)malloc(sizeof(struct AI));
	ai->type = ai_simpleFlower;
	ai->data = NULL;
	ai->enabled = true;

	return ai;
}

void simpleFlower_update( struct Map *map, struct object *obj, void *data) {
	
	if(obj->health < HEALTH_GROWN) {
		log0("flower AI growing at (%d,%d)\n", obj->pos.i, obj->pos.j);
		obj->health ++;
		if(obj->health == HEALTH_GROWN)
			obj->visualState = 2;

		obj->timerCounter = TURNS_PER_MOVE_YOUNG;
	}
	else {
		if(rand() % 4 == 0) {
			log0("\tflower lucky\n");
			int xDiff = (rand() % 5) - 2;
			int xPos = obj->pos.i + xDiff;

			int yMod = 5 - 2*abs(xDiff);
			int yPos = obj->pos.j + rand()%yMod - yMod/2;

			if( TILE_CLEAR(map, xPos, yPos) ) {
				struct object *newFlower = createObject(go_npc, xPos, yPos, obj->id, obj->textureId);
				newFlower->ai = simpleFlower_create();
				addObject( newFlower, map, xPos, yPos);
			}
		}
		obj->timerCounter = TURNS_PER_MOVE_GROWN;
	}
}
