#ifndef AI_H
#define AI_H

#include "basic.h"
#include "object.h"
#include "map.h"


enum aiType {
	ai_none,
	ai_leftTurner,
	ai_hungryLeftTurner,
	ai_simpleFlower,
	ai_creeperPlant,
	ai_peekaboo,
	ai_weepingAngel,
	ai_door,
	ai_lineSensor,
};

struct AI {
	enum aiType type;
	void *data;
	bool enabled;
};


#endif
