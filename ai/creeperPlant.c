#include <stdlib.h> //for malloc
#include "creeperPlant.h"


struct creeperPlantData {
	uint8_t type;
	struct object* children[4];

	uint8_t clingeFlag;
};

#define TYPE_ROOT 0
#define TYPE_NODE 1
#define TYPE_SPROUT 2

#define TURNS_PER_MOVE 10

void creeperPlant_destroy(struct AI *ai) {
	//TODO needs proper deallocation
	free((struct creeperPlantData*)(ai->data));
	free(ai);
}

struct AI* creeperPlant_createDetailed(int type, bool leftClinges, bool rightClinges) {
	struct AI* ai = (struct AI*)malloc(sizeof(struct AI));

	struct creeperPlantData *data = (struct creeperPlantData*)malloc(sizeof(struct creeperPlantData));
	data->type = type;

	if(leftClinges)
		data->clingeFlag = dirFlags[dir_left];
	else
		data->clingeFlag = 0x00;
	if(rightClinges)
		data->clingeFlag = data->clingeFlag | dirFlags[dir_right];

	ai->type = ai_creeperPlant;
	ai->enabled = true;
	ai->data = data;

	return ai;
}

struct AI* creeperPlant_create() {
	return creeperPlant_createDetailed(TYPE_ROOT, false, false);
}

void addNewSprout( struct creeperPlantData *parentAiData, enum direction dirFromParent, struct Map *map, int posX, int posY, enum direction sproutDir, bool leftClinges, bool rightClinges, unsigned int health) {
	if(health > 0) {
		struct object *newPlant = createObject(go_creeperPlant, posX, posY);
		newPlant->dir = sproutDir;
		newPlant->timerCounter = TURNS_PER_MOVE;
		newPlant->health = health;

		newPlant->ai = creeperPlant_createDetailed(TYPE_SPROUT, leftClinges, rightClinges);

		parentAiData->children[dirFromParent] = newPlant;


		addObject( newPlant, map, posX, posY);
		log0("sprout at (%d,%d)\n", posX, posY);
	}
}

void creeperPlant_update( struct Map *map, struct object *obj, void *data) {
	log0("update\n");
	if(obj->health == 0) {
		creeperPlant_destroy(obj->ai);
		obj->ai = 0;
		return;
	}
	
	
	
	obj->timerCounter = TURNS_PER_MOVE;

	struct creeperPlantData *aiData = (struct creeperPlantData*)data;

	if( aiData->type == TYPE_SPROUT) {
		log0("update for sprout. dir: %d\n", obj->dir);
		/* 	LC: leftClinges
			RC: rightClinges
			LW: leftWall //there is wall at left
			RW: rightWall //there is wall at right
			FE: frontEmpty //is front empty

		*/
		bool LC = aiData->clingeFlag & dirFlags[dir_left];
		bool RC = aiData->clingeFlag & dirFlags[dir_right];
		bool LW, RW, FE;
		bool goesLeft, goesRight, goesForward;
		struct Vector leftPos, rightPos, fwdPos;


		vectorAdd( &leftPos, &obj->pos, &dirVectors[DIR_ROTATE_LEFT(obj->dir)]);
		LW = map->tiles[leftPos.i][leftPos.j] == terrain_wall;

		vectorAdd( &rightPos, &obj->pos, &dirVectors[DIR_ROTATE_RIGHT(obj->dir)]);
		RW = map->tiles[rightPos.i][rightPos.j] == terrain_wall;

		vectorAdd( &fwdPos, &obj->pos, &dirVectors[obj->dir]);
		FE = TILE_CLEAR(map, fwdPos.i, fwdPos.j);

		//TODO can replace obj-check and !LW or !RW with TILE_CLEAR(...)
		goesLeft  = TILE_CLEAR(map, leftPos.i, leftPos.j) && ( !FE || LC);
		goesRight = TILE_CLEAR(map, rightPos.i, rightPos.j) && ( !FE || RC);
		goesForward = FE && ( LW || RW || ( !LC && !RC ));

		log0("\tLC %d, RC %d, LW %d, RW %d, FE %d\n\t\tgoesLeft %d, goesRight %d, goesForward %d\n", LC, RC, LW, RW, FE, goesLeft, goesRight, goesForward);

		if( goesLeft || goesRight || goesForward) {
			aiData->type = TYPE_NODE;
			obj->ai->enabled = false;

			if(goesLeft)
				addNewSprout(aiData, dir_left, map, leftPos.i, leftPos.j, DIR_ROTATE_LEFT(obj->dir), !LW && LC, !FE && !LW, obj->health);
			else
				aiData->children[dir_left] = NULL;

			if(goesRight)
				addNewSprout(aiData, dir_right, map, rightPos.i, rightPos.j, DIR_ROTATE_RIGHT(obj->dir), !FE && !RW, !RW && RC, obj->health);
			else
				aiData->children[dir_right] = NULL;

			if(goesForward)
				addNewSprout(aiData, dir_up, map, fwdPos.i, fwdPos.j, obj->dir, FE && LW, FE && RW, obj->health - ((LC || RC) ? 0 : 1) );
			else
				aiData->children[dir_up] = NULL;
		}
		else {
			obj->health --;
		}
	}
	else if(aiData->type == TYPE_NODE) {
		log0("update for node\n");
		//TODO
	}
	else { //TYPE_ROOT
		log0("update for root. dir\n");
		int dir;
		struct Vector newPos;
		for(dir=0; dir<4; dir++) {
			struct Vector* dirVector = & dirVectors[dir];
			vectorAdd( &newPos, dirVector, &obj->pos);

			if( TILE_CLEAR( map, newPos.i, newPos.j) ){
				addNewSprout( aiData, dir, map, newPos.i, newPos.j, (obj->dir + dir)%4, false, false, obj->health);
			}
			else
				aiData->children[dir] = NULL;
		}

		obj->ai->enabled = false;
	}

	log0("update end\n");
}
