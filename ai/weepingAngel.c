#include "weepingAngel.h"
#include "../pf/roughAStar.h"

#define TURNSPERMOVE 2

#define STATE_DEAD 0
#define STATE_FAR 1
#define STATE_CLOSE 2
#define STATE_NEAR 3

struct WeepingAngelData {
	struct RoughAStarData *pfData;
	struct object *targetObj;
	bool seen;
};

struct AI* weepingAngel_create() {
	struct AI* ai = (struct AI*)malloc(sizeof(struct AI));
	ai->type = ai_weepingAngel;

	struct WeepingAngelData *aiData = (struct WeepingAngelData*)malloc( sizeof( struct WeepingAngelData));;
	aiData->pfData = NULL;
	aiData->targetObj = NULL;
	aiData->seen = false;

	ai->data = aiData;
	ai->enabled = true;

	log1("weeping angel created\n");
	return ai;
}

void weepingAngel_destroy(struct AI* ai) {
    //struct WeepingAngelData *aiData = (struct WeepingAngelData*)ai->data;
	//TODO free aiData->pfData as well
	free(ai);
}

void weepingAngel_update( struct Map *map, struct object *obj, void *data) {

	struct WeepingAngelData *aiData = (struct WeepingAngelData*)data;

	if( aiData->targetObj == NULL) {
		int i;
		for( i=0; i<map->objListCount; i++) {
			if( map->objList[i]->type ==  go_player) {
				aiData->targetObj = map->objList[i];
				log1("\tFound targetObj at %d,%d\n", aiData->targetObj->pos.i, aiData->targetObj->pos.j);

				aiData->pfData = roughAStar_initData( map);

				break;
			}
		}

		if( aiData->targetObj == NULL){
			obj->visualState = STATE_DEAD;
			weepingAngel_destroy( obj->ai);
			obj->ai = NULL;
			return;
		}
	}
	else if( aiData->seen) {
		aiData->seen = false;
		obj->timerCounter = 10;
		return;
	}
	/* at every update, re-calculates the path and takes one step on it
		TODO needs optimizing
	*/
	struct LinkedListNode *path = roughAStar_pathfind( aiData->pfData, &obj->pos, obj->dir, &aiData->targetObj->pos);
	if( path) {
		moveFun *fun = (moveFun*)path->data;
		fun( map, obj);

		linkedList_free( path);

		//TODO change visuals depending on how far the target object is
		obj->visualState = STATE_NEAR;
	}

	obj->timerCounter = TURNSPERMOVE;
}

void weepingAngel_seen( struct AI *ai) {
	log1("weeping angel is seen\n");
	((struct WeepingAngelData*)ai->data)->seen = true;
}
