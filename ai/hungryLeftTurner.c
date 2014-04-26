#include <stdlib.h> //for malloc
#include "hungryLeftTurner.h"

struct hungryLeftTurnerData {
	uint8_t state;
	int goalDistance;
};

#define STATE_CALM 0
#define STATE_EXCITED 1
#define TURNS_PER_MOVE_CALM 4
#define TURNS_PER_MOVE_EXCITED 1

struct AI* hungryLeftTurner_create() {
	struct AI* ai = (struct AI*)malloc(sizeof(struct AI));
	ai->type = ai_hungryLeftTurner;

	struct hungryLeftTurnerData *data = (struct hungryLeftTurnerData*)malloc(sizeof(struct hungryLeftTurnerData));
	data->state = STATE_CALM;
	ai->data = data;

	return ai;
}

int hungryLeftTurner_look( struct Map *map, struct Vector *pos, enum direction dir) {
	struct Vector cur;
	vectorClone( pos, &cur);
	struct Vector *dirVector = &dirVectors[dir];
	int count = 0;
	bool found = false;
	struct object *obj;
	do {
		vectorAdd(&cur, dirVector, &cur);
		count ++;
		obj = map->objs[cur.i][cur.j];
		if( obj != 0 && obj->type == go_player) {
			found = true;
			break;
		}
	}
	while( map->tiles[cur.i][cur.j] == terrain_none && found != true);

	if( found )
		return count;
	else
		return 0;
}

void hungryLeftTurner_update( struct Map *map, struct object *obj, void *data) {
	struct hungryLeftTurnerData *aiData = (struct hungryLeftTurnerData*)data;

	int leftDistance = hungryLeftTurner_look( map, &obj->pos, (obj->dir -1)%4);
	if( leftDistance > 0) {
		log2("hungryLeftTurner: found target %d away\n", leftDistance);
		aiData->goalDistance = leftDistance;
		aiData->state = STATE_EXCITED;
		turnLeft( map, obj);
		goto excited_end;
	}
	
	switch( aiData->state) {
		case STATE_CALM:
			if( moveForward(map, obj) )
				goto calm_end;
			else
				goto calm_turnLeft;
			break;
		case STATE_EXCITED:
			log2("hungryLeftTurner: state excited\ngoalDistance > 0 : %s\n", aiData->goalDistance > 0 ? "true" : "false");
			if( aiData->goalDistance > 0 && moveForward(map, obj) )
				goto excited_end;
			else {
				aiData->state = STATE_CALM;
				goto calm_end;
			}
			break;
	};

	calm_turnLeft:
		turnLeft(map, obj);

	calm_end:
		obj->timerCounter = TURNS_PER_MOVE_CALM;
		return;

	excited_end:
		obj->timerCounter = TURNS_PER_MOVE_EXCITED;
		return;
}
