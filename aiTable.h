#ifndef AITABLE_H
#define AITABLE_H

#include "ai/leftTurner.h"
#include "ai/hungryLeftTurner.h"
#include "ai/simpleFlower.h"
#include "ai/creeperPlant.h"
#include "ai/peekaboo.h"

#include "ai.h"
#include "aiFun.h"

struct aiTableItem {
	aiCreateFun* constructor;
	aiUpdateFun* updateFun;
	aiDestroyFun* destructor;
};


static struct aiTableItem aiTable[] = {
	[ai_none] = 			{ 0, 						0, 							common_ai_destroy},
	[ai_leftTurner] = 		{ leftTurner_create, 		leftTurner_update, 			common_ai_destroy},
	[ai_hungryLeftTurner] = { hungryLeftTurner_create, 	hungryLeftTurner_update,	hungryLeftTurner_destroy},
	[ai_simpleFlower] = 	{ simpleFlower_create, 		simpleFlower_update,		common_ai_destroy},
	[ai_creeperPlant] = 	{ creeperPlant_create, 		creeperPlant_update,		creeperPlant_destroy},
	[ai_peekaboo] = 		{ peekaboo_create, 			peekaboo_update,			peekaboo_destroy},
};

/* table interface */
#define AI_CREATE( type) aiTable[type].constructor()
#define AI_UPDATE( map, obj) aiTable[ obj->ai->type].updateFun( map, obj, obj->ai->data)
#define AI_DESTROY( ai) aiTable[ ai->type].destructor( ai)


#endif
