#include <stdlib.h>
#include "escaper.h"

#define MAX_SIGHT_DISTANCE 8
#define SLEEP_TICKS 10
#define TURN_TICKS 4
#define MOVE_FWD_CHANCE 10 /* so it is 1/MOVE_FWD_CHANCE */

struct EscaperData {
	struct object *targetObject;
};

void escaper_destroy(struct AI *ai) {
	free((struct EscaperData*)(ai->data));
	free(ai);
}

struct AI* escaper_create() {
	struct AI* ai = (struct AI*)malloc(sizeof(struct AI));
	ai->type = ai_escaper;
	ai->enabled = true;

	struct EscaperData *data = (struct EscaperData*)malloc(sizeof(struct EscaperData));
	data->targetObject = NULL;
	ai->data = data;

	return ai;
}

struct object* escaper_findTarget( struct Map *map, struct Vector *pos, enum direction dir) {
	printf("finding target\n");
	int i,j;
	for( i=0; i<map->width; i++) {
		for( j=0; j<map->height; j++) {
			if( map->objs[i][j] != NULL && map->objs[i][j]->type == go_player) {
				return map->objs[i][j];
			}
		}
	}
	return NULL;
}

void escaper_update( struct Map *map, struct object *obj, void *data) {
	struct EscaperData *aiData = (struct EscaperData*)data;

	if( aiData->targetObject == NULL) {
		aiData->targetObject = escaper_findTarget( map, &obj->pos, obj->dir);
		obj->timerCounter = SLEEP_TICKS;
		return;
	}

	struct object *target = aiData->targetObject;
	struct Vector diffVector;
	vectorSub( &diffVector, &target->pos, &obj->pos);
	unsigned int distance = vector_length( &diffVector);
	
	if( distance >= MAX_SIGHT_DISTANCE) {
		obj->timerCounter = SLEEP_TICKS;
		return;
	}

	enum direction potentialDirs[4];
	int potentialDirsCount = 0;

	int dir;
	struct BasePfNode *base = map->pfBase[obj->pos.i][ obj->pos.j];
	struct Vector *dirPos = NULL;

	

	for( dir=0; dir<4; dir++) {
		if( base->neighbours[dir] != NULL) {
			dirPos = &(base->neighbours[dir]->pos);
			vectorSub( &diffVector, dirPos, &aiData->targetObject->pos);

			if( vector_length( &diffVector) > distance && map->objs[ dirPos->i][ dirPos->j] == NULL) {
				if( dir == obj->dir && (random() % MOVE_FWD_CHANCE) != 0 ) {
					obj->timerCounter = TURN_TICKS;
					moveForward( map, obj);
					return;
				}
				potentialDirs[ potentialDirsCount] = dir;
				potentialDirsCount ++;
			}
		}
	}

	if( potentialDirsCount > 0) {
		enum direction selectedDir = potentialDirs[ random() % potentialDirsCount ];

		if( selectedDir == obj->dir) {
			moveForward( map, obj);
		}
		else if (selectedDir == DIR_ROTATE_RIGHT(obj->dir)) {
			turnRight( map, obj);
		}
		else {
			turnLeft( map, obj);
		}
	}
	obj->timerCounter = TURN_TICKS;
}
