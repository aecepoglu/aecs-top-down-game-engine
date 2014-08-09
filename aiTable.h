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
	aiSeenFun* seenFun;
};


static struct aiTableItem aiTable[] = {
	[ai_none] = 			{ NULL, 					NULL, 						NULL,						NULL},
	[ai_leftTurner] = 		{ leftTurner_create, 		leftTurner_update, 			common_ai_destroy,			NULL},
	[ai_hungryLeftTurner] = { hungryLeftTurner_create, 	hungryLeftTurner_update,	hungryLeftTurner_destroy,	NULL},
	[ai_simpleFlower] = 	{ simpleFlower_create, 		simpleFlower_update,		common_ai_destroy,			NULL},
	[ai_creeperPlant] = 	{ creeperPlant_create, 		creeperPlant_update,		creeperPlant_destroy,		NULL},
	[ai_peekaboo] = 		{ peekaboo_create, 			peekaboo_update,			peekaboo_destroy,			NULL},
};

/* table interface */
#define AI_CREATE( type) aiTable[type].constructor()
#define AI_UPDATE( map, obj) aiTable[ obj->ai->type].updateFun( map, obj, obj->ai->data)
#define AI_DESTROY( ai) aiTable[ ai->type].destructor( ai)
#define AI_SEEN( ai) if( aiTable[ ai->type].seenFun) aiTable[ ai->type].seenFun( ai)


#endif
