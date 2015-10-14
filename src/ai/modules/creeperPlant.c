#include <stdlib.h> //for malloc
#include "creeperPlant.h"


struct creeperPlantData {
	uint8_t type;
	struct AI* children[4];

	uint8_t clingeFlag;
};

#define TYPE_ROOT 0
#define TYPE_NODE 1
#define TYPE_SPROUT 2
#define TYPE_DYING 3

#define TURNS_PER_MOVE 10

void creeperPlant_disableChildren(struct AI* ai) {
	struct creeperPlantData *data = (struct creeperPlantData*)ai->data;
	data->type = TYPE_DYING;

	int dir;
	int numChildren = 0;
	for(dir=0; dir<4; dir++) {
		if(dir != dir_down && data->children[dir] != NULL) {
			numChildren ++;
			creeperPlant_disableChildren(data->children[dir]);
		}
	}

	ai->enabled = numChildren == 0;
}

void creeperPlant_destroy(struct AI *ai) {
	struct creeperPlantData* data = (struct creeperPlantData*)ai->data;
	int dir;

	log0("destroying type %d\n", data->type);
	if(data->type != TYPE_ROOT && data->children[dir_down] != NULL) {
		log0("\tclearing parent's pointer\n");
		struct creeperPlantData* parentData = (struct creeperPlantData*)data->children[dir_down]->data;
		for(dir=0; dir<4; dir++) {
			if(parentData->children[dir] == ai) {
				parentData->children[dir] = NULL;
				break;
			}
		}
		data->children[dir_down] = NULL;
	}

	log0("destroying children\n");
	for( dir=0; dir<4; dir++) {
		if ( data->children[dir] != NULL) {
			((struct creeperPlantData*)data->children[dir]->data)->children[dir_down] = NULL;
			creeperPlant_disableChildren(data->children[dir]);
		}
	}
	free(data);
	free(ai);
	log0("destroyed\n");
}

struct AI* creeperPlant_createDetailed(int type, bool leftClinges, bool rightClinges, struct AI* parentAI) {
	struct AI* ai = (struct AI*)malloc(sizeof(struct AI));

	struct creeperPlantData *data = (struct creeperPlantData*)malloc(sizeof(struct creeperPlantData));
	data->type = type;

	if(leftClinges)
		data->clingeFlag = dirFlags[dir_left];
	else
		data->clingeFlag = 0x00;
	if(rightClinges)
		data->clingeFlag = data->clingeFlag | dirFlags[dir_right];

	int dir;
	for(dir =0; dir<4; dir++) {
		data->children[dir] = NULL;
	}
	data->children[dir_down] = parentAI;

	ai->type = ai_creeperPlant;
	ai->enabled = true;
	ai->data = data;

	return ai;
}

struct AI* creeperPlant_create() {
	return creeperPlant_createDetailed(TYPE_ROOT, false, false, NULL);
}

void addNewSprout( struct object *parentObj, enum direction dirFromParent, struct Map *map, int posX, int posY, enum direction sproutDir, bool leftClinges, bool rightClinges, unsigned int health) {
	if(health > 0) {
		struct object *newPlant = createObject(go_npc, posX, posY, parentObj->id, parentObj->textureId);
		newPlant->dir = sproutDir;
		newPlant->timerCounter = TURNS_PER_MOVE;
		newPlant->health = health;

		newPlant->ai = creeperPlant_createDetailed(TYPE_SPROUT, leftClinges, rightClinges, parentObj->ai);

		((struct creeperPlantData*)parentObj->ai->data)->children[dirFromParent] = newPlant->ai;

		addObject( newPlant, map, posX, posY);
		log0("sprout at (%d,%d)\n\tgrow from %d\n", posX, posY, dirFromParent);
	}
}

