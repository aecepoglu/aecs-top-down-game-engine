#include "ai/leftTurner.h"
#include "ai.h"
#include "aiFun.h"

struct aiTableItem {
	aiCreateFun* constructor;
	aiUpdateFun* updateFun;
};


struct aiTableItem aiTable[] = {
	{ 0, 0 }, //ai_none
	{ leftTurner_create, leftTurner_update }, //ai_leftTurner
};
#define AICOUNT 1

/* table interface */
#define AI_CREATE( type) aiTable[type].constructor()
#define AI_UPDATE( type, map, object) aiTable[type].updateFun( map, object)
