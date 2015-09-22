#include "peekaboo.h"
#include "../../pf/roughAStar.h"
#include <stdlib.h>

#define TURNSPERMOVE 4

#define STATE_SEEKING	1
#define STATE_MOVING	2

struct PeekabooData {
	struct RoughAStarData *pfData;
	struct LinkedListNode *path;
	struct object *targetObj;
};

struct AI* peekaboo_create() {
	struct AI* ai = (struct AI*)malloc(sizeof(struct AI));
	ai->type = ai_peekaboo;

	struct PeekabooData *aiData = (struct PeekabooData*)malloc( sizeof( struct PeekabooData));;
	aiData->pfData = NULL;
	aiData->path = NULL;
	aiData->targetObj = NULL;

	ai->data = aiData;
	ai->enabled = true;

	log1("peekaboo created\n");
	return ai;
}

void peekaboo_destroy(struct AI* ai) {
    struct PeekabooData *aiData = (struct PeekabooData*)ai->data;
	roughAStarData_free( aiData->pfData);
    linkedList_free( aiData->path);
	free(ai);
}

void peekaboo_update( struct Map *map, struct object *obj, void *data) {

	struct PeekabooData *aiData = (struct PeekabooData*)data;

	if( aiData->pfData) {
		if( aiData->path) {
			obj->visualState = STATE_MOVING;

			struct LinkedListNode *node = aiData->path;
			linkedList_remove( &aiData->path, node);

			moveFun *fun = (moveFun*)node->data;
			fun( map, obj);
			
			free( node);
		}
		else if( aiData->targetObj){
			log1("\tOpen eyes and find path\n");
			obj->visualState = STATE_SEEKING;
			aiData->path = roughAStar_pathfind( aiData->pfData, &obj->pos, obj->dir, &aiData->targetObj->pos);
		}
		else {
			log1("\tNothing to do for peekaboo chaser. Terminating\n");
			peekaboo_destroy( obj->ai);
			obj->ai = NULL;
		}
	}
	else {
		aiData->pfData = roughAStar_initData( map);
		
		aiData->targetObj = NULL;
		
		int i;
		for( i=0; i<map->objListCount; i++) {
			if( map->objList[i]->type ==  go_player) {
				aiData->targetObj = map->objList[i];
				log1("\tFound targetObj at %d,%d\n", aiData->targetObj->pos.i, aiData->targetObj->pos.j);
				break;
			}
		}

	}


	
	obj->timerCounter = TURNSPERMOVE;
}