void creeperPlant_update( struct Map *map, struct object *obj, void *data) {
	obj->timerCounter = TURNS_PER_MOVE;

	struct creeperPlantData *aiData = (struct creeperPlantData*)data;
	log1("update creeperPlant. type: %d\n", aiData->type);

	if( aiData->type == TYPE_SPROUT) {
		log2("\tSprout dir: %d\n", obj->dir);
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

		struct BasePfNode *curPfNode = map->pfBase[ obj->pos.i ][ obj->pos.j ];

		LW = curPfNode->neighbours[ DIR_ROTATE_LEFT(obj->dir) ] == NULL;
		RW = curPfNode->neighbours[ DIR_ROTATE_RIGHT(obj->dir) ] == NULL;

		vectorAdd( &leftPos, &obj->pos, &dirVectors[DIR_ROTATE_LEFT(obj->dir)]);
		vectorAdd( &rightPos, &obj->pos, &dirVectors[DIR_ROTATE_RIGHT(obj->dir)]);

		vectorAdd( &fwdPos, &obj->pos, &dirVectors[obj->dir]);
		FE = TILE_CLEAR(map, fwdPos.i, fwdPos.j);

		//TODO can replace obj-check and !LW or !RW with TILE_CLEAR(...)
		goesLeft  = TILE_CLEAR(map, leftPos.i, leftPos.j) && ( !FE || LC);
		goesRight = TILE_CLEAR(map, rightPos.i, rightPos.j) && ( !FE || RC);
		goesForward = FE && ( LW || RW || ( !LC && !RC ));

		log2("\tLC %d, RC %d, LW %d, RW %d, FE %d\n\t\tgoesLeft %d, goesRight %d, goesForward %d\n", LC, RC, LW, RW, FE, goesLeft, goesRight, goesForward);

		if( goesLeft || goesRight || goesForward) {
			aiData->type = TYPE_NODE;
			obj->ai->enabled = false;

			if(goesLeft)
				addNewSprout(obj, dir_left, map, leftPos.i, leftPos.j, DIR_ROTATE_LEFT(obj->dir), !LW && LC, !FE && !LW, obj->health);

			if(goesRight)
				addNewSprout(obj, dir_right, map, rightPos.i, rightPos.j, DIR_ROTATE_RIGHT(obj->dir), !FE && !RW, !RW && RC, obj->health);

			if(goesForward)
				addNewSprout(obj, dir_up, map, fwdPos.i, fwdPos.j, obj->dir, FE && LW, FE && RW, obj->health - ((LC || RC) ? 0 : 1) );
		}
		else {
			obj->health --;
			if(obj->health == 0) {
				creeperPlant_destroy(obj->ai);
				obj->ai=0;
			}
		}
	}
	else if(aiData->type == TYPE_NODE) {
	}
	else if (aiData->type == TYPE_ROOT) {
		int dir;
		struct Vector newPos;
		for(dir=0; dir<4; dir++) {
			struct Vector* dirVector = & dirVectors[dir];
			vectorAdd( &newPos, dirVector, &obj->pos);

			if( TILE_CLEAR( map, newPos.i, newPos.j) ){
				addNewSprout( obj, dir, map, newPos.i, newPos.j, (obj->dir + dir)%4, false, false, obj->health);
			}
			else
				aiData->children[dir] = NULL;
		}

		obj->ai->enabled = false;
	}
	else /*if TYPE_DYING*/ { 

		int numChildren = 0;
		int dir;
		for(dir=0; dir<4; dir++) {
			if(dir != dir_down && aiData->children[dir] != NULL)
				numChildren ++;
		}
		log2("\tnumChildren: %d\n", numChildren);
		if(numChildren == 0) {
			if(obj->health > 0)
				obj->health --;
			log2("\t\thealth decreased to %d\n", obj->health);
			if(obj->health == 0){
				if(aiData->children[dir_down] != NULL) {
					aiData->children[dir_down]->enabled = true;
					log2("\t\t\tenabling parent\n");
				}
				creeperPlant_destroy(obj->ai);
				obj->ai = NULL;
			}
		}
		else
			obj->ai->enabled = false;
	}

	log0("update end\n");
}
