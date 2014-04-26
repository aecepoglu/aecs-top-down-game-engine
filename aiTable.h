#ifndef AITABLE_H
#define AITABLE_H

#include "ai/leftTurner.h"
#include "ai/hungryLeftTurner.h"
#include "ai.h"
#include "aiFun.h"

struct aiTableItem {
	aiCreateFun* constructor;
	aiUpdateFun* updateFun;
};


static struct aiTableItem aiTable[] = {
	[ai_none] = 			{ 0, 						0 						},
	[ai_leftTurner] = 		{ leftTurner_create, 		leftTurner_update 		},
	[ai_hungryLeftTurner] = { hungryLeftTurner_create, 	hungryLeftTurner_update },
};

/* table interface */
#define AI_CREATE( type) aiTable[type].constructor()
#define AI_UPDATE( map, obj) aiTable[ obj->ai->type].updateFun( map, obj, obj->ai->data)


#endif
