#ifndef AI_H
#define AI_H

#include "basic.h"
#include "object.h"
#include "map.h"


enum AIType {
	ai_none,
	ai_leftTurner,
	ai_hungryLeftTurner,
	ai_simpleFlower,
	ai_creeperPlant,
	ai_peekaboo,
	ai_weepingAngel,
	ai_door,
	ai_lineSensor,
	ai_switch,
	ai_NUM_ITEMS
};

struct AI {
	enum AIType type;
	void *data;
	bool enabled;
};

extern char *aiNames[];

#endif
