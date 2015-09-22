#include <stdlib.h>
#include "weepingAngel.h"
#include "../../pf/roughAStar_depthLimited.h"
#include "../../collection/stack.h"

#define TURNSPERMOVE 2

#define STATE_DEAD 0
#define STATE_FAR 1
#define STATE_CLOSE 2
#define STATE_NEAR 3

#define PF_MAX_DEPTH 20
#define PF_MAX_DEPTH_DIV_3 7 //PF_MAX_DEPTH divided by this should result in values of visual states (except STATE_DEAD)

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
    struct WeepingAngelData *aiData = (struct WeepingAngelData*)ai->data;
	roughAStarData_free( aiData->pfData);
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
		obj->timerCounter = rand()%10 + 1;
		log1("weeping angel will wake up in %d\n", obj->timerCounter);
		return;
	}
	/* at every update, re-calculates the path and takes one step on it
		TODO needs optimizing
	*/
	moveFun *path[PF_MAX_DEPTH];
	int pathLen;
	if( roughAStar_dl_pathfind( aiData->pfData, &obj->pos, obj->dir, &aiData->targetObj->pos, PF_MAX_DEPTH, path, &pathLen)) {
		moveFun *fun;
		STACK_POP( path, fun, pathLen);
		fun( map, obj);

		int newVisual = pathLen / PF_MAX_DEPTH_DIV_3;
		obj->visualState = 3 - (newVisual <= 3 ? newVisual : 2);
	}
	else {
		obj->visualState = STATE_FAR;
	}

	obj->timerCounter = TURNSPERMOVE;
}

void weepingAngel_seen( struct AI *ai) {
	((struct WeepingAngelData*)ai->data)->seen = true;
}
