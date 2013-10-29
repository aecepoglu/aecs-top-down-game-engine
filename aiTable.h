#ifndef AITABLE_H
#define AITABLE_H

#include "ai/leftTurner.h"
#include "ai.h"
#include "aiFun.h"

struct aiTableItem {
	aiCreateFun* constructor;
	aiUpdateFun* updateFun;
};


static struct aiTableItem aiTable[] = {
	{ 0, 0 }, //ai_none
	{ leftTurner_create, leftTurner_update }, //ai_leftTurner
};

/* table interface */
#define AI_CREATE( type) aiTable[type].constructor()
#define AI_UPDATE( map, obj) aiTable[ obj->ai->type].updateFun( map, obj, obj->ai->data)


#endif
