#include "ai/leftTurner.h"
#include "ai.h"

struct aiTableItem {
	enum aiType type;
	aiCreateFun* constructor;
};


struct aiTableItem aiTable[] = {
	{ ai_leftTurner, leftTurner_create },
};
#define AICOUNT 1
